#include "../include/RingBuffer.hpp"
#include "../include/MPMCqueue.hpp"
#include <iostream>
#include <thread>
#include <functional>
#include <mutex>
#include <assert.h>

void test_mpmc_queue() {
    constexpr size_t QUEUE_CAPACITY = 1024;
    constexpr size_t NUM_PRODUCERS = 4;
    constexpr size_t NUM_CONSUMERS = 4;
    constexpr size_t OPS_PER_PRODUCER = 100000;

    MPMCqueue<int, QUEUE_CAPACITY> queue;
    std::atomic<size_t> total_pushed{0};
    std::atomic<size_t> total_popped{0};
    std::atomic<bool> start_flag{false};
    std::vector<int> results;
    std::mutex results_mutex;

    // producers
    std::vector<std::thread> producers;
    for (size_t p = 0; p < NUM_PRODUCERS; ++p) {
        producers.emplace_back([&, p]() {
            while (!start_flag.load(std::memory_order_acquire)); // синхронный старт
            for (size_t i = 0; i < OPS_PER_PRODUCER; ++i) {
                int val = static_cast<int>(p * OPS_PER_PRODUCER + i);
                while (!queue.push(val)); // ждем, пока не вставится
                total_pushed.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }

    // consumers
    std::vector<std::thread> consumers;
    for (size_t c = 0; c < NUM_CONSUMERS; ++c) {
        consumers.emplace_back([&, c]() {
            while (!start_flag.load(std::memory_order_acquire));
            int value;
            while (total_popped.load(std::memory_order_relaxed) < NUM_PRODUCERS * OPS_PER_PRODUCER) {
                if (queue.pop(value)) {
                    total_popped.fetch_add(1, std::memory_order_relaxed);

                    // собираем результаты для проверки корректности
                    std::scoped_lock lock(results_mutex);
                    results.push_back(value);
                }
            }
        });
    }

    // Запускаем одновременно
    start_flag.store(true, std::memory_order_release);

    // ждём завершения
    for (auto& t : producers) t.join();
    for (auto& t : consumers) t.join();

    std::cout << "Total pushed: " << total_pushed.load() << std::endl;
    std::cout << "Total popped: " << total_popped.load() << std::endl;

    assert(total_pushed == total_popped);
    std::cout << "✅ Lock-free MPMC test passed!" << std::endl;

    // опционально — проверка уникальности элементов
    std::sort(results.begin(), results.end());
    for (size_t i = 1; i < results.size(); ++i) {
        if (results[i] == results[i - 1]) {
            std::cerr << "Duplicate value detected: " << results[i] << std::endl;
            assert(false);
        }
    }
}

int main() 
{
    test_mpmc_queue();
    return 0;
}