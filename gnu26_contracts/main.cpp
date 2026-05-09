#include <iostream>
#include <contracts>

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

int divide(const int a, const int b)
    pre(b != 0)
    post(r : r == a / b && r * b == a)
{
    return a / b;
}

void process_positive(const int value)
    pre (value > 0)
{
    std::cout << "Processing value: " << value << '\n';
}

int main() {
    int result = divide(20, 5);
    std::cout << "20 / 5 = " << result << '\n';

    process_positive(42);

    // Trigger a contract violation
    // (behavior depends on -fcontract-evaluation-semantic=... flag):
    divide(10, 0);

    return 0;
}
