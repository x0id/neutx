// ex: ts=4 sw=4 ft=cpp et indentexpr=
/**
 * \file
 * \brief test time converter
 *
 * \author Dmitriy Kargapolov
 * \since 08 April 2014
 *
 * Copyright (C) 2013 Dmitriy Kargapolov <dmitriy.kargapolov@gmail.com>
 * Use, modification and distribution are subject to the Boost Software
 * License, Version 1.0 (See accompanying file LICENSE_1_0.txt or copy
 * at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <config.h>
#include <neutx/time/timeconv.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include <boost/test/unit_test.hpp>

#if defined HAVE_BOOST_CHRONO
#include <boost/chrono/system_clocks.hpp>
#endif

namespace tztree_test {

namespace nt = neutx::time;

#if defined HAVE_BOOST_CHRONO
typedef boost::chrono::high_resolution_clock clock;
typedef clock::time_point time_point;
typedef clock::duration duration;
using boost::chrono::nanoseconds;
using boost::chrono::microseconds;
using boost::chrono::milliseconds;
using boost::chrono::duration_cast;
#endif

#define NSAMPLES 1000000

struct f {
    nt::tzdata *tz;

    f() {
        tz = new nt::tzdata(1950, 2100, "America/New_York");
    }
    ~f() {
        delete tz;
    }

    time_t to_utc(time_t t, int is_dst = -1) {
        nt::tzdata::shift_point ret;
        tz->offset(t, ret);
        // switching to winter time?
        if (ret.off1 > ret.off2)
            if (is_dst > 0)
                return t - ret.off1;
            if (is_dst == 0)
                return t - ret.off2;
        // all other cases
        return t - ret.off1;
    }

    time_t lc_to_utc(struct tm *tm) {
        return to_utc(nt::to_secs(
            tm->tm_year + 1900,
            tm->tm_mon + 1,
            tm->tm_mday,
            tm->tm_hour,
            tm->tm_min,
            tm->tm_sec
        ), tm->tm_isdst);
    }
};

BOOST_AUTO_TEST_SUITE( test_tztree )

BOOST_AUTO_TEST_CASE( load_tztree_test )
{
    BOOST_TEST_MESSAGE("loading tz data");
    nt::tzdata *tz = new nt::tzdata(1950, 2100, "America/New_York");
    BOOST_REQUIRE(tz != 0);
}

BOOST_FIXTURE_TEST_CASE( simple_test, f )
{
    BOOST_TEST_MESSAGE("simple test");
    nt::tzdata::shift_point ret;
    tz->offset(123, ret);
    to_utc(123);
    time_t t = 1397224741;
    struct tm *tm = localtime(&t);
    time_t t1 = lc_to_utc(tm);
    time_t t2 = timelocal(tm);
    BOOST_REQUIRE_EQUAL(t1, t2);
}

BOOST_FIXTURE_TEST_CASE( massive_test, f )
{
    BOOST_TEST_MESSAGE("random test using " << NSAMPLES << " samples");
    srand(1);
    time_t tmin = tz->lotime();
    time_t tmax = tz->hitime();
    long range = boost::numeric_cast<long>(tmax - tmin);
    double coeff = (double)range / RAND_MAX;
    for (int i=0; i<NSAMPLES; ++i) {
        time_t t = tmin + boost::numeric_cast<time_t>(rand() * coeff);
        BOOST_REQUIRE(t >= tmin && t <= tmax);
        struct tm *tm = localtime(&t);
        time_t t1 = lc_to_utc(tm);
        time_t t2 = timelocal(tm);
        BOOST_REQUIRE_EQUAL(t1, t2);
    }
}

#if defined HAVE_BOOST_CHRONO
BOOST_FIXTURE_TEST_CASE( time_test, f )
{
    BOOST_TEST_MESSAGE("timing test using " << NSAMPLES << " samples");

    struct tm *tms = new tm[NSAMPLES];
    srand(1);
    time_t tmin = tz->lotime();
    time_t tmax = tz->hitime();
    long range = boost::numeric_cast<long>(tmax - tmin);
    double coeff = (double)range / RAND_MAX;
    for (int i=0; i<NSAMPLES; ++i) {
        time_t t = tmin + boost::numeric_cast<time_t>(rand() * coeff);
        BOOST_REQUIRE(t >= tmin && t <= tmax);
        struct tm *tm = localtime(&t);
        tms[i] = *tm;
    }

    time_point tp0 = clock::now();
    for (int i=0; i<NSAMPLES; ++i) lc_to_utc(&tms[i]);
    duration d0 = (clock::now() - tp0) / NSAMPLES;

    time_point tp1 = clock::now();
    for (int i=0; i<NSAMPLES; ++i) timelocal(&tms[i]);
    duration d1 = (clock::now() - tp1) / NSAMPLES;

    BOOST_TEST_MESSAGE( "lc_to_utc() time: "
        << duration_cast<nanoseconds>(d0).count() << " ns" );

    BOOST_TEST_MESSAGE( "timelocal() time: "
        << duration_cast<nanoseconds>(d1).count() << " ns" );

    delete[] tms;
}
#endif

BOOST_AUTO_TEST_SUITE_END()

} // namespace tztree_test
