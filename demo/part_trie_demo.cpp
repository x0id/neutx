// ex: ts=4 sw=4 ft=cpp et indentexpr=
/**
 * \file
 * \brief strie composition demo
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
#include <neutx/container/detail/pnode.hpp>
#include <neutx/container/ptrie.hpp>

#include <iostream>

namespace ct = neutx::container;
namespace dt = neutx::container::detail;

// payload type
typedef std::string data_t;

struct part {};

template<typename K>
class part_cursor {
    part p;
public:
    part_cursor(const char *key) {}
    // part_cursor(const K& key) {}
    bool has_data() const { return true; }
    const part& get_data() { return p; }
    void next() {}
};

template<typename Data = char, typename Alloc = std::allocator<char> >
class parts {
    Data fixme;
public:
    typedef part symbol_t;
    template<typename U>
    struct rebind { typedef parts<U, Alloc> other; };
    const Data* get(part a_symbol) const {
        return 0;
    }
    template<typename C> Data& ensure(part a_symbol, C create) {
        fixme = create();
        return fixme;
    }
    template<typename F> void foreach_value(F f) {
        f(fixme);
    }
    template<typename F> void foreach_keyval(F f) const {
        part p;
        f(p, fixme);
    }
};

struct ptrie_traits_parts {
    template<typename Key>
    struct cursor { typedef part_cursor<Key> type; };
    // element position in the key sequence type
    typedef uint32_t position_type;
};

// trie node type
typedef dt::pnode<dt::simple_node_store<>, data_t, parts<> > node_t;

// trie type
typedef ct::ptrie<node_t, ptrie_traits_parts> trie_t;

// concrete trie store type
typedef trie_t::store_t store_t;

// key element position type (default: uint32_t)
typedef trie_t::position_t pos_t;

// fold functor example
static bool fun(std::string& acc, const data_t& data, const trie_t::store_t&,
        pos_t, bool) {
    if (data.empty())
        return true;
    acc = data;
    std::cout << acc << std::endl;
    return true;
}

typedef std::vector<part> parts_t;

namespace std {
ostream& operator <<(ostream& out, const parts_t& parts) {
    return out;
}
}

// foreach functor example
static void enumerate(const parts_t& key, node_t& node, store_t&) {
    std::cout << "'" << key << "' -> '" << node.data() << "'" << std::endl;
}

int main() {
    trie_t trie;

    // store some data
    trie.store("123", "three");
    trie.store("1234", "four");
    trie.store("12345", "five");

    // fold through the key-matching nodes
    std::string ret;
    trie.fold("1234567", ret, fun);
    std::cout << "lookup result: " << (ret.empty() ? "not found" : ret)
        << std::endl;

    // traverse all the nodes
    trie.foreach<ct::up, parts_t>(enumerate);
    trie.foreach<ct::down, parts_t>(enumerate);

    return 0;
}
