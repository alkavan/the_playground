#ifndef SPECIALDAY_H
#define SPECIALDAY_H

#include <string>
#include <compare>

class SpecialDay
{
private:
    unsigned char _sd;

public:
    SpecialDay() = default;

    explicit constexpr
    SpecialDay(const unsigned _sd) noexcept
        : _sd(_sd) {
    }

    constexpr SpecialDay& operator++() noexcept { ++_sd; return *this; }
    constexpr SpecialDay  operator++(int) noexcept { const auto _ret = *this; ++(*this); return _ret; }

    constexpr SpecialDay& operator--() noexcept { --_sd; return *this; }
    constexpr SpecialDay  operator--(int) noexcept { const auto _ret = *this; --(*this); return _ret; }

    constexpr explicit operator unsigned() const noexcept { return _sd; }

    [[nodiscard]] constexpr bool
    ok() const noexcept { return _sd <= 207u; }

    [[nodiscard]] constexpr SpecialDay
    next() const noexcept {
        const auto v = unsigned{*this};

        if (v <= 7) return SpecialDay(v == 7 ? 0 : v + 1);       // Cultural   0-7
        if (v <= 31) return SpecialDay(v == 31 ? 16 : v + 1);    // National  16-31
        if (v <= 57) return SpecialDay(v == 57 ? 48 : v + 1);    // Christian 48-57
        if (v <= 88) return SpecialDay(v == 88 ? 80 : v + 1);    // Jewish    80-88
        if (v <= 151) return SpecialDay(v == 151 ? 144 : v + 1); // Muslim   144-151

        return *this; // when unknown ID return value of self
    }

    [[nodiscard]] constexpr SpecialDay
    prev() const noexcept {
        const auto v = unsigned{*this};

        if (v <= 7) return SpecialDay(v == 0 ? 7 : v - 1);       // Cultural   0-7
        if (v <= 31) return SpecialDay(v == 16 ? 31 : v - 1);    // National  16-31
        if (v <= 57) return SpecialDay(v == 48 ? 57 : v - 1);    // Christian 48-57
        if (v <= 88) return SpecialDay(v == 80 ? 88 : v - 1);    // Jewish    80-88
        if (v <= 151) return SpecialDay(v == 144 ? 151 : v - 1); // Muslim   144-151

        return *this; // when unknown ID return value of self
    }

    friend constexpr bool
    operator==(const SpecialDay& _x, const SpecialDay& _y) noexcept {
        return unsigned{_x} == unsigned{_y};
    }

    friend constexpr std::strong_ordering
    operator<=>(const SpecialDay& _x, const SpecialDay& _y) noexcept {
        return unsigned{_x} <=> unsigned{_y};
    }
};

enum class HolidayCategory
{
    Cultural,
    National,
    Christian,
    Jewish,
    Muslim
};

[[nodiscard]] constexpr HolidayCategory
getDayCategory(const SpecialDay d) noexcept {
    switch (static_cast<unsigned>(d)) {
    case 0:  // PublicHoliday
    case 1:  // Valentine's Day
    case 2:  // Super Bowl
    case 3:  // Halloween
    case 4:  // Mother's Day
    case 5:  // Father's Day
    case 6:  // Black Friday
    case 7:  // Cyber Monday
    default:
        return HolidayCategory::Cultural;

    case 16: // New Year's Day
    case 17: // Inauguration Day
    case 18: // MLK Day
    case 19: // Presidents' Day
    case 20: // Memorial Day
    case 21: // Flag Day
    case 22: // Juneteenth
    case 23: // Independence Day
    case 24: // Labor Day
    case 25: // Patriot Day
    case 26: // Columbus Day
    case 27: // Election Day
    case 28: // Veterans Day
    case 29: // Thanksgiving Day
    case 30: // Pearl Harbor Remembrance Day
    case 31: // Christmas Day
        return HolidayCategory::National;

    case 48: // Epiphany
    case 49: // Ash Wednesday
    case 50: // Palm Sunday
    case 51: // Good Friday
    case 52: // Easter
    case 53: // Easter Monday
    case 54: // Ascension Day
    case 55: // Pentecost
    case 56: // All Saints' Day
    case 57: // First Sunday of Advent
        return HolidayCategory::Christian;

    case 80: // Purim
    case 81: // Passover
    case 82: // Shavuot
    case 83: // Tisha B'Av
    case 84: // Rosh Hashanah
    case 85: // Yom Kippur
    case 86: // Sukkot
    case 87: // Simchat Torah
    case 88: // Hanukkah
        return HolidayCategory::Jewish;

    case 144: // Islamic New Year
    case 145: // Ashura
    case 146: // Mawlid al-Nabi
    case 147: // Start of Ramadan
    case 148: // Laylat al-Qadr
    case 149: // Eid al-Fitr
    case 150: // Day of Arafah
    case 151: // Eid al-Adha
        return HolidayCategory::Muslim;
    }
}

[[nodiscard]] constexpr const char*
getCategoryName(const HolidayCategory c)
{
    switch (c) {
    case HolidayCategory::Cultural:  return "Cultural";
    case HolidayCategory::National:  return "National";
    case HolidayCategory::Christian: return "Christian";
    case HolidayCategory::Jewish:    return "Jewish";
    case HolidayCategory::Muslim:    return "Muslim";
    }
    return "Unknown";
}

[[nodiscard]] constexpr const char*
getDayName(const SpecialDay d) noexcept
{
    switch (static_cast<unsigned>(d))
    {
    // Cultural / Popular Events
    case 0:  return "Public Holiday";
    case 1:  return "Valentine's Day";
    case 2:  return "Super Bowl";
    case 3:  return "Halloween";
    case 4:  return "Mother's Day";
    case 5:  return "Father's Day";
    case 6:  return "Black Friday";
    case 7:  return "Cyber Monday";

    // National (US)
    case 16: return "New Year's Day";
    case 17: return "Inauguration Day";
    case 18: return "Martin Luther King Jr. Day";
    case 19: return "Presidents' Day";
    case 20: return "Memorial Day";
    case 21: return "Flag Day";
    case 22: return "Juneteenth";
    case 23: return "Independence Day";
    case 24: return "Labor Day";
    case 25: return "Patriot Day";
    case 26: return "Columbus Day";
    case 27: return "Election Day";
    case 28: return "Veterans Day";
    case 29: return "Thanksgiving Day";
    case 30: return "Pearl Harbor Remembrance Day";
    case 31: return "Christmas Day";

    // Christian
    case 48: return "Epiphany";
    case 49: return "Ash Wednesday";
    case 50: return "Palm Sunday";
    case 51: return "Good Friday";
    case 52: return "Easter";
    case 53: return "Easter Monday";
    case 54: return "Ascension Day";
    case 55: return "Pentecost";
    case 56: return "All Saints' Day";
    case 57: return "First Sunday of Advent";

    // Jewish
    case 80: return "Purim";
    case 81: return "Passover";
    case 82: return "Shavuot";
    case 83: return "Tisha B'Av";
    case 84: return "Rosh Hashanah";
    case 85: return "Yom Kippur";
    case 86: return "Sukkot";
    case 87: return "Simchat Torah";
    case 88: return "Hanukkah";

    // Muslim
    case 144: return "Islamic New Year";
    case 145: return "Ashura";
    case 146: return "Mawlid al-Nabi";
    case 147: return "Start of Ramadan";
    case 148: return "Laylat al-Qadr";
    case 149: return "Eid al-Fitr";
    case 150: return "Day of Arafah";
    case 151: return "Eid al-Adha";

    default:
        return "Unknown";
    }
}

[[nodiscard]] inline std::string
to_string(const SpecialDay d)
{
    return getDayName(d);
}

// Popular Events
inline constexpr SpecialDay PublicHoliday{0};   // (variable)
inline constexpr SpecialDay ValentinesDay{1};   // February 14
inline constexpr SpecialDay Superball{2};       // early February
inline constexpr SpecialDay Halloween{3};       // October 31
inline constexpr SpecialDay MothersDay{4};      // 2nd Sunday in May
inline constexpr SpecialDay FathersDay{5};      // 3rd Sunday in June
inline constexpr SpecialDay BlackFriday{6};     // day after Thanksgiving
inline constexpr SpecialDay CyberMonday{7};     // Monday after Thanksgiving

// National (US Federal, Observances, Holidays)
inline constexpr SpecialDay NewYearsDay{16};                 // January 1
inline constexpr SpecialDay InaugurationDay{17};             // January 20 (every 4 years)
inline constexpr SpecialDay MLKDay{18};                      // 3rd Monday in January
inline constexpr SpecialDay PresidentsDay{19};               // 3rd Monday in February
inline constexpr SpecialDay MemorialDay{20};                 // last Monday in May
inline constexpr SpecialDay FlagDay{21};                     // June 14
inline constexpr SpecialDay Juneteenth{22};                  // June 19
inline constexpr SpecialDay IndependenceDay{23};             // July 4
inline constexpr SpecialDay LaborDay{24};                    // 1st Monday in September
inline constexpr SpecialDay PatriotDay{25};                  // September 11
inline constexpr SpecialDay ColumbusDay{26};                 // 2nd Monday in October
inline constexpr SpecialDay ElectionDay{27};                 // 1st Tuesday after 1st Monday in November
inline constexpr SpecialDay VeteransDay{28};                 // November 11
inline constexpr SpecialDay ThanksgivingDay{29};             // 4th Thursday in November
inline constexpr SpecialDay PearlHarborRemembranceDay{30};   // December 7
inline constexpr SpecialDay Christmas{31};                   // December 25

// Christian (ordered by Gregorian calendar)
inline constexpr SpecialDay Epiphany{48};              // January 6
inline constexpr SpecialDay AshWednesday{49};          // 46 days before Easter
inline constexpr SpecialDay PalmSunday{50};            // Sunday before Easter
inline constexpr SpecialDay GoodFriday{51};            // Friday before Easter
inline constexpr SpecialDay Easter{52};                // Sunday after first full moon ≥ March 21
inline constexpr SpecialDay EasterMonday{53};          // day after Easter
inline constexpr SpecialDay Ascension{54};             // 40 days after Easter (Thursday)
inline constexpr SpecialDay Pentecost{55};             // 50 days after Easter
inline constexpr SpecialDay AllSaintsDay{56};          // November 1
inline constexpr SpecialDay FirstSundayOfAdvent{57};   // 4th Sunday before Christmas

// Jewish (ordered by Gregorian calendar)
inline constexpr SpecialDay Purim{80};          // 14 Adar
inline constexpr SpecialDay Passover{81};       // 15 Nisan
inline constexpr SpecialDay Shavuot{82};        // 6 Sivan
inline constexpr SpecialDay TishaBAv{83};       // 9 Av
inline constexpr SpecialDay RoshHashanah{84};   // 1–2 Tishrei
inline constexpr SpecialDay YomKippur{85};      // 10 Tishrei
inline constexpr SpecialDay Sukkot{86};         // 15 Tishrei
inline constexpr SpecialDay SimchatTorah{87};   // 22/23 Tishrei
inline constexpr SpecialDay Hanukkah{88};       // 25 Kislev

// Muslim (relative order within the Islamic year)
inline constexpr SpecialDay IslamicNewYear{144};   // 1 Muharram
inline constexpr SpecialDay Ashura{145};           // 10 Muharram
inline constexpr SpecialDay MawlidAlNabi{146};     // 12 Rabi' al-Awwal
inline constexpr SpecialDay RamadanStart{147};     // 1 Ramadan
inline constexpr SpecialDay LaylatAlQadr{148};     // 27 Ramadan (traditionally)
inline constexpr SpecialDay EidAlFitr{149};        // 1 Shawwal
inline constexpr SpecialDay DayOfArafah{150};      // 9 Dhu al-Hijjah
inline constexpr SpecialDay EidAlAdha{151};        // 10 Dhu al-Hijjah

#endif // SPECIALDAY_H
