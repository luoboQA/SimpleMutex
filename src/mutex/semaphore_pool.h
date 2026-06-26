#pragma once

#include "sema.h"
#include <vector>
#include <mutex>
#include <condition_variable>

class SemaphorePool {
private:
    struct Resource {
        Semaphore* sem;
        bool used;
        int id;
        
        Resource(int init_count, int id) : sem(new Semaphore(init_count)), used(false), id(id) {}
        ~Resource() { delete sem; }
    };
    
    std::vector<Resource> resources_;
    std::mutex mutex_;
    std::condition_variable cv_;  
    
public:
    SemaphorePool(int pool_size, int init_count = 1) {
        resources_.reserve(pool_size);
        for (int i = 0; i < pool_size; i++) {
            resources_.emplace_back(init_count, i);
        }
    }
    
    ~SemaphorePool() = default;
    
    int acquire() {
        std::unique_lock<std::mutex> lock(mutex_);
        while (true) {
            for (auto& r : resources_) {
                if (!r.used) {
                    r.used = true;
                    r.sem->wait();
                    return r.id;
                }
            }
            
            cv_.wait(lock);
        }
    }
    
    void release(int id) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& r : resources_) {
            if (r.id == id && r.used) {
                r.sem->signal();
                r.used = false;
                cv_.notify_one();  
                return;
            }
        }
    }
    
    int available() {
        std::lock_guard<std::mutex> lock(mutex_);
        int count = 0;
        for (auto& r : resources_) {
            if (!r.used) count++;
        }
        return count;
    }
    
    int total() const {
        return resources_.size();
    }
};