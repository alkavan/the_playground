#ifndef ROLES_H
#define ROLES_H

#include <memory>
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
    template <typename T> requires EnvironmentReaction<Animal, T>
    AnimalReactionRole(Animal animal, T impl)
        : animal_(std::move(animal)), impl_(std::make_shared<Model<T>>(std::move(impl))) {}

    void react(const EnvironmentContext& context) const {
        impl_->react(animal_, context);
    }

private:
    struct Concept {
        virtual ~Concept() = default;
        virtual void react(const Animal& animal, const EnvironmentContext& context) = 0;
    };

    template <typename T>
    struct Model final : Concept {
        explicit Model(T impl) : impl_(std::move(impl)) {}
        void react(const Animal& animal, const EnvironmentContext& context) override {
            impl_.react(animal, context);
        }
        T impl_;
    };

    Animal animal_;
    std::shared_ptr<Concept> impl_;
};

#endif //ROLES_H
