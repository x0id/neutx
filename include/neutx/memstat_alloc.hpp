// ex: ts=4 sw=4 ft=cpp et indentexpr=
/**
 * \file
 * \brief allocator with memory usage counter
 *
 * \author Dmitriy Kargapolov
 * \version 1.0
 * \since 01 April 2013
 *
 */

/*
 * Copyright (C) 2013 Dmitriy Kargapolov <dmitriy.kargapolov@gmail.com>
 * Use, modification and distribution are subject to the Boost Software
 * License, Version 1.0 (See accompanying file LICENSE_1_0.txt or copy
 * at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef _NEUTX_MEMSTAT_ALLOC_HPP_
#define _NEUTX_MEMSTAT_ALLOC_HPP_

#include <memory>
#include <cstdlib>

namespace neutx {

struct std_alloc {
    static void *allocate(std::size_t size) {
        return std::malloc(size);
    }

    static void deallocate(void *p) {
        std::free(p);
    }
};

template<typename T, typename C, typename A = std_alloc>
struct memstat_alloc : std::allocator<T> {
    using typename std::allocator<T>::pointer;
    using typename std::allocator<T>::size_type;

    template<typename U>
    struct rebind {
        typedef memstat_alloc<U, C, A> other;
    };

    memstat_alloc() noexcept
        : std::allocator<T>()
    {}

    template<typename U>
    memstat_alloc(memstat_alloc<U, C, A> const& u) noexcept
        : std::allocator<T>(u)
    {}

    pointer allocate(size_type size, const void *hint = 0) {
        size *= sizeof(T);
        void *p = A::allocate(size);
        if (p == 0) throw std::bad_alloc();
        C::inc(size);
        return static_cast<pointer>(p);
    }

    void deallocate(pointer p, size_type size) {
        C::dec(size * sizeof(T));
        A::deallocate(p);
    }
};

} // namespace neutx

#endif // _NEUTX_MEMSTAT_ALLOC_HPP_
