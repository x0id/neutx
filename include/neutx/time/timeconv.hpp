// ex: ts=4 sw=4 ft=cpp et indentexpr=
/**
 * \file
 * \brief time converter
 *
 * \author Dmitriy Kargapolov
 * \since 10 Aug 2012
 *
 */

/*
 * Copyright (C) 2012 Dmitriy Kargapolov <dmitriy.kargapolov@gmail.com>
 * Use, modification and distribution are subject to the Boost Software
 * License, Version 1.0 (See accompanying file LICENSE_1_0.txt or copy
 * at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef _NEUTX_TIMECONV_HPP_
#define _NEUTX_TIMECONV_HPP_

#include <ctime>
#include <map>
#include <boost/integer_traits.hpp>

namespace neutx {

namespace time {

enum {
    mintime = boost::integer_traits<time_t>::const_min,
    maxtime = boost::integer_traits<time_t>::const_max
};

// check if gregorian year is leap year
inline bool is_leap(int year) {
    return year % 400 == 0 || (year % 4 == 0 && year % 100 > 0);
}

// calculate time difference between two broken-time points
inline long tmdiff(const struct tm& t1, const struct tm& t2) {
    if (t1.tm_year < t2.tm_year)
        return -tmdiff(t2, t1);
    long result = 0;
    for (int y = t2.tm_year; y < t1.tm_year; ++y)
        result += 365 + is_leap(1900 + y);
    result += t1.tm_yday - t2.tm_yday;
    result *= 24;
    result += t1.tm_hour - t2.tm_hour;
    result *= 60;
    result += t1.tm_min - t2.tm_min;
    result *= 60;
    result += t1.tm_sec - t2.tm_sec;
    return result;
}

// calculate seconds since Epoch from broken time
inline time_t to_secs(int y, int n, int d, int h, int m, int s) {
    static int dm[] =
        {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
    // days in prev years
    int x = y - 1;
    int dy = x/4 - x/100 + x/400 + x*365 + 366;
    // leap y adjustment for month 3..12
    int ly = n > 2 && is_leap(y) ? 1 : 0;
    // UTC days == gregorian days - 719528
    int days = dy + dm[n - 1] + ly + d - 719529;
    // UTC secs
    return (time_t) days * 86400 + h * 3600 + m * 60 + s;
}

struct tim {
    time_t m_t;
    struct tm m_tm;
    struct tm m_tm_utc;
    bool m_ok;

    tim() : m_ok(false) {}

    tim(time_t a_t) : m_t(a_t) {
        localtime();
    }

    bool localtime() {
        m_ok  = (&m_tm == localtime_r(&m_t, &m_tm));
        m_ok &= (&m_tm_utc == gmtime_r(&m_t, &m_tm_utc));
        return m_ok;
    }

    long tdiff(const tim& t) const {
        return m_t - t.m_t;
    }

    long tmdiff(const tim& t) const {
        return time::tmdiff(m_tm, t.m_tm);
    }

    bool compare_diffs(const tim& t) {
        return (!m_ok || !t.m_ok) ? (m_ok == t.m_ok) : (tmdiff(t) == tdiff(t));
    }
};

namespace {

inline long year_seconds(long year) {
    if (is_leap(year))
        return 366 * 24 * 3600;
    else
        return 365 * 24 * 3600;
}

inline time_t year_to_time_t(long year) {
    long t = 0;
    long y = 1970;
    while (y != year) {
        if (y < year) {
            long seconds = year_seconds(y++);
            if (t > maxtime - seconds) {
                t = maxtime;
                break;
            }
            t += seconds;
        } else {
            long seconds = year_seconds(--y);
            if (t < mintime + seconds) {
                t = mintime;
                break;
            }
            t -= seconds;
        }
    }
    return t;
}

inline void hunt(tim& t1, tim& t2) {
    for (;;) {
        long diff = t2.m_t - t1.m_t;
        if (diff < 2)
            break;
        tim t = t1;
        t.m_t += diff / 2;
        if (t.m_t <= t1.m_t)
            ++t.m_t;
        else if (t.m_t >= t2.m_t)
            --t.m_t;
        t.localtime();
        if (t.compare_diffs(t1))
            t1 = t;
        else
            t2 = t;
    }
}

} // anonymous namespace

class tzdata {
    friend class tzdata_codec;

public:
    // timezone offset switch point
    struct shift_point {
        void set(time_t t, long o1, long o2) {
            t0 = t;
            off1 = o1;
            off2 = o2;
        }
        time_t t0; // switch time (UTC seconds)
        long off1; // old seconds east of UTC
        long off2; // new seconds east of UTC
    };

private:
    // local secs to shift point map type
    typedef std::map<time_t, shift_point> ltztree_t;
    typedef ltztree_t::value_type ltzval_t;

    // offset data for lookup by utc seconds
    struct loc_offset {
        void set(long off, bool dst) {
            offset = off;
            is_dst = dst;
        }
        long offset; // seconds east of UTC
        bool is_dst; // is daylight saving on
    };

    // utc secs to loc_offset map type
    typedef std::map<time_t, loc_offset> utztree_t;
    typedef utztree_t::value_type utzval_t;

    // for containers...
    tzdata()
        : m_switch_t(0), m_def_offset(0), m_def_is_dst(false)
        , m_lotime(0), m_hitime(0)
    {}

    void load_tztree(int y1, int y2, const char *tz) {
        // save original TZ
        const char *save_tz = getenv("TZ");
        // set given TZ
        setenv("TZ", tz, 1);
        tzset();
        // discover switch points, load the tree
        load_tztree(y1, y2);
        // restore TZ
        if (save_tz)
            setenv("TZ", save_tz, 1);
        else
            unsetenv("TZ");
        tzset();
    }

    void load_tztree(int y1, int y2) {
        m_lotime = year_to_time_t(y1);
        m_hitime = year_to_time_t(y2);
        m_switch_t = m_hitime;

        const int l_step = 3600 * 12;
        time_t hi_edge = m_hitime - l_step;

        // init m_def_offset in case we won't find any switch
        tim t0(1398127371);
        m_def_offset = tmdiff(t0.m_tm, t0.m_tm_utc);

        tim t1(m_lotime);

        for (tim t2 = t1; t2.m_t < m_hitime; t1 = t2) {
            if (t2.m_t >= hi_edge)
                t2 = m_hitime;
            else
                t2.m_t += l_step;
            t2.localtime();
            if (!t2.compare_diffs(t1)) {
                hunt(t1, t2);
                long o1 = tmdiff(t1.m_tm, t1.m_tm_utc);
                long o2 = tmdiff(t2.m_tm, t2.m_tm_utc);
                if (o1 < 0 && t2.m_t < mintime - o1) continue;
                if (o1 > 0 && t2.m_t > maxtime - o1) continue;
                if (o2 < 0 && t2.m_t < mintime - o2) continue;
                if (o2 > 0 && t2.m_t > maxtime - o2) continue;
                // time gap
                if (o1 < o2) {
                    m_ltztree[t2.m_t + o1].set(t2.m_t, o1, o1);
                    m_ltztree[t2.m_t + o2].set(t2.m_t, o1, o2);
                    m_utztree[t2.m_t].set(o1, false);
                    m_def_is_dst = true;
                }
                // time repetition
                else if (o1 > o2) {
                    m_ltztree[t2.m_t + o2].set(t2.m_t, o1, o1);
                    m_ltztree[t2.m_t + o1].set(t2.m_t, o1, o2);
                    m_utztree[t2.m_t].set(o1, true);
                    m_def_is_dst = false;
                }
                m_switch_t = t2.m_t;
                m_def_offset = o2;
            }
        }
    }

    ltztree_t m_ltztree; // local secs to shift point ordered map
    utztree_t m_utztree; // utc secs to offset ordered map
    time_t m_switch_t;   // time of last switch or m_hitime if no switch exists
    long m_def_offset;   // default tz offset
    bool m_def_is_dst;   // default dst flag
    // domain of the tree
    time_t m_lotime;     // minimum time, UTC seconds
    time_t m_hitime;     // maximum time, UTC seconds

public:
    // construct tzdata from result of particular
    // timezone discovering process in given years interval
    tzdata(int y1, int y2, const char *tz = 0) {
        if (tz)
            load_tztree(y1, y2, tz);
        else
            load_tztree(y1, y2);
    }

    // get offset(s) for local time represented in seconds
    void offset(time_t local_secs, shift_point& ret) const {
        ltztree_t::const_iterator it = m_ltztree.upper_bound(local_secs);
        if (it == m_ltztree.end()) {
            ret.t0 = m_switch_t;
            ret.off1 = m_def_offset;
            ret.off2 = m_def_offset;
        } else
            ret = it->second;
    }

    // get offset for utc time represented in seconds
    long offset(time_t utc_secs) const {
        utztree_t::const_iterator it = m_utztree.upper_bound(utc_secs);
        if (it == m_utztree.end()) {
            return m_def_offset;
        } else
            return it->second.offset;
    }

    // get offset for utc time represented in seconds and set dst flag
    long offset(time_t utc_secs, bool& dst) const {
        utztree_t::const_iterator it = m_utztree.upper_bound(utc_secs);
        if (it == m_utztree.end()) {
            dst = m_def_is_dst;
            return m_def_offset;
        } else {
            dst = it->second.is_dst;
            return it->second.offset;
        }
    }
};

} // namespace time

} // namespace neutx

#endif // _NEUTX_TIMECONV_HPP_
