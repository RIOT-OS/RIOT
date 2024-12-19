/*
 * Copyright (C) 2019 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     sys_rtc_utils
 * @{
 *
 * @file
 * @brief       common RTC helper functions
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "periph/rtc.h"

#ifndef RTC_NORMALIZE_COMPAT
#define RTC_NORMALIZE_COMPAT (0)
#endif

#define MINUTE  (60U)
#define HOUR    (60U * MINUTE)
#define DAY     (24U * HOUR)

/*
 * The rules here are (to be checked in that explicit order):
 *  1. If the year is not a multiple of four, it is not a leap year.
 *  2. If the year is a multiple of 400, it is a leap year
 *  3. If the year is a multiple of 100, it is not a leap year.
 */
static int _is_leap_year(int year)
{
    /* not a multiple of four, so not a leap year */
    if (year & 0x3) {
        return 0;
    }

    /*
     * We know that year is a multiple of four by now. If it is also a multiple of 25,
     * it is therefore a multiple of hundred. Thus (with year being a multiple of
     * four), !!(year %25) is 0 if and only if year is a multiple of 100. !(year & 15)
     * is 0 for all year that are not a multiple of 16.
     *
     * As !!(year %25) only returns 0 for years that are a multiple of 100 (out of all
     * the multiples of four), || !(year &15) is only evaluated for these. Out of all
     * multiples of 100, !(year &15) is 0 except for those that are a multiple of 400.
     */
    return !!(year % 25) || !(year & 15);
}

/*
 * 1. Every fourth year was a leap year.
 * 2. Subtract all years divisible by 100, those are divisible by 4 but no leap years.
 * 3. Add back all years divisible by 400, those are divisible by 100 but leap years.
 */
static unsigned _leap_years_before(unsigned year)
{
    --year;
    return (year / 4) - (year / 100) + (year / 400);
}

static unsigned _leap_years_since_epoch(unsigned year)
{
    if (year < RIOT_EPOCH) {
        return 0;
    }

    return _leap_years_before(year) - _leap_years_before(RIOT_EPOCH);
}

static int _month_length(int month, int year)
{
    if (month == 1) {
        return 28 + _is_leap_year(year);
    }

   /*
    * From January (0) to July (6), with the exception of the already handled February
    * (month == 1), the months have 31 and 30 days in turns. Thus, subtracting 1 da
    * if the month is odd works here.
    *
    * From August (7) to December (11) again we have 31 and 30 days in turns. If we
    * reset the counter at August (7) to 0, we can therefore again subtract 1 day if
    * the month counter is odd. (month % 7) here is used to reset the counter to zero
    * at August (7).
    */
    return 31 - ((month % 7) & 1);
}

#if RTC_NORMALIZE_COMPAT
static int _wday(int day, int month, int year)
{
    /* Tomohiko Sakamoto's Algorithm
     * https://en.wikipedia.org/wiki/Determination_of_the_day_of_the_week#Sakamoto's_methods
     */
    static const uint8_t t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    year -= month < 2;
    return (year + year/4 - year/100 + year/400 + t[month] + day) % 7;
}
#endif /* RTC_NORMALIZE_COMPAT */

static int _yday(int day, int month, int year)
{
    /* Cumulative number of days since start of the year. In order to squeeze this into
       8 bits, we wrap around at 255 and handle this later. */
    static const uint8_t d[] = { 0,  31,  59, 90, 120, 151,
                               181, 212, 243, 17,  48,  78};

    /* If year is a leap year, February (1) has an extra day. */
    if (month > 1) {
        day += _is_leap_year(year);
    }

    /* For months October (9), November (10), and December (11) number in d was wrapped
       around to fit into 8 bits. We undo this now. */
    if (month > 8) {
        day |= 0x100;
    }

    /* subtract 1 as counting starts at zero, e.g.
       2019-01-01 will be be day 0 in year 2019 */
    return d[month] + day - 1;
}

void rtc_tm_normalize(struct tm *t)
{
    div_t d;

    if (t->tm_mday == 0) {
        t->tm_mday = 1;
    }

    d = div(t->tm_sec, 60);
    t->tm_min += d.quot;
    t->tm_sec  = d.rem;

    d = div(t->tm_min, 60);
    t->tm_hour += d.quot;
    t->tm_min   = d.rem;

    d = div(t->tm_hour, 24);
    t->tm_mday += d.quot;
    t->tm_hour  = d.rem;

    d = div(t->tm_mon, 12);
    t->tm_year += d.quot;
    t->tm_mon   = d.rem;

    /* Loop to handle days overflowing the month. */
    while (1) {
        int days = _month_length(t->tm_mon, t->tm_year + 1900);

        if (t->tm_mday <= days) {
            break;
        }

        if (++t->tm_mon > 11) {
            t->tm_mon = 0;
            ++t->tm_year;
        }

        t->tm_mday -= days;
    }

#if RTC_NORMALIZE_COMPAT
    t->tm_yday = _yday(t->tm_mday, t->tm_mon, t->tm_year + 1900);
    t->tm_wday = _wday(t->tm_mday, t->tm_mon, t->tm_year + 1900);
#endif
}

uint32_t rtc_mktime(const struct tm *t)
{
    unsigned year = t->tm_year + 1900;
    uint32_t time = t->tm_sec
                  + t->tm_min * MINUTE
                  + t->tm_hour * HOUR
                  + _yday(t->tm_mday, t->tm_mon, year) * DAY;

    unsigned leap_years = _leap_years_since_epoch(year);
    unsigned common_years = (year - RIOT_EPOCH) - leap_years;

    time += (leap_years * 366 + common_years * 365) * DAY;

    return time;
}

void rtc_localtime(uint32_t time, struct tm *t)
{
    uint32_t y_secs = _is_leap_year(RIOT_EPOCH) ? (366 * DAY) : (365 * DAY);
    unsigned year = RIOT_EPOCH;

    while (time > y_secs) {
        time -= y_secs;
        ++year;
        y_secs = _is_leap_year(year) ? (366 * DAY) : (365 * DAY);
    }

    memset(t, 0, sizeof(*t));
    t->tm_sec  = time;
    t->tm_year = year - 1900;

    rtc_tm_normalize(t);
}

#define RETURN_IF_DIFFERENT(a, b, member)   \
    if (a->member != b->member) {           \
        return a->member-b->member;         \
    }

int rtc_tm_compare(const struct tm *a, const struct tm *b)
{
    RETURN_IF_DIFFERENT(a, b, tm_year);
    RETURN_IF_DIFFERENT(a, b, tm_mon);
    RETURN_IF_DIFFERENT(a, b, tm_mday);
    RETURN_IF_DIFFERENT(a, b, tm_hour);
    RETURN_IF_DIFFERENT(a, b, tm_min);
    RETURN_IF_DIFFERENT(a, b, tm_sec);

    return 0;
}

bool rtc_tm_valid(const struct tm *t)
{
    if (t->tm_sec < 0) {
        return false;
    }

    if (t->tm_min < 0) {
        return false;
    }

    if (t->tm_hour < 0) {
        return false;
    }

    if (t->tm_mday < 0) {
        return false;
    }

    if (t->tm_mon < 0) {
        return false;
    }

    if (t->tm_year < 0) {
        return false;
    }

    struct tm norm = *t;
    rtc_tm_normalize(&norm);
    return rtc_tm_compare(t, &norm) == 0;
}
