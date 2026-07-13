#include <array>
#include <vector>
#include <string_view>
#include <algorithm>
#include <ranges>
#include <print>

// -----------------------------------------------------------------------------
// Domain
// -----------------------------------------------------------------------------
struct Item {
    std::string_view name;
    int value = 0;     // gold
    int rarity = 1;    // 1=common ... 5=legendary
    int quantity = 1;  //
};

struct AdventureResult {
    std::array<Item, 16> inventory{};
    std::size_t count = 0;
    int total_gold = 0;
    int legendary_count = 0;
    int items_discovered = 0;
};

// -----------------------------------------------------------------------------
// The entire program runs at compile time
// -----------------------------------------------------------------------------
consteval AdventureResult run_adventure() {
    std::vector<Item> bag;
    std::vector<Item> loot_log;

    // Helper: add or increase quantity
    auto add_item = [&](const std::string_view name, const int value, const int rarity, const int qty = 1) {
        const auto it = std::ranges::find_if(bag, [&](const Item &i) {
            return i.name == name;
        });

        if (it != bag.end()) {
            it->quantity += qty;
        } else {
            bag.push_back({name, value, rarity, qty});
        }
        loot_log.push_back({name, value, rarity, qty});
    };

    // ---------- Starting Gear ------------------------------------------------
    add_item("Rusty Sword",     15, 1);
    add_item("Leather Armor",   25, 1);
    add_item("Health Potion",   10, 1, 3);

    // ---------- Event 1: Cave ------------------------------------------------
    add_item("Gold Coin",          1, 1, 47);
    add_item("Silver Ring",       40, 2);
    add_item("Mysterious Scroll",  5, 3);

    // ---------- Event 2: Boss Fight ------------------------------------------
    if (const auto it = std::ranges::find_if(bag,
        [](const Item& i) { return i.name == "Health Potion"; });
        it != bag.end()
        ) { /* Lose some potions */it->quantity = std::max(0, it->quantity - 2);
    }

    add_item("Dragon Scale",  200, 4);
    add_item("Ancient Blade", 350, 5);

    // ---------- Event 3: Crafting --------------------------------------------
    // Spend 10 gold coins + the scroll -> Enchanted Amulet
    const auto coin_it   = std::ranges::find_if(bag,
        [](const Item& i){ return i.name == "Gold Coin"; });
    auto scroll_it = std::ranges::find_if(bag,
        [](const Item& i){ return i.name == "Mysterious Scroll"; });

    if (coin_it != bag.end() && coin_it->quantity >= 10 && scroll_it != bag.end()) {
        coin_it->quantity -= 10;
        bag.erase(scroll_it);
        add_item("Enchanted Amulet", 180, 4);
    }

    AdventureResult result{};
    result.items_discovered = static_cast<int>(loot_log.size());

    // Remove zero-quantity items and compute totals
    std::vector<Item> final_items;
    for (const auto& item : bag) {
        if (item.quantity > 0) {
            final_items.push_back(item);
            result.total_gold += item.value * item.quantity;
            if (item.rarity >= 5)
                result.legendary_count += item.quantity;
        }
    }

    // Sort: highest rarity first, then highest value
    std::ranges::sort(final_items, [](const Item& a, const Item& b) {
        if (a.rarity != b.rarity) return a.rarity > b.rarity;
        return a.value > b.value;
    });

    // Freeze into a fixed-size array (the only thing that survives)
    result.count = std::min(final_items.size(), result.inventory.size());
    for (std::size_t i = 0; i < result.count; ++i)
        result.inventory[i] = final_items[i];

    return result;
}

// Everything below is pure compile-time
constexpr auto FINAL = run_adventure();

static_assert(FINAL.count >= 5);
static_assert(FINAL.legendary_count >= 1);
static_assert(FINAL.total_gold > 500);

int main() {
    /* Runtime = just pretty-print the already-baked data */
    std::println("╔══════════════════════════════════════════════════════╗");
    std::println("║     COMPILE-TIME ADVENTURE INVENTORY ENGINE          ║");
    std::println("╚══════════════════════════════════════════════════════╝\n");

    std::println("Final Inventory (rarity -> value):\n");

    for (std::size_t i = 0; i < FINAL.count; ++i) {
        const auto&[name, value, rarity, quantity] = FINAL.inventory[i];
        const char* stars =
            rarity == 5 ? "★★★★★" :
            rarity == 4 ? "★★★★☆" :
            rarity == 3 ? "★★★☆☆" :
            rarity == 2 ? "★★☆☆☆" : "★☆☆☆☆";

        std::println("  {:>2}. {:<20} {:>4} gold  {}  ×{}",
                     i + 1, name, value, stars, quantity);
    }

    std::println("\n────────────────────────────────────────────────────");
    std::println("Total gold value   : {:>6}", FINAL.total_gold);
    std::println("Legendary items    : {:>6}", FINAL.legendary_count);
    std::println("Items discovered   : {:>6}", FINAL.items_discovered);
    std::println("Final unique items : {:>6}", FINAL.count);
    std::println("────────────────────────────────────────────────────");
    std::println("\nAll computation happened at compile time.");
    std::println("The binary contains only this frozen table.");
}
