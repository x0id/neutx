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

namespace {

    class ofile {
        std::ofstream ofs;
    public:
        ofile(const char *file) {
            ofs.exceptions(std::ofstream::failbit | std::ofstream::badbit);
            ofs.open(file, std::ios::out | std::ios::binary | std::ios::trunc);
        }
        ~ofile() {
            ofs.close();
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
            ifs.close();
        }
        void read(void *buf, size_t len) {
            ifs.read((char *)buf, len);
        }
    };
}

namespace neutx {
namespace time {

struct tzdata_codec {
    struct head {
        tzdata::ltztree_t::size_type ln;
        tzdata::utztree_t::size_type un;
        time_t lo, hi, sw;
        long off; bool dst;
    };

    static void dump(const char *file, const tzdata& data) {
        ofile out(file);
        head h; bzero(&h, sizeof(h));
        h.ln = data.m_ltztree.size();
        h.un = data.m_utztree.size();
        h.lo = data.m_lotime;
        h.hi = data.m_hitime;
        h.sw = data.m_switch_t;
        h.off = data.m_def_offset;
        h.dst = data.m_def_is_dst;
        out.write(&h, sizeof(h));
        BOOST_FOREACH(const tzdata::ltzval_t& v, data.m_ltztree)
            out.write(&v, sizeof(v));
        BOOST_FOREACH(const tzdata::utzval_t& v, data.m_utztree)
            out.write(&v, sizeof(v));
    }

    static tzdata *load(const char *file) {
        ifile in(file);
        head h;
        in.read(&h, sizeof(h));
        tzdata *tz = new tzdata;
        tz->m_switch_t = h.sw;
        tz->m_def_offset = h.off;
        tz->m_def_is_dst = h.dst;
        tz->m_lotime = h.lo;
        tz->m_hitime = h.hi;
        { tzdata::ltztree_t::size_type i;
          tzdata::ltzval_t v;
          for (i=0; i<h.ln; ++i) {
            in.read(&v, sizeof(v));
            tz->m_ltztree.insert(v);
        } }
        { tzdata::utztree_t::size_type i;
          tzdata::utzval_t v;
          for (i=0; i<h.un; ++i) {
            in.read(&v, sizeof(v));
            tz->m_utztree.insert(v);
        } }
        return tz;
    }

    static time_t lotime(const tzdata& tz) { return tz.m_lotime; }
    static time_t hitime(const tzdata& tz) { return tz.m_hitime; }

    static void compare(const tzdata& t1, const tzdata& t2) {
        BOOST_REQUIRE_EQUAL(t1.m_lotime, t2.m_lotime);
        BOOST_REQUIRE_EQUAL(t1.m_hitime, t2.m_hitime);
        BOOST_REQUIRE_EQUAL(t1.m_switch_t, t2.m_switch_t);
        BOOST_REQUIRE_EQUAL(t1.m_def_offset, t2.m_def_offset);
        BOOST_REQUIRE_EQUAL(t1.m_def_is_dst, t2.m_def_is_dst);
        BOOST_REQUIRE_EQUAL_COLLECTIONS(
            t1.m_ltztree.begin(), t1.m_ltztree.end(),
            t2.m_ltztree.begin(), t2.m_ltztree.end()
        );
        BOOST_REQUIRE_EQUAL_COLLECTIONS(
            t1.m_utztree.begin(), t1.m_utztree.end(),
            t2.m_utztree.begin(), t2.m_utztree.end()
        );
    }

    typedef tzdata::ltzval_t shiftp_t;
    typedef tzdata::utzval_t offset_t;
};

typedef tzdata_codec::shiftp_t shiftp_t;
typedef tzdata_codec::offset_t offset_t;

bool operator==(const shiftp_t& v1, const shiftp_t& v2) {
    if (v1.first != v2.first) return false;
    if (v1.second.t0 != v2.second.t0) return false;
    if (v1.second.off1 != v2.second.off1) return false;
    if (v1.second.off2 != v2.second.off2) return false;
    return true;
}

bool operator==(const offset_t& v1, const offset_t& v2) {
    if (v1.first != v2.first) return false;
    if (v1.second.offset != v2.second.offset) return false;
    if (v1.second.is_dst != v2.second.is_dst) return false;
    return true;
}

std::ostream& operator<<(std::ostream& out, const shiftp_t& t) {
    out << "t = " << t.first << ", p = ";
    return out << "#shift_point{t0 = " << t.second.t0 << ", off1 = " <<
        t.second.off1 << ", off2 = " << t.second.off2 << "}";
}

std::ostream& operator<<(std::ostream& out, const offset_t& t) {
    out << "t = " << t.first << ", p = ";
    return out << "#loc_offset{off = " << t.second.offset << ", dst = " <<
        t.second.is_dst << "}";
}

} // namespace time
} // namespace neutx

bool operator==(const tm& t1, const tm& t2) {
    if (t1.tm_sec != t2.tm_sec) return false;
    if (t1.tm_min != t2.tm_min) return false;
    if (t1.tm_hour != t2.tm_hour) return false;
    if (t1.tm_mday != t2.tm_mday) return false;
    if (t1.tm_mon != t2.tm_mon) return false;
    if (t1.tm_year != t2.tm_year) return false;
    if (t1.tm_wday != t2.tm_wday) return false;
    if (t1.tm_yday != t2.tm_yday) return false;
    if (t1.tm_isdst != t2.tm_isdst) return false;
    return true;
}

std::ostream& operator<<(std::ostream& out, const tm& t) {
    return out << "#tm{" << t.tm_sec << ":" << t.tm_min << ":" << t.tm_hour <<
        ":" << t.tm_mday << ":" << t.tm_mon << ":" << t.tm_year << ":" <<
        t.tm_wday << ":" << t.tm_yday << ":" << t.tm_isdst << "}";
}

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

    struct tm *utc_to_lc(const time_t *t) {
        bool dst;
        time_t loc = *t + tz->offset(*t, dst);
        struct tm *ret = gmtime(&loc);
        ret->tm_isdst = dst;
        return ret;
    }
};

struct codec {
    void dump(const char *file, const nt::tzdata& data) {
        nt::tzdata_codec::dump(file, data);
    }

    nt::tzdata *load(const char *file) {
        return nt::tzdata_codec::load(file);
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
    nt::tzdata_codec::compare(*tz1, *tz2);

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
    time_t tmin = nt::tzdata_codec::lotime(*tz);
    time_t tmax = nt::tzdata_codec::hitime(*tz);
    double coeff = ((double)tmax - tmin) / RAND_MAX;
    for (int i=0; i<NSAMPLES; ++i) {
        time_t t = boost::numeric_cast<time_t>(tmin + rand() * coeff);
        BOOST_REQUIRE(t >= tmin && t <= tmax);
        struct tm tm1 = *localtime(&t);
        struct tm tm2 = *utc_to_lc(&t);
        BOOST_REQUIRE_EQUAL(tm1, tm2);
        time_t t1 = lc_to_utc(&tm1);
        time_t t2 = timelocal(&tm1);
        BOOST_REQUIRE_EQUAL(t1, t2);
    }
}

#if defined HAVE_BOOST_CHRONO
BOOST_FIXTURE_TEST_CASE( time_test, f )
{
    BOOST_TEST_MESSAGE("timing test using " << NSAMPLES << " samples");

    struct tm *tms = new tm[NSAMPLES];
    srand(1);
    time_t tmin = nt::tzdata_codec::lotime(*tz);
    time_t tmax = nt::tzdata_codec::hitime(*tz);
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
