// ex: ts=4 sw=4 ft=cpp et indentexpr=
/**
 * \file
 * \brief binary search demo
 *
 * \author Dmitriy Kargapolov
 * \since 11 June 2014
 *
 */

/*
 * Copyright (C) 2014 Dmitriy Kargapolov <dmitriy.kargapolov@gmail.com>
 * Use, modification and distribution are subject to the Boost Software
 * License, Version 1.0 (See accompanying file LICENSE_1_0.txt or copy
 * at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <neutx/algorithm/bsearch.hpp>
#include <stdlib.h>
#include <iostream>

struct ab {
    int a;
    int b;
};

void report(int n, ab *ret);

bool operator<(int a, const ab& b) {
    std::cout << "test " << b.a << std::endl;
    return a < b.a;
}

int main() {

    ab data[100];
    int v = 0;
    for (int i=0; i<100; ++i) {
        v += random() % 200;
        data[i].a = v;
        data[i].b = random() % 1000;
        std::cout << " (" << data[i].a << "," << data[i].b << ")";
    }
    std::cout << std::endl;
    report(500, neutx::algorithm::bs<int, ab, 100>::bsearch(data, 500));
    report(500, neutx::algorithm::bsearch(data, 100, 500));

    return 0;
}

void report(int n, ab *ret) {
    std::cout << "ret " << n << " : ";
    if (ret)
        std::cout << ret->b;
    else
        std::cout << "null";
    std::cout << std::endl;
}
