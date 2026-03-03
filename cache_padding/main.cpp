#include <atomic>
#include <thread>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <cstdint>
#include <new>

constexpr uint64_t ITERATIONS = 100'000'000ULL;

struct Counters {
    std::atomic<uint64_t> counter1{0};
    std::atomic<uint64_t> counter2{0};
};

struct alignas(std::hardware_destructive_interference_size) PaddedCounters {
    std::atomic<uint64_t> counter1{0};
    char _pad1[std::hardware_destructive_interference_size - sizeof(std::atomic<uint64_t>)]{};
    std::atomic<uint64_t> counter2{0};
    char _pad2[std::hardware_destructive_interference_size - sizeof(std::atomic<uint64_t>)]{};
};

template<typename T>
void run_benchmark(const char* name) {
    T counters;

    const auto start = std::chrono::high_resolution_clock::now();

    std::thread t1([&] {
        for (uint64_t i = 0; i < ITERATIONS; ++i) {
            counters.counter1.fetch_add(1, std::memory_order_relaxed);
        }
    });

    std::thread t2([&] {
        for (uint64_t i = 0; i < ITERATIONS; ++i) {
            counters.counter2.fetch_add(1, std::memory_order_relaxed);
        }
    });

    t1.join();
    t2.join();

    const auto end = std::chrono::high_resolution_clock::now();
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << std::left << std::setw(40) << name
              << ": " << std::right << std::setw(6) << ms << " ms   "
              << "(c1=" << counters.counter1.load() << ", c2=" << counters.counter2.load() << ")\n";
}

int main() {
    std::cout << "=== Cache Padding C++ Example ===\n\n";

    std::cout << "hardware_destructive_interference_size = "
              << std::hardware_destructive_interference_size << " bytes\n\n";
    std::cout << "sizeof(Counters)       = " << sizeof(Counters) << " bytes\n";
    std::cout << "sizeof(PaddedCounters) = " << sizeof(PaddedCounters) << " bytes\n\n";

    run_benchmark<Counters>("Without padding (false sharing)");
    run_benchmark<PaddedCounters>("With portable padding");

    std::cout << "\n(Compile with: g++ -O3 -std=c++20 -pthread -march=native)\n";
    return 0;
}
