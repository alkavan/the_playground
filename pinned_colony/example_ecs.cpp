// example_ecs.cpp
// A larger Bevy-style ECS simulation that stresses pinned_colony
// and demonstrates stable component pointers under heavy mutation.

#include "pinned_colony.hpp"

#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>

using clock_type = std::chrono::high_resolution_clock;

// =============================================================================
// Entity
// =============================================================================
using Entity = std::uint32_t;
constexpr Entity INVALID_ENTITY = static_cast<Entity>(-1);

namespace
{
    // =========================================================================
    // Components
    // =========================================================================
    struct Position {
        float x = 0.0f;
        float y = 0.0f;
        Entity owner = INVALID_ENTITY;
    };

    struct Velocity {
        float dx = 0.0f;
        float dy = 0.0f;
        Entity owner = INVALID_ENTITY;
    };

    struct Health {
        int current = 100;
        int max     = 100;
        Entity owner = INVALID_ENTITY;
    };

    // Holds a raw pointer into another colony. This is only safe because
    // pinned_colony guarantees that live elements never relocate.
    struct Target {
        Entity owner          = INVALID_ENTITY;
        Health* target_health = nullptr;   // stable
    };

    // =========================================================================
    // World
    // =========================================================================
    struct World {
        static constexpr std::size_t MAX_ENTITIES = 32 * 1024;   // 32k

        pinned_colony<Position> positions{MAX_ENTITIES};
        pinned_colony<Velocity> velocities{MAX_ENTITIES};
        pinned_colony<Health>   healths{MAX_ENTITIES};
        pinned_colony<Target>   targets{MAX_ENTITIES};

        Entity next_entity_id = 0;

        Entity spawn() {
            return next_entity_id++;
        }

        Entity spawn_moving_entity(const float x, const float y,
                                   const float dx, const float dy,
                                   const int hp = 100) {
            const Entity e = spawn();
            positions.emplace(Position{.x = x, .y = y, .owner = e});
            velocities.emplace(Velocity{.dx = dx, .dy = dy, .owner = e});
            healths.emplace(Health{.current = hp, .max = hp, .owner = e});
            return e;
        }
    };

    // =========================================================================
    // Helpers
    // =========================================================================
    struct BenchResult {
        double spawn_ms      = 0.0;
        double total_sim_ms  = 0.0;
        double movement_ms   = 0.0;
        double attract_ms    = 0.0;
        double vitality_ms   = 0.0;
        double repro_ms      = 0.0;
        double target_ms     = 0.0;
        std::size_t entities = 0;
        int frames           = 0;
    };
}

// =============================================================================
// Systems
// =============================================================================

static void movement_system(World& world, const float dt) {
    for (auto& [dx, dy, owner1] : world.velocities) {
        for (auto& [x, y, owner2] : world.positions) {
            if (owner2 == owner1) {
                x += dx * dt;
                y += dy * dt;
                break;
            }
        }
    }
}

static void attract_system(World& world, const float dt) {
    for (auto& [dx, dy, owner1] : world.velocities) {
        for (auto& [x, y, owner2] : world.positions) {
            if (owner2 == owner1) {
                constexpr float damping  = 0.992f;
                constexpr float strength = 0.18f;
                dx += -x * strength * dt;
                dy += -y * strength * dt;
                dx *= damping;
                dy *= damping;
                break;
            }
        }
    }
}

// Dramatic vitality: mild decay + random damage spikes + strong heal
static void vitality_system(World& world, const int frame, std::mt19937& rng) {
    std::uniform_int_distribution dmg_chance(0, 99);
    std::uniform_int_distribution dmg_amount(8, 25);

    for (auto& hp : world.healths) {
        if (hp.current <= 0) continue;

        // mild continuous decay
        if (frame % 3 == 0) {
            hp.current -= 1;
        }

        // random damage spike (~4% chance)
        if (dmg_chance(rng) < 4) {
            hp.current -= dmg_amount(rng);
        }

        // strong heal every 45 frames
        if (frame % 45 == 0) {
            hp.current = std::min(hp.current + 18, hp.max);
        }

        if (hp.current < 0) hp.current = 0;
    }
}

// Healthy entities can spawn children
static void reproduction_system(World& world, const int frame, std::mt19937& rng) {
    if (frame % 30 != 0) return;
    if (world.healths.size() >= World::MAX_ENTITIES - 64) return;

    std::uniform_real_distribution pos_dist(-40.0f, 40.0f);
    std::uniform_real_distribution vel_dist(-1.8f, 1.8f);
    std::uniform_int_distribution  chance(0, 99);

    // snapshot parents so we can insert while iterating
    std::vector<Entity> parents;
    for (const auto& hp : world.healths) {
        if (hp.current > 70) {
            parents.push_back(hp.owner);
        }
    }

    int born = 0;
    for ([[maybe_unused]] const Entity parent : parents) {
        if (chance(rng) < 6) {          // 6% chance per healthy parent
            world.spawn_moving_entity(
                pos_dist(rng), pos_dist(rng),
                vel_dist(rng), vel_dist(rng),
                80 + (chance(rng) % 40));
            ++born;
            if (born > 40) break;       // soft limit per wave
        }
    }
}

static void target_system(World& world) {
    for (auto& [owner, target_health] : world.targets) {
        if (target_health && target_health->current > 0) {
            const volatile int sink = target_health->current;
            (void)sink;
        }
    }
}

static double ms_since(const clock_type::time_point start) {
    using namespace std::chrono;
    return duration<double, std::milli>(clock_type::now() - start).count();
}

// =============================================================================
// Main
// =============================================================================
int main() {
    constexpr int   NUM_ENTITIES = 8'000;     // leave room to grow
    constexpr int   NUM_FRAMES   = 360;
    constexpr float DT           = 1.0f / 60.0f;

    World world;
    BenchResult bench;
    bench.frames   = NUM_FRAMES;
    bench.entities = static_cast<std::size_t>(NUM_ENTITIES);

    std::mt19937 rng{42};
    std::uniform_real_distribution pos_dist(-50.0f, 50.0f);
    std::uniform_real_distribution vel_dist(-2.0f, 2.0f);
    std::uniform_int_distribution  hp_dist(70, 110);

    // --- spawn ---------------------------------------------------------------
    const auto t0 = clock_type::now();

    std::vector<Entity> entities;
    entities.reserve(NUM_ENTITIES);

    for (int i = 0; i < NUM_ENTITIES; ++i) {
        const Entity e = world.spawn_moving_entity(
            pos_dist(rng), pos_dist(rng),
            vel_dist(rng), vel_dist(rng),
            hp_dist(rng));
        entities.push_back(e);
    }

    // Stable Health* targets
    constexpr int NUM_TARGETS = 400;
    for (int i = 0; i < NUM_TARGETS; ++i) {
        const Entity owner  = entities[static_cast<std::size_t>(i) % entities.size()];
        const Entity victim = entities[static_cast<std::size_t>(i * 7 + 13) % entities.size()];

        Health* victim_hp = nullptr;
        for (auto& hp : world.healths) {
            if (hp.owner == victim) {
                victim_hp = &hp;
                break;
            }
        }
        world.targets.emplace(Target{.owner = owner, .target_health = victim_hp});
    }

    bench.spawn_ms = ms_since(t0);

    // --- simulate ------------------------------------------------------------
    const auto sim_start = clock_type::now();

    for (int frame = 0; frame < NUM_FRAMES; ++frame) {
        auto s0 = clock_type::now();
        movement_system(world, DT);
        bench.movement_ms += ms_since(s0);

        s0 = clock_type::now();
        attract_system(world, DT);
        bench.attract_ms += ms_since(s0);

        s0 = clock_type::now();
        vitality_system(world, frame, rng);
        bench.vitality_ms += ms_since(s0);

        s0 = clock_type::now();
        reproduction_system(world, frame, rng);
        bench.repro_ms += ms_since(s0);

        s0 = clock_type::now();
        target_system(world);
        bench.target_ms += ms_since(s0);

        // progress every 15 frames
        if (frame % 15 == 0) {
            std::size_t alive = 0;
            int total_hp = 0;
            for (const auto& hp : world.healths) {
                if (hp.current > 0) {
                    ++alive;
                    total_hp += hp.current;
                }
            }
            std::cout << "frame " << std::setw(3) << frame
                      << "  live ~ " << std::setw(5) << alive
                      << "  avgHP ~ " << std::setw(3)
                      << (alive ? total_hp / static_cast<int>(alive) : 0)
                      << "  total slots ~ " << world.healths.size() << '\n';
        }
    }

    bench.total_sim_ms = ms_since(sim_start);

    // --- results -------------------------------------------------------------
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\n========== pinned_colony ECS benchmark ==========\n";
    std::cout << "Initial entities     : " << bench.entities << '\n';
    {
        std::size_t alive = 0;
        for (const auto& hp : world.healths)
            if (hp.current > 0) ++alive;
        std::cout << "Final live entities  : " << alive << '\n';
    }
    std::cout << "Target components    : " << NUM_TARGETS << " (stable Health*)\n";
    std::cout << "Frames simulated     : " << bench.frames << '\n';
    std::cout << "Spawn time           : " << bench.spawn_ms << " ms\n";
    std::cout << "Total simulation     : " << bench.total_sim_ms << " ms\n";
    std::cout << "  movement_system    : " << bench.movement_ms << " ms\n";
    std::cout << "  attract_system     : " << bench.attract_ms << " ms\n";
    std::cout << "  vitality_system    : " << bench.vitality_ms << " ms\n";
    std::cout << "  reproduction_system: " << bench.repro_ms << " ms\n";
    std::cout << "  target_system      : " << bench.target_ms << " ms\n";
    std::cout << "Avg frame time       : "
              << (bench.total_sim_ms / bench.frames) << " ms\n";

    // Final proof that the stored pointers are still valid
    std::size_t valid_targets = 0;
    for (const auto& [owner, target_health] : world.targets) {
        if (target_health) {
            const volatile int sink = target_health->current;
            (void)sink;
            ++valid_targets;
        }
    }
    std::cout << "Stable target pointers still valid: " << valid_targets
              << " / " << NUM_TARGETS << '\n';
    std::cout << "=================================================\n";

    return 0;
}