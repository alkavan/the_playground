#include <stdexec/execution.hpp>
#include <exec/static_thread_pool.hpp>

#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include <numeric>
#include <chrono>
#include <optional>

int main() {
    using namespace std::chrono_literals;
    namespace ex = stdexec;

    std::cout << "Starting stdexec pipeline...\n";

    exec::static_thread_pool pool{4};
    auto sched = pool.get_scheduler();

    // Use schedule() + let_value() pattern — more reliable with static_thread_pool
    auto fetch_user = ex::schedule(sched)
        | ex::let_value([] {
            return ex::just(42)
                 | ex::then([](int user_id) {
                       std::this_thread::sleep_for(120ms);
                       std::cout << "  [Thread " << std::this_thread::get_id()
                                 << "] Fetched user for ID " << user_id << "\n";
                       return std::string{"Alice"};
                   });
        });

    auto fetch_orders = ex::schedule(sched)
        | ex::let_value([] {
            return ex::just(150)
                 | ex::then([](int count) {
                       std::this_thread::sleep_for(180ms);
                       std::cout << "  [Thread " << std::this_thread::get_id()
                                 << "] Fetched " << count << " orders\n";
                       return count * 42;
                   });
        });

    auto check_inventory = ex::schedule(sched)
        | ex::let_value([] {
            return ex::just(std::vector<int>{25, 40, 15, 60})
                 | ex::then([](const auto& levels) {
                       std::this_thread::sleep_for(90ms);
                       int total = std::accumulate(levels.begin(), levels.end(), 0);
                       std::cout << "  [Thread " << std::this_thread::get_id()
                                 << "] Inventory total: " << total << "\n";
                       return total;
                   });
        });

    auto pipeline = ex::when_all(
            std::move(fetch_user),
            std::move(fetch_orders),
            std::move(check_inventory)
        )
        | ex::then([](std::tuple<std::string, int, int> res) {
            auto [user, value, stock] = res;
            std::cout << "\n=== Report ===\n"
                      << "User: " << user << "\n"
                      << "Order value: $" << value << "\n"
                      << "Stock: " << stock << "\n";
            return std::string{"Done"};
        });

    auto result = ex::sync_wait(std::move(pipeline));

    if (result) {
        std::cout << "Finished: " << std::get<0>(*result) << "\n";
    }
}