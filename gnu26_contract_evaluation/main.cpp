#include <contracts>
#include <cstddef>
#include <iostream>

// Contract violation handler
void handle_contract_violation(const std::contracts::contract_violation& violation)
{
    std::cerr << "\n=== CONTRACT VIOLATION ===\n";
    std::cerr << "Kind     : " << static_cast<int>(violation.kind()) << "\n";
    std::cerr << "Semantic : " << static_cast<int>(violation.semantic()) << "\n";
    std::cerr << "Location : "
              << violation.location().file_name() << ":"
              << violation.location().line() << "\n";
    std::cerr << "Function : " << violation.location().function_name() << "\n";
    std::cerr << "Comment: " << violation.comment() << "\n";

    std::terminate();
}

namespace {
    // Minimal fixed-size Array with C++26 contracts
    template <typename T, std::size_t N>
    class Array {
        T data_[N]{};

    public:
        // size() is constexpr and noexcept so the precondition can be evaluated cheaply
        [[nodiscard]] constexpr std::size_t size() const noexcept {
            return N;
        }

        // Non-const overload
        T& operator[](std::size_t i)
        pre (i < size())
        {
            return data_[i];
        }

        // Const overload
        const T& operator[](std::size_t i) const
        pre (i < size())          // ← same precondition
        {
            return data_[i];
        }
    };
}

int main() {
    Array<int, 5> a;

    // Valid access – precondition holds
    a[0] = 42;
    a[4] = 99;

    std::cout << a[0] << ' ' << a[4] << '\n';   // 42 99

    //a[5] = 0;   // i == 5, size() == 5 → precondition fails

    return 0;
}
