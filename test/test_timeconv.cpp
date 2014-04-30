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
#include <boost/foreach.hpp>
#include <fstream>

#include <boost/test/unit_test.hpp>

#if defined HAVE_BOOST_CHRONO
#include <boost/chrono/system_clocks.hpp>
#endif

namespace neutx {
namespace time {

bool operator==(const tzdata::tzval_t& v1, const tzdata::tzval_t& v2) {
    if (v1.first != v2.first) return false;
    if (v1.second.t0 != v2.second.t0) return false;
    if (v1.second.off1 != v2.second.off1) return false;
    if (v1.second.off2 != v2.second.off2) return false;
    return true;
}

std::ostream& operator<<(std::ostream& out, const tzdata::tzval_t& t) {
    out << "t = " << t.first << ", p = ";
    return out << "#shift_point{t0 = " << t.second.t0 << ", off1 = " <<
        t.second.off1 << ", off2 = " << t.second.off2 << "}";
}

} // namespace time
} // namespace neutx

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
    const char *save_tz;

    f() {
        const char *timezone = "America/New_York";
        // save original TZ
        save_tz = getenv("TZ");
        // set given TZ
        setenv("TZ", timezone, 1);
        tzset();
        // compile our own data
        tz = new nt::tzdata(1950, 2100, timezone);
    }
    ~f() {
        // restore TZ
        setenv("TZ", save_tz, 1);
        tzset();
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

struct codec {
    class ofile {
        std::ofstream ofs;
    public:
        ofile(const char *file) {
            ofs.exceptions(std::ofstream::failbit | std::ofstream::badbit);
            ofs.open(file, std::ios::out | std::ios::binary | std::ios::trunc);
        }
        ~ofile() {
            try {
                ofs.close();
            } catch (...) {
            }
        }
        void write(const void *buf, size_t len) {
            ofs.write((const char *)buf, len);
        }
    };

    class ifile {
        std::ifstream ifs;
    public:
        ifile(const char *fname) {
            ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            ifs.open(fname, std::ios::in | std::ios::binary);
        }
        ifile() {
            try {
                ifs.close();
            } catch (...) {
            }
        }
        void read(void *buf, size_t len) {
            ifs.read((char *)buf, len);
        }
    };

    struct head {
        nt::tzdata::tztree_t::size_type n;
        time_t lo, hi, sw;
        long off;
    };

    void dump(const char *file, const nt::tzdata& data) {
        ofile out(file);
        head h; bzero(&h, sizeof(h));
        h.n = data.tztree().size();
        h.lo = data.lotime();
        h.hi = data.hitime();
        h.sw = data.last_switch_time();
        h.off = data.default_offset();
        out.write(&h, sizeof(h));
        BOOST_FOREACH(const nt::tzdata::tzval_t& v, data.tztree())
            out.write(&v, sizeof(v));
    }

    nt::tzdata *load(const char *file) {
        ifile in(file);
        head h;
        in.read(&h, sizeof(h));
        nt::tzdata *tz = new nt::tzdata(h.lo, h.hi, h.off);
        nt::tzdata::tztree_t::size_type i;
        nt::tzdata::tzval_t v;
        for (i=0; i<h.n; ++i) {
            in.read(&v, sizeof(v));
            tz->add_point(v.first, v.second);
        }
        return tz;
    }

};

BOOST_AUTO_TEST_SUITE( test_tztree )

BOOST_AUTO_TEST_CASE( load_tztree_test )
{
    BOOST_TEST_MESSAGE("loading tz data");
    nt::tzdata *tz = new nt::tzdata(1950, 2100, "America/New_York");
    BOOST_REQUIRE(tz != 0);
    delete tz;
}

BOOST_FIXTURE_TEST_CASE( export_import_test, codec )
{
    BOOST_TEST_MESSAGE("loading tz data");
    nt::tzdata *tz1 = new nt::tzdata(1950, 2100, "America/New_York");
    BOOST_REQUIRE(tz1 != 0);

    const char *file = "file.bin";
    BOOST_TEST_MESSAGE("writing to file " << file);
    dump(file, *tz1);

    BOOST_TEST_MESSAGE("reading from file " << file);
    nt::tzdata *tz2 = load(file);
    BOOST_REQUIRE(tz2 != 0);

    // compare original tztree to what we read from file
    time_t t1, t2;
    t1 = tz1->lotime(); t2 = tz2->lotime(); BOOST_REQUIRE_EQUAL(t1, t2);
    t1 = tz1->hitime(); t2 = tz2->hitime(); BOOST_REQUIRE_EQUAL(t1, t2);
    t1 = tz1->last_switch_time(); t2 = tz2->last_switch_time();
    BOOST_REQUIRE_EQUAL(t1, t2);
    long off1 = tz1->default_offset(); long off2 = tz2->default_offset();
    BOOST_REQUIRE_EQUAL(off1, off2);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        tz1->tztree().begin(), tz1->tztree().end(),
        tz2->tztree().begin(), tz2->tztree().end()
    );

    delete tz2;
    delete tz1;
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
    double coeff = ((double)tmax - tmin) / RAND_MAX;
    for (int i=0; i<NSAMPLES; ++i) {
        time_t t = boost::numeric_cast<time_t>(tmin + rand() * coeff);
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
    double coeff = ((double)tmax - tmin) / RAND_MAX;
    for (int i=0; i<NSAMPLES; ++i) {
        time_t t = boost::numeric_cast<time_t>(tmin + rand() * coeff);
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
