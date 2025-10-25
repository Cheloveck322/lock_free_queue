#include <gtest/gtest.h>
#include <thread>
#include <atomic>
#include <vector>
#include <algorithm>
#include <chrono>
#include "../include/RingBuffer.hpp"
#include "../include/MPMCqueue.hpp"

// ------------------------------
//  RingBuffer tests
// ------------------------------

TEST(RingBuffer, SingleThreadPushPop) {
    RingBuffer<int, 8> q;
    EXPECT_TRUE(q.push(10));
    int val = 0;
    EXPECT_TRUE(q.pop(val));
    EXPECT_EQ(val, 10);
}

TEST(RingBuffer, MultiPushSinglePop) {
    RingBuffer<int, 16> q;
    std::vector<int> input{1, 2, 3, 4, 5};
    for (int v : input) EXPECT_TRUE(q.push(v));

    std::vector<int> output;
    int val;
    while (q.pop(val))
        output.push_back(val);

    EXPECT_EQ(input, output);
}

TEST(RingBuffer, ConcurrentProducerConsumer) {
    RingBuffer<int, 1024> q;
    constexpr int N = 100000;

    std::thread producer([&]() {
        for (int i = 0; i < N; ++i)
            while (!q.push(i));
    });

    std::vector<int> result;
    result.reserve(N);
    std::thread consumer([&]() {
        int x;
        for (int i = 0; i < N; ++i) {
            while (!q.pop(x));
            result.push_back(x);
        }
    });

    producer.join();
    consumer.join();

    ASSERT_EQ(result.size(), N);
    for (int i = 0; i < N; ++i)
        EXPECT_EQ(result[i], i);
}

// ------------------------------
//  MPMC TESTS
// ------------------------------

TEST(MPMCqueue, SingleThreadPushPop) {
    MPMCqueue<int, 8> q;
    EXPECT_TRUE(q.push(42));
    int val = 0;
    EXPECT_TRUE(q.pop(val));
    EXPECT_EQ(val, 42);
}

TEST(MPMCqueue, MultiProducerMultiConsumer) {
    constexpr size_t CAPACITY = 1024;
    constexpr size_t NUM_PRODUCERS = 4;
    constexpr size_t NUM_CONSUMERS = 4;
    constexpr size_t OPS_PER_PRODUCER = 25000;

    MPMCqueue<int, CAPACITY> q;
    std::atomic<size_t> total_pushed{0};
    std::atomic<size_t> total_popped{0};
    std::vector<int> results;
    std::mutex results_mutex;

    std::vector<std::thread> producers, consumers;

    for (size_t p = 0; p < NUM_PRODUCERS; ++p) {
        producers.emplace_back([&, p]() {
            for (size_t i = 0; i < OPS_PER_PRODUCER; ++i) {
                int val = static_cast<int>(p * OPS_PER_PRODUCER + i);
                while (!q.push(val));
                total_pushed.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }

    for (size_t c = 0; c < NUM_CONSUMERS; ++c) {
        consumers.emplace_back([&, c]() {
            int val;
            while (total_popped.load() < NUM_PRODUCERS * OPS_PER_PRODUCER) {
                if (q.pop(val)) {
                    std::scoped_lock lock(results_mutex);
                    results.push_back(val);
                    total_popped.fetch_add(1, std::memory_order_relaxed);
                }
            }
        });
    }

    for (auto& t : producers) t.join();
    for (auto& t : consumers) t.join();

    EXPECT_EQ(total_pushed.load(), NUM_PRODUCERS * OPS_PER_PRODUCER);
    EXPECT_EQ(total_popped.load(), NUM_PRODUCERS * OPS_PER_PRODUCER);

    std::sort(results.begin(), results.end());
    for (size_t i = 1; i < results.size(); ++i)
        EXPECT_NE(results[i], results[i - 1]) << "Duplicate value detected!";
}

TEST(MPMCqueue, StressTest) {
    MPMCqueue<int, 2048> q;
    constexpr int N = 200000;
    std::atomic<bool> start_flag{false};
    std::atomic<int> sum_produced{0};
    std::atomic<int> sum_consumed{0};

    std::thread producer([&]() {
        while (!start_flag.load());
        for (int i = 0; i < N; ++i) {
            while (!q.push(i));
            sum_produced += i;
        }
    });

    std::thread consumer([&]() {
        while (!start_flag.load());
        int val;
        for (int i = 0; i < N; ++i) {
            while (!q.pop(val));
            sum_consumed += val;
        }
    });

    start_flag.store(true);
    producer.join();
    consumer.join();

    EXPECT_EQ(sum_produced.load(), sum_consumed.load());
}
