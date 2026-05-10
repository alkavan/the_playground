#include <meta>
#include <string_view>
#include <type_traits>
#include <iostream>

template <typename E>
    requires std::is_enum_v<E>
constexpr std::string_view enum_name(E value)
{
    template for (constexpr auto e : std::define_static_array(std::meta::enumerators_of(^^E))) {
        if (value == [:e:]) {
            return std::meta::identifier_of(e);
        }
    }
    return {};
}

// ==================== Example Usage ====================

enum class Color { Red, Green, Blue = 10 };

enum Fruit { Banana, Cherry = -5 };

int main()
{
    std::cout << "Color::Red       = " << enum_name(Color::Red)   << '\n';
    std::cout << "Color::Green     = " << enum_name(Color::Green)   << '\n';
    std::cout << "Color::Blue      = " << enum_name(Color::Blue)  << '\n';
    std::cout << "(Fruit::)Banana  = " << enum_name(Banana)  << '\n';
    std::cout << "(Fruit::)Cherry  = " << enum_name(Cherry)  << '\n';
    std::cout << "Color(99)        = " << enum_name(static_cast<Color>(99)) << "\n";
}