// ex: ts=4 sw=4 ft=cpp et indentexpr=
/**
 * \file
 * \brief digit trie write-to-file demo
 *
 * \author Dmitriy Kargapolov
 * \since 20 February 2014
 *
 */

/*
 * Copyright (C) 2014 Dmitriy Kargapolov <dmitriy.kargapolov@gmail.com>
 * Use, modification and distribution are subject to the Boost Software
 * License, Version 1.0 (See accompanying file LICENSE_1_0.txt or copy
 * at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <neutx/container/digit_trie.hpp>
#include "string_codec.hpp"

// digit trie types
typedef neutx::container::digit_trie<std::string> types;

// trie encoder types
typedef types::encoder_type<string_codec> encoder;

int main() {
    types::trie_type trie;

    // store some data
    trie.store("123", "three");
    trie.store("1234", "four");
    trie.store("12345", "five");

    // write (export) trie to the file in external format
    encoder::file_store out("trie.bin");
    encoder enc;
    trie.store_trie(enc, out);

    return 0;
}
