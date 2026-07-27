/*
*  Copyright (C) 2026 Igal Alkon and contributors
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included
 *  in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *  DEALINGS IN THE SOFTWARE.
 */
#ifndef VM_ALLOCATOR_HPP
#define VM_ALLOCATOR_HPP

#include <cstddef>
#include <new>
#include <stdexcept>
#include <type_traits>

#if defined(_WIN32)
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  include <windows.h>
#else
#  include <sys/mman.h>
#  include <unistd.h>
#  include <cerrno>
#endif

template <typename T>
class vm_allocator {
public:
    using value_type      = T;
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;
    using propagate_on_container_move_assignment = std::true_type;
    using is_always_equal = std::true_type;

    constexpr vm_allocator() noexcept = default;

    template <typename U>
    explicit constexpr vm_allocator(const vm_allocator<U>&) noexcept {}

    [[nodiscard]] static T* allocate(const size_type n) {
        if (n == 0) {
            return nullptr;
        }
        if (n > max_size()) {
            throw std::bad_array_new_length();
        }

        const size_type bytes = n * sizeof(T);
        void* p = map_pages(bytes);
        if (!p) {
            throw std::bad_alloc();
        }
        return static_cast<T*>(p);
    }

    static void deallocate(T* p, const size_type n) noexcept {
        if (!p || n == 0) {
            return;
        }
        unmap_pages(p, n * sizeof(T));
    }

    [[nodiscard]] static size_type max_size() noexcept {
        return static_cast<size_type>(-1) / sizeof(T);
    }

    template <typename U>
    struct rebind {
        using other = vm_allocator<U>;
    };

private:
    static void* map_pages(size_type bytes) {
#if defined(_WIN32)
        return VirtualAlloc(nullptr, bytes, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
#else
        void* p = ::mmap(nullptr, bytes,
                         PROT_READ | PROT_WRITE,
                         MAP_PRIVATE | MAP_ANONYMOUS,
                         -1, 0);
        return (p == MAP_FAILED) ? nullptr : p;
#endif
    }

    static void unmap_pages(void* p, size_type bytes) noexcept {
#if defined(_WIN32)
        (void)bytes;
        VirtualFree(p, 0, MEM_RELEASE);
#else
        ::munmap(p, bytes);
#endif
    }
};

template <typename T, typename U>
constexpr bool operator==(const vm_allocator<T>&, const vm_allocator<U>&) noexcept {
    return true;
}

template <typename T, typename U>
constexpr bool operator!=(const vm_allocator<T>&, const vm_allocator<U>&) noexcept {
    return false;
}

#endif // VM_ALLOCATOR_HPP