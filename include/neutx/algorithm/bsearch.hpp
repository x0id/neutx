// ex: ts=4 sw=4 ft=cpp et indentexpr=
/**
 * \file
 * \brief binary search
 *
 * \author Dmitriy Kargapolov
 * \since 11 June 2014
 *
 */

/*
 * Copyright (C) 2012 Dmitriy Kargapolov <dmitriy.kargapolov@gmail.com>
 * Use, modification and distribution are subject to the Boost Software
 * License, Version 1.0 (See accompanying file LICENSE_1_0.txt or copy
 * at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef _NEUTX_ALGORITHM_BSEARCH_HPP_
#define _NEUTX_ALGORITHM_BSEARCH_HPP_

namespace neutx {
namespace algorithm {

template<typename T, typename S, unsigned N> struct bs {
    static S *bsearch(S *p, const T& t) {
        if (t < p[N/2]) {
            S *r = bs<T, S, N/2>::bsearch(p, t);
            return r ? r : &p[N/2];
        }
        return bs<T, S, N-N/2-1>::bsearch(p+N/2+1, t);
    }
};

template<typename T, typename S> struct bs<T, S, 0> {
    static S *bsearch(S*, const T&) { return 0; }
};

template<typename T, typename S> struct bs<T, S, 1> {
    static S *bsearch(S *p, const T& t) { return (t < p[0]) ? p : 0; }
};

template<typename T, typename S>
inline S *bsearch(S *p, unsigned n, const T& t) {
    switch (n) {
    case 0: return bs<T, S, 0>::bsearch(p, t);
    case 1: return bs<T, S, 1>::bsearch(p, t);
    case 2: return bs<T, S, 2>::bsearch(p, t);
    case 3: return bs<T, S, 3>::bsearch(p, t);
    case 4: return bs<T, S, 4>::bsearch(p, t);
    case 5: return bs<T, S, 5>::bsearch(p, t);
    case 6: return bs<T, S, 6>::bsearch(p, t);
    case 7: return bs<T, S, 7>::bsearch(p, t);
    case 8: return bs<T, S, 8>::bsearch(p, t);
    case 9: return bs<T, S, 9>::bsearch(p, t);
    default:
        unsigned k = n / 2;
        if (t < p[k]) {
            S *r = bsearch<T, S>(p, k, t);
            return r ? r : &p[k];
        }
        return bsearch<T, S>(p + k + 1, n - k - 1, t);
    }
}

} // namespace algorithm
} // namespace neutx

#endif // _NEUTX_ALGORITHM_BSEARCH_HPP_
