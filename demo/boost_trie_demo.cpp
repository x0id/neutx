// ex: ts=4 sw=4 ft=cpp et indentexpr=
/**
 * \file
 * \brief Boost.Trie demo
 *
 * \author Dmitriy Kargapolov
 * \since 23 February 2014
 *
 */

/*
 * Copyright (C) 2014 Dmitriy Kargapolov <dmitriy.kargapolov@gmail.com>
 * Use, modification and distribution are subject to the Boost Software
 * License, Version 1.0 (See accompanying file LICENSE_1_0.txt or copy
 * at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <boost/trie/trie_map.hpp>

int main() {
	boost::tries::trie_map<char, std::string> trie;

    // store some data
    std::string s;
    s = "123"; trie[s] = "three";
    s = "1234"; trie[s] = "four";
    s = "12345"; trie[s] = "five";

    return 0;
}
