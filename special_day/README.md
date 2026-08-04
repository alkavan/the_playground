# SpecialDay

A lightweight, header-only C++ class that provides a type-safe way to represent holidays and special days.

Inspired by the design of `std::chrono` calendar types (`weekday`, `month`, `day`, …), `SpecialDay` stores a compact integer identifier and offers comparison, iteration within a category, and convenient name/category lookup.

## Features

- Strongly typed holiday identifiers (no raw integers in client code)
- Five categories: **Cultural**, **National**, **Christian**, **Jewish**, **Muslim**
- Holidays inside each category are ordered chronologically
- `next()` / `prev()` – move to the next/previous holiday **in the same category** (with wrap-around)
- `getDayName()` / `to_string()` – human-readable name
- `getDayCategory()` / `getCategoryName()` – category information
- Fully `constexpr` friendly
- Header-only, dependency-free (only `<string>` and `<compare>`)

## Categories & Holidays

### Cultural / Popular Events (0–7)
| Constant          | ID | Typical date                  |
|-------------------|----|-------------------------------|
| `PublicHoliday`   | 0  | (variable)                    |
| `ValentinesDay`   | 1  | February 14                   |
| `Superball`       | 2  | early February (1st Sunday)   |
| `Halloween`       | 3  | October 31                    |
| `MothersDay`      | 4  | 2nd Sunday in May             |
| `FathersDay`      | 5  | 3rd Sunday in June            |
| `BlackFriday`     | 6  | day after Thanksgiving        |
| `CyberMonday`     | 7  | Monday after Thanksgiving     |

### National – US (16–31)
Ordered by appearance in the calendar year.

| Constant                        | ID | Date                                      |
|---------------------------------|----|-------------------------------------------|
| `NewYearsDay`                   | 16 | January 1                                 |
| `InaugurationDay`               | 17 | January 20 (every 4 years)                |
| `MLKDay`                        | 18 | 3rd Monday in January                     |
| `PresidentsDay`                 | 19 | 3rd Monday in February                    |
| `MemorialDay`                   | 20 | last Monday in May                        |
| `FlagDay`                       | 21 | June 14                                   |
| `Juneteenth`                    | 22 | June 19                                   |
| `IndependenceDay`               | 23 | July 4                                    |
| `LaborDay`                      | 24 | 1st Monday in September                   |
| `PatriotDay`                    | 25 | September 11                              |
| `ColumbusDay`                   | 26 | 2nd Monday in October                     |
| `ElectionDay`                   | 27 | 1st Tuesday after 1st Monday in November  |
| `VeteransDay`                   | 28 | November 11                               |
| `ThanksgivingDay`               | 29 | 4th Thursday in November                  |
| `PearlHarborRemembranceDay`     | 30 | December 7                                |
| `Christmas`                     | 31 | December 25                               |

### Christian (48–57)
Ordered by Gregorian calendar appearance.

| Constant                | ID | Date / Rule                                      |
|-------------------------|----|--------------------------------------------------|
| `Epiphany`              | 48 | January 6                                        |
| `AshWednesday`          | 49 | 46 days before Easter                            |
| `PalmSunday`            | 50 | Sunday before Easter                             |
| `GoodFriday`            | 51 | Friday before Easter                             |
| `Easter`                | 52 | Sunday after first full moon ≥ March 21          |
| `EasterMonday`          | 53 | day after Easter                                 |
| `Ascension`             | 54 | 40 days after Easter (Thursday)                  |
| `Pentecost`             | 55 | 50 days after Easter                             |
| `AllSaintsDay`          | 56 | November 1                                       |
| `FirstSundayOfAdvent`   | 57 | 4th Sunday before Christmas                      |

### Jewish (80–88)
Ordered by appearance in the civil year.

| Constant          | ID | Hebrew date       |
|-------------------|----|-------------------|
| `Purim`           | 80 | 14 Adar           |
| `Passover`        | 81 | 15 Nisan          |
| `Shavuot`         | 82 | 6 Sivan           |
| `TishaBAv`        | 83 | 9 Av              |
| `RoshHashanah`    | 84 | 1–2 Tishrei       |
| `YomKippur`       | 85 | 10 Tishrei        |
| `Sukkot`          | 86 | 15 Tishrei        |
| `SimchatTorah`    | 87 | 22/23 Tishrei     |
| `Hanukkah`        | 88 | 25 Kislev         |

### Muslim (144–151)
Ordered by relative position inside the Islamic year.

| Constant            | ID  | Islamic date              |
|---------------------|-----|---------------------------|
| `IslamicNewYear`    | 144 | 1 Muharram                |
| `Ashura`            | 145 | 10 Muharram               |
| `MawlidAlNabi`      | 146 | 12 Rabi' al-Awwal         |
| `RamadanStart`      | 147 | 1 Ramadan                 |
| `LaylatAlQadr`      | 148 | 27 Ramadan (traditionally)|
| `EidAlFitr`         | 149 | 1 Shawwal                 |
| `DayOfArafah`       | 150 | 9 Dhu al-Hijjah           |
| `EidAlAdha`         | 151 | 10 Dhu al-Hijjah          |

## Quick Start

```cpp
#include "specialday.h"
#include <iostream>

int main()
{
    SpecialDay d = Easter;

    // Name & category
    std::cout << getDayName(d) << '\n';                 // "Easter"
    std::cout << getCategoryName(getDayCategory(d));    // "Christian"

    // Navigation inside the same category
    std::cout << getDayName(d.next()) << '\n';          // "Easter Monday"
    std::cout << getDayName(d.prev()) << '\n';          // "Good Friday"

    // Wrap-around
    d = Hanukkah;
    std::cout << getDayName(d.next()) << '\n';          // "Purim"

    // Comparison
    if (Christmas > ThanksgivingDay)
        std::cout << "Christmas comes after Thanksgiving\n";

    // Conversion & validation
    unsigned id = static_cast<unsigned>(d);
    bool valid  = d.ok();
}
```

## API Overview

### Construction
```cpp
SpecialDay d;                    // default (0)
SpecialDay e{52};                // from integer
SpecialDay f = Easter;           // from named constant
```

### Navigation
```cpp
d.next();     // next holiday in the same category (wraps)
d.prev();     // previous holiday in the same category (wraps)
++d; --d;     // raw ID increment / decrement
```

### Information
```cpp
getDayName(d);               // const char*  – "Easter"
to_string(d);                // std::string  – "Easter"
getDayCategory(d);           // HolidayCategory
getCategoryName(category);   // const char*  – "Christian"
d.ok();                      // true if ID ≤ 207
static_cast<unsigned>(d);    // underlying ID
```

### Comparison
```cpp
d1 == d2
d1 <=> d2          // enables <, >, <=, >=
```

## Design Notes

- All holidays belonging to the same category occupy a **dense consecutive range** of IDs. This makes `next()` / `prev()` extremely cheap (a few comparisons + arithmetic).
- The ordering inside each category follows the order the holidays appear in a typical civil year (or the relative order inside the Islamic year for the Muslim category).
- The class deliberately does **not** calculate actual calendar dates. It only identifies *which* holiday you are talking about. Date calculation belongs in a separate layer that combines a `SpecialDay` with a year.
- Unknown IDs are treated as `Cultural` by `getDayCategory()` and return `"Unknown"` from `getDayName()`. `next()` / `prev()` on an unknown ID simply return the same value.

## Requirements

- C++20 (for `std::strong_ordering` and `operator<=>`)
- No external dependencies
