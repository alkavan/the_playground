# DCI Traits Example

A small experiment exploring **Data-Context-Interaction (DCI)** style role injection in modern C++ (C++23).

## Core Idea

Animals (`Bear`, `Fox`) are plain data objects. Behavior that depends on the current environment (season, time of day)
is provided by **roles** that are attached at runtime.

```cpp
const AnimalReactionRole bearReaction(bear, BearReaction());
bearReaction.react(context);   // role method
```

## Dispatch Characteristics

| Aspect                  | Nature     | Mechanism                                         |
|-------------------------|------------|---------------------------------------------------|
| Animal type             | Static     | Class template (`AnimalReactionRole<Animal>`)     |
| Reaction binding        | Runtime    | Captured into `std::move_only_function`           |
| Call to reaction        | Dynamic    | Indirect call through type-erased function object |
| Virtual functions       | None       | —                                                 |
| Inheritance             | None       | —                                                 |

### Type Erasure

`AnimalReactionRole` erases the concrete reaction type:

```cpp
template <typename Animal>
class AnimalReactionRole {
    // ...
    mutable std::move_only_function<void(const Animal&, const EnvironmentContext&)> react_func_;
};
```

- **Construction time (runtime)**: The concrete reaction (`BearReaction` / `FoxReaction`) is captured in a lambda and
  stored inside the type-erased function object. From this point the role no longer knows the original reaction type in
  its static type.
- **Call time (runtime)**: `react()` performs an indirect call through the type-erased callable.

This is **not** classic OOP dynamic dispatch (no vtables / inheritance).  
It is also **not** fully static dispatch (the reaction is not known at the call site).

It is a hybrid: the animal type is known statically while the role/behavior can be chosen and swapped at runtime via
type erasure.

## Files

- `domain.h`   – Plain data types (`Bear`, `Fox`) and enums
- `context.h`  – Environment context
- `roles.h`    – Role template and concepts
- `main.cpp`   – Usage example

## Build

Requires C++23 (`std::move_only_function`).

```bash
cmake -B build -S .
cmake --build build
```
