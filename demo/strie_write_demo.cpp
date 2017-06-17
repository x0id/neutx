// ex: ts=4 sw=4 ft=cpp et indentexpr=
/**
 * \file
 * \brief strie write-to-file demo
 *
 * \author Dmitriy Kargapolov
 * \since 14 November 2013
 *
 */

/*
 * Copyright (C) 2013 Dmitriy Kargapolov <dmitriy.kargapolov@gmail.com>
 * Use, modification and distribution are subject to the Boost Software
 * License, Version 1.0 (See accompanying file LICENSE_1_0.txt or copy
 * at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <neutx/container/detail/simple_node_store.hpp>
#include <neutx/container/detail/file_store.hpp>
#include <neutx/container/detail/svector.hpp>
#include <neutx/container/detail/sarray.hpp>
#include <neutx/container/detail/pnode.hpp>
#include <neutx/container/detail/default_ptrie_codec.hpp>
#include <neutx/container/ptrie.hpp>

#include "string_codec.hpp"

namespace ct = neutx::container;
namespace dt = neutx::container::detail;

// payload type
typedef std::string data_t;

// trie node type
typedef dt::pnode<dt::simple_node_store<>, data_t, dt::svector<> > node_t;

// trie type
typedef ct::ptrie<node_t> trie_t;

template<typename AddrType>
struct EncoderTraits {
    typedef AddrType addr_type;
    typedef dt::file_store<addr_type> store_type;
    typedef typename string_codec::bind<addr_type>::encoder data_encoder;
    typedef typename dt::sarray<addr_type>::encoder coll_encoder;
    typedef typename dt::mmap_trie_codec::bind<addr_type>::encoder trie_encoder;
};

// offset type in external data representation
typedef EncoderTraits<uint32_t> encoder_t;
typedef encoder_t::store_type output_t;

struct updater {
    void operator()(data_t& a, const data_t& b) {
        std::cout << "update '" << a << "' with '" << b << "'" << std::endl;
        a += b;
    }
};

int main() {
    trie_t trie;

    // store some data
    trie.store("123", "three");
    trie.store("1234", "four");
    trie.store("12345", "five");

    // update path
    updater u;
    trie.update_path("12389", "^", u);
    trie.update_path("1", "%", u);
    trie.update_path("", "?", u);

    // write (export) trie to the file in external format
    output_t file("trie.bin");
    encoder_t encoder;
    trie.store_trie(encoder, file);

    return 0;
}
