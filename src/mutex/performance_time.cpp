#include "semaphore_pool.h"
#include "simple_mutex.h"
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>

void test_semaphore_pool() {
    SemaphorePool pool(3, 1);
    SimpleMutex data_mutex;
    int counter = 0;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<std::thread> threads;
    for (int i = 0; i < 30; i++) {
        threads.emplace_back([&pool, &data_mutex, &counter, i]() {
            int id = pool.acquire();
            if (id != -1) {
                for (int j = 0; j < 5000000; j++) {
                    volatile int x = j * j;
                }
                data_mutex.lock();
                counter++;
                data_mutex.unlock();
                pool.release(id);
            }
        });
    }
    
    for (auto& t : threads) t.join();
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "信号量池: counter = " << counter << ", 耗时: " << duration.count() << "ms" << std::endl;
}

void test_mutex() {
    SimpleMutex mutex;
    int counter = 0;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<std::thread> threads;
    for (int i = 0; i < 30; i++) {
        threads.emplace_back([&mutex, &counter]() {
            mutex.lock();
            for (int j = 0; j < 5000000; j++) {
                volatile int x = j * j;
            }
            counter++;
            mutex.unlock();
        });
    }
    
    for (auto& t : threads) t.join();
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "互斥锁: counter = " << counter << ", 耗时: " << duration.count() << "ms" << std::endl;
}

int main() {
    std::cout << "=== 性能对比（30个任务，3个资源）===" << std::endl;
    std::cout << std::endl;
    
    test_semaphore_pool();
    test_mutex();
    
    return 0;
}