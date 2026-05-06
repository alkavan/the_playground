import math;
import utils;

#include <iostream>

int main() {
    // Math module
    std::cout << "Math module:\n";
    std::cout << "  15 + 7   = " << add(15, 7) << "\n";
    std::cout << "  20 - 8   = " << subtract(20, 8) << "\n";
    std::cout << "  6 * 9    = " << multiply(6, 9) << "\n";
    std::cout << "  100 / 4  = " << divide(100.0, 4.0) << "\n\n";

    // Utils module
    std::cout << "Utils module:\n";
    std::cout << "  square(7)     = " << square(7) << "\n";
    std::cout << "  cube(4)       = " << cube(4) << "\n";
    std::cout << "  power(2, 10)  = " << power(2, 10) << "\n\n";

    return 0;
}
