#include <iostream>
#include "specialday.h"

int main()
{
    // -------------------------------------------------
    // 1. Basic usage – name, id, category
    // -------------------------------------------------
    SpecialDay d = Easter;

    std::cout << "Holiday : " << getDayName(d) << '\n';
    std::cout << "ID      : " << static_cast<unsigned>(d) << '\n';
    std::cout << "Category: " << getCategoryName(getDayCategory(d)) << "\n\n";

    // -------------------------------------------------
    // 2. next() / prev() inside the same category
    // -------------------------------------------------
    std::cout << "Around Easter:\n";
    std::cout << "  prev -> " << getDayName(d.prev()) << '\n';
    std::cout << "  this -> " << getDayName(d) << '\n';
    std::cout << "  next -> " << getDayName(d.next()) << "\n\n";

    // -------------------------------------------------
    // 3. Wrap-around at the end/beginning of a category
    // -------------------------------------------------
    d = Hanukkah;   // last Jewish holiday
    std::cout << "Wrap-around (Jewish):\n";
    std::cout << "  Hanukkah.next() -> " << getDayName(d.next()) << '\n';   // Purim

    d = Purim;      // first Jewish holiday
    std::cout << "  Purim.prev()    -> " << getDayName(d.prev()) << "\n\n"; // Hanukkah

    // -------------------------------------------------
    // 4. Comparison operators
    // -------------------------------------------------
    if constexpr (Christmas > ThanksgivingDay)
        std::cout << "Christmas comes after Thanksgiving (by ID order)\n";

    if constexpr (Easter == SpecialDay{52})
        std::cout << "Easter has ID 52\n\n";

    // -------------------------------------------------
    // 5. Increment / decrement (raw ID)
    // -------------------------------------------------
    d = GoodFriday;
    ++d;
    std::cout << "GoodFriday ++ -> " << getDayName(d) << "\n\n";   // Easter

    // -------------------------------------------------
    // 6. to_string() and ok()
    // -------------------------------------------------
    std::cout << "to_string(IndependenceDay) = " << to_string(IndependenceDay) << '\n';
    std::cout << "IndependenceDay.ok()       = " << std::boolalpha << IndependenceDay.ok() << '\n';

    return 0;
}
