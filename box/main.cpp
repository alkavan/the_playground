#include "box.h"
#include <string>
#include <vector>

int main()
{
    using namespace util;

    /* 1. no_create – allocate handle but don't create resource */
    box<std::string> s1{tags::no_create};       // s1 is empty (nullptr)
    box<std::vector<int>> v1{tags::no_create};  // v1.get() returns NULL
    v1.reset(new std::vector{1,2,3});           // ... later ...

    /* 2. No Init – Allocate but leave uninitialized (fastest) */
    const box<int> raw{tags::no_init};  // *raw is uninitialized garbage until you write to it
    *raw = 42;                          // now safe

    /* 3. Value Init – Allocate + zero/value initialize */
    box<int> zero{tags::value_init};         // *zero == 0
    box<std::string> def{tags::value_init};  // calls default constructor

    /* 4. In-Place – Construct in-place from args */
    box<std::string> s2{tags::in_place, "hello", 5};           // in-place construction
    box<std::vector<int>> v2{tags::in_place, {1, 2, 3, 4}};  // no temporary vector

    /* 5. Ownership */
    box<std::string> b(tags::in_place, "cat");  // Owns a string.
    const std::string* owner = b.release();     // b is now empty; c owns the string.
    delete owner;                               // or wrap in another smart pointer.

    /* 6. Non-default-constructible types still only work with in_place */
    struct NonDefault { explicit NonDefault(int) {} };
    box<NonDefault> nd{std::in_place, 42};  // OK
}
