# The C++ Playground

A playground project for testing new features introduced in new C++ standards.

## Examples

### Box
Custom `box<T>` smart pointer demonstrating tagged constructors for different
initialization strategies (`no_create`, `no_init`, `value_init`, `in_place`).

```cpp
const box<int> raw{tags::no_init};  // *raw is uninitialized garbage until you write to it
*raw = 42;                          // now safe
```

```cpp
box<int> zero{tags::value_init};       // *zero == 0
box<std::string> s1{tags::no_create};  // s1 is empty (nullptr)
```

```cpp
box<std::string> s2{tags::in_place, "hello", 5};         // in-place construction
box<std::vector<int>> v2{tags::in_place, {1, 2, 3, 4}};  // no temporary vector
```

### Cache Padding
Demonstrates cache line padding with `std::hardware_destructive_interference_size`
to avoid false sharing between atomics.

```cpp
struct alignas(std::hardware_destructive_interference_size) PaddedCounters {
    std::atomic<uint64_t> counter1{0};
    char _pad1[std::hardware_destructive_interference_size - sizeof(std::atomic<uint64_t>)]{};
    std::atomic<uint64_t> counter2{0};
    // ...
};
```

### Covariant Dispatch
Covariant dispatch simulation using `oxide::Union` and pattern matching without inheritance.

```cpp
using ShapeVariant = oxide::Union<Circle, Rectangle>;
ShapeVariant >> oxide::match{
    [](const Circle& c) { std::cout << c.area(); },
    [](const Rectangle& r) { std::cout << r.perimeter(); }
};
```

### DCI Traits
Data, Context, Interaction (DCI) using traits/roles for context-dependent reactions (e.g., animals by season).

```cpp
const AnimalReactionRole bearReaction(bear, BearReaction());
bearReaction.react(context);
```

### Modules (C++ Modules, MSVC)
Consuming C++ modules: custom and stdlib imports.

```cpp
import hello;
import <iostream>;
```

### Match (Rust idioms)
Rust-like pattern matching on discriminated unions, Option, Result with `oxide`.

```cpp
using Message = oxide::Union<Quit, Move, Write, Read>;
msg >> ox::match{
    [](const Quit&) { std::cout << "Quit\n"; },
    [](const Move& m) { std::cout << "Move (" << m.x << ", " << m.y << ")\n"; },
    // ...
};
```

### Reference Wrapper
`std::reference_wrapper` enables shuffle etc. on list via vector wrapper.

```cpp
std::vector<std::reference_wrapper<int>> v(l.begin(), l.end());
std::ranges::shuffle(v, std::mt19937{std::random_device{}()});
```

## Resources

### Modules

* [Consume C++ Standard Library as modules (Microsoft)](https://learn.microsoft.com/en-us/cpp/cpp/modules-cpp?view=msvc-170#consume-c-standard-library-as-modules-experimental)
* [import CMake; the Experiment is Over! (kitware, Oct 18, 2023)](https://www.kitware.com/import-cmake-the-experiment-is-over/)
* [Tutorial: Import the C++ standard library using modules from the command line (Microsoft)](https://learn.microsoft.com/en-us/cpp/cpp/tutorial-import-stl-named-module?view=msvc-170)
* [What is the file extension naming convention for c++20 modules?](https://stackoverflow.com/questions/75733706/what-is-the-file-extension-naming-convention-for-c20-modules)
