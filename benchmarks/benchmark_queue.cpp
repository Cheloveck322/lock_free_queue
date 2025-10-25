#include <benchmark/benchmark.h>
#include <thread>
#include <atomic>
#include "../include/RingBuffer.hpp"
#include "../include/MPMCqueue.hpp"

// ------------------------------------
//   BENCHMARK: RingBuffer
// ------------------------------------
static void BM_RingBuffer_PushPop(benchmark::State& state) {
    RingBuffer<int, 1024> q;
    constexpr int N = 100000;

    for (auto _ : state) {
        std::atomic<bool> start{false};
        std::thread producer([&]() {
            while (!start.load());
            for (int i = 0; i < N; ++i)
                while (!q.push(i));
        });
        std::thread consumer([&]() {
            while (!start.load());
            int x;
            for (int i = 0; i < N; ++i)
                while (!q.pop(x));
        });

        start.store(true);
        producer.join();
        consumer.join();
    }

    state.SetItemsProcessed(state.iterations() * N * 2);
}
BENCHMARK(BM_RingBuffer_PushPop)->Iterations(15);

// ------------------------------------
//   BENCHMARK: MPMCqueue
// ------------------------------------
static void BM_MPMC_PushPop(benchmark::State& state) {
    constexpr size_t CAPACITY = 1024;
    constexpr size_t NUM_PRODUCERS = 4;
    constexpr size_t NUM_CONSUMERS = 4;
    constexpr size_t OPS_PER_PRODUCER = 50000;

    MPMCqueue<int, CAPACITY> q;

    for (auto _ : state) {
        std::atomic<bool> start{false};
        std::vector<std::thread> producers, consumers;

        for (size_t p = 0; p < NUM_PRODUCERS; ++p) {
            producers.emplace_back([&, p]() {
                while (!start.load());
                for (size_t i = 0; i < OPS_PER_PRODUCER; ++i)
                    while (!q.push(static_cast<int>(p * OPS_PER_PRODUCER + i)));
            });
        }

        for (size_t c = 0; c < NUM_CONSUMERS; ++c) {
            consumers.emplace_back([&, c]() {
                while (!start.load());
                int val;
                for (size_t i = 0; i < OPS_PER_PRODUCER; ++i)
                    while (!q.pop(val));
            });
        }

        start.store(true);
        for (auto& t : producers) t.join();
        for (auto& t : consumers) t.join();
    }

    size_t total_ops = state.iterations() * NUM_PRODUCERS * OPS_PER_PRODUCER * 2;
    state.SetItemsProcessed(total_ops);
}
BENCHMARK(BM_MPMC_PushPop)->Iterations(15);

// ------------------------------------
BENCHMARK_MAIN();
