#ifndef ROLES_H
#define ROLES_H

#include <functional>
#include <concepts>
#include "context.h"

template <typename Animal, typename Reaction>
concept EnvironmentReaction = requires(
    Reaction reaction,
    const Animal& animal,
    const EnvironmentContext& context)
{
    { reaction.react(animal, context) } -> std::same_as<void>;
};

template <typename Animal>
class AnimalReactionRole {
public:
    template <typename Reaction> requires EnvironmentReaction<Animal, Reaction>
    AnimalReactionRole(Animal animal, Reaction reaction)
        : animal_(std::move(animal))
        , react_func_([r = std::move(reaction)](const Animal& a, const EnvironmentContext& ctx) {
            r.react(a, ctx);
        }) {}

    void react(const EnvironmentContext& context) const {
        react_func_(animal_, context);
    }

private:
    Animal animal_;
    mutable std::move_only_function<void(const Animal&, const EnvironmentContext&)> react_func_;
};

#endif // ROLES_H
