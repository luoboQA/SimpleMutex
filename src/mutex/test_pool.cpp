//
// Created by Rascal_X on 2026/6/26.
// 信号量池控制资源的分配,最多 N 个线程同时执行
#include "semaphore_pool.h"
#include "simple_mutex.h"
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>

int main() {
    SemaphorePool pool(3, 1);
    SimpleMutex data_mutex;   
    int counter = 0;
    
    std::cout << "可用: " << pool.available() << "/" << pool.total() << "\n" << std::endl;
    
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; i++) {
        threads.emplace_back([&pool, &data_mutex, &counter, i]() {
            std::cout << "线程" << i << " 等待资源..." << std::endl;
            int id = pool.acquire();
            if (id != -1) {
                std::cout << "线程" << i << " 获得资源" << id << std::endl;
                
                data_mutex.lock();
                counter++;
                std::cout << "  线程" << i << " counter = " << counter << std::endl;
                data_mutex.unlock();
                
                std::this_thread::sleep_for(std::chrono::seconds(2));
                
                pool.release(id);
                std::cout << "线程" << i << " 释放资源" << id << std::endl;
            }
        });
    }
    
    for (auto& t : threads) t.join();
    
    std::cout << "\n最终 counter = " << counter << std::endl;
    std::cout << "可用: " << pool.available() << "/" << pool.total() << std::endl;
    return 0;
}