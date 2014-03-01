// ex: ts=4 sw=4 ft=cpp et indentexpr=
/**
 * \file
 * \brief atomic value operations tests
 *
 * \author Dmitriy Kargapolov
 * \since 01 February 2013
 *
 * Copyright (C) 2013 Dmitriy Kargapolov <dmitriy.kargapolov@gmail.com>
 * Use, modification and distribution are subject to the Boost Software
 * License, Version 1.0 (See accompanying file LICENSE_1_0.txt or copy
 * at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <neutx/atomic_value.hpp>
#include <boost/test/unit_test.hpp>

using namespace neutx::atomic;

BOOST_AUTO_TEST_SUITE( test_atomic_value )

BOOST_AUTO_TEST_CASE( single_thread_ops )
{
    atomic_value<int> a = 10;
    BOOST_REQUIRE_EQUAL(a.get(), 10);
    BOOST_REQUIRE_EQUAL(++a, 11);
    BOOST_REQUIRE_EQUAL(a++, 11);
    BOOST_REQUIRE_EQUAL(a.get(), 12);
    a += 10;
    BOOST_REQUIRE_EQUAL(a.get(), 22);
    a -= 15;
    BOOST_REQUIRE_EQUAL(a.get(), 7);
    a.cas(6, 12);
    BOOST_REQUIRE_EQUAL(a.get(), 7);
    a.cas(7, 12);
    BOOST_REQUIRE_EQUAL(a.get(), 12);
    a.bclear(7);
    BOOST_REQUIRE_EQUAL(a.get(), 8);
    a.bset(5);
    BOOST_REQUIRE_EQUAL(a.get(), 13);
    BOOST_REQUIRE_EQUAL(a & 7, 5);
}

BOOST_AUTO_TEST_SUITE_END()
