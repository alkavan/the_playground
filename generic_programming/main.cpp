#include <algorithm>
#include <array>
#include <iostream>
#include <string>
#include <random>
#include <chrono>
#include <experimental/contract>

struct TransactionResult {
    bool        success;      // true  → transaction applied
    double      new_balance; // balance after the operation
    std::string tx_id;       // unique transaction identifier
};

inline auto make_rng()
{
    std::array<std::mt19937_64::result_type, 4> seed{};
    std::random_device rd;
    std::ranges::generate(seed, std::ref(rd));
    std::seed_seq seq(seed.begin(), seed.end());
    return std::mt19937_64(seq);
}
thread_local std::mt19937_64 rng = make_rng();

[[nodiscard]]
auto withdraw(const double balance, const double amount, const std::string& id)
[[expects (balance >= 0.0)]]
[[expects (amount > 0.0)]]
[[expects (!id.empty())]]
[[expects (amount <= balance)]]
[[ensures (result: result.new_balance <= balance)]]
[[ensures (result: result.new_balance >= 0.0)]]
-> TransactionResult
{
    const double new_bal = balance - amount;

    std::uniform_int_distribution<std::uint64_t> dist;
    const std::string tx = id + "-TX" + std::to_string(dist(rng));

    const TransactionResult result{true, new_bal, tx};

    return result;
}

int main()
{
    const std::string account = "ACC-12345";

    try {
        constexpr double balance = 0.;
        auto [ok, new_balance, txid] = withdraw(balance, 250.0, account);
        std::cout << "Transaction result:\n"
                  << "  success      : " << std::boolalpha << ok << '\n'
                  << "  new balance  : $" << new_balance << '\n'
                  << "  transaction id: " << txid << "\n\n";

        if (ok) {
            std::cout << "  -> Funds withdrawn, balance updated.\n";
        } else {
            std::cout << "  -> Withdrawal failed.\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Transaction failed: " << e.what() << '\n';
    }
    return 0;
}
