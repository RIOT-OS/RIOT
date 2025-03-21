/*
 * Copyright (C) 2019 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 */
#include <string.h>
#include <errno.h>

#include "embUnit.h"

#include "periph/rtc.h"

static void _test_equal_tm(const struct tm *a, const struct tm *b)
{
    TEST_ASSERT_EQUAL_INT((a)->tm_sec,  (b)->tm_sec);
    TEST_ASSERT_EQUAL_INT((a)->tm_min,  (b)->tm_min);
    TEST_ASSERT_EQUAL_INT((a)->tm_hour, (b)->tm_hour);
    TEST_ASSERT_EQUAL_INT((a)->tm_mday, (b)->tm_mday);
    TEST_ASSERT_EQUAL_INT((a)->tm_mon,  (b)->tm_mon);
    TEST_ASSERT_EQUAL_INT((a)->tm_year, (b)->tm_year);
    TEST_ASSERT_EQUAL_INT((a)->tm_wday, (b)->tm_wday);
    TEST_ASSERT_EQUAL_INT((a)->tm_yday, (b)->tm_yday);
    TEST_ASSERT_EQUAL_INT(0, rtc_tm_compare(a, b));
}

static void test_rtc_compat(void)
{
    struct tm t1 = {
        .tm_sec  = 42,
        .tm_min  = 37,
        .tm_hour = 13,
        .tm_mday = 23,
        .tm_mon  =  5,
        .tm_year = 84,
        .tm_wday = 0,
        .tm_yday = 0,
        .tm_isdst = -1,
    };

    struct tm t2 = t1;

    mktime(&t1);
    rtc_tm_normalize(&t2);
    _test_equal_tm(&t1, &t2);
}

static void test_rtc_sec_wrap(void)
{
    struct tm t1 = {
        .tm_sec  = 360,
        .tm_min  =  58,
        .tm_hour =  23,
        .tm_mday =  30,
        .tm_mon  =   5,
        .tm_year = 100,
        .tm_wday =   0,
        .tm_yday =   0,
        .tm_isdst=  -1,
    };

    struct tm t2 = t1;

    mktime(&t1);
    rtc_tm_normalize(&t2);
    _test_equal_tm(&t1, &t2);
}

static void test_rtc_lyear(void)
{
    struct tm t1 = {
        .tm_sec  = 360,
        .tm_min  =  58,
        .tm_hour =  23,
        .tm_mday =  28,
        .tm_mon  =   1,
        .tm_year = 100,
        .tm_wday =   0,
        .tm_yday =   0,
        .tm_isdst=   0
    };

    struct tm t2 = t1;

    mktime(&t1);
    rtc_tm_normalize(&t2);
    _test_equal_tm(&t1, &t2);
}

static void test_rtc_nyear(void)
{
    struct tm t1 = {
        .tm_sec  = 360,
        .tm_min  =  58,
        .tm_hour =  23,
        .tm_mday =  28,
        .tm_mon  =   1,
        .tm_year = 101,
        .tm_wday =   0,
        .tm_yday =   0,
        .tm_isdst=   0
    };

    struct tm t2 = t1;

    mktime(&t1);
    rtc_tm_normalize(&t2);
    _test_equal_tm(&t1, &t2);
}

static void test_rtc_ywrap(void)
{
    struct tm t1 = {
        .tm_sec  = 360,
        .tm_min  =  58,
        .tm_hour =  23,
        .tm_mday =  32,
        .tm_mon  =  11,
        .tm_year = 101,
        .tm_wday =   0,
        .tm_yday =   0,
        .tm_isdst=   0
    };

    struct tm t2 = t1;

    mktime(&t1);
    rtc_tm_normalize(&t2);
    _test_equal_tm(&t1, &t2);
}

static void test_rtc_year(void)
{
    struct tm t1 = {
        .tm_sec  = 360,
        .tm_min  =  58,
        .tm_hour =  23,
        .tm_mday =  32,
        .tm_mon  =  11,
        .tm_year = 100,
        .tm_wday =   0,
        .tm_yday =   0,
    };

    struct tm t2 = t1;

    for (int i = 0; i <= 2*366; ++i) {
        t1.tm_mday++;
        t2.tm_mday++;

        mktime(&t1);

        /* rtc_tm_normalize does not handle DST */
        if (t1.tm_isdst && !t2.tm_isdst) {
            t2.tm_hour++;
            t2.tm_isdst = 1;
        } else if (!t1.tm_isdst && t2.tm_isdst) {
            t2.tm_hour--;
            t2.tm_isdst = 0;
        }

        rtc_tm_normalize(&t2);
        _test_equal_tm(&t1, &t2);
    }
}

static void test_rtc_compare(void)
{
    struct tm t1 = {
        .tm_sec  =  10,
        .tm_min  =  58,
        .tm_hour =  23,
        .tm_mday =  32,
        .tm_mon  =  11,
        .tm_year = 100,
        .tm_wday =   0,
        .tm_yday =   0,
    };

    mktime(&t1);

    struct tm t2 = t1;

    TEST_ASSERT(rtc_tm_compare(&t1, &t2) == 0);

    t1.tm_hour += 1;
    mktime(&t1);
    TEST_ASSERT(rtc_tm_compare(&t1, &t2) > 0);

    t1.tm_mday -= 1;
    mktime(&t1);
    TEST_ASSERT(rtc_tm_compare(&t1, &t2) < 0);
}

static void test_rtc_mktime(void)
{
    struct tm t  = {
        .tm_sec  =  11,
        .tm_min  =  12,
        .tm_hour =  13,
        .tm_mday =  1,
        .tm_mon  =  0,
        .tm_year = 120,
        .tm_wday =   0,
        .tm_yday =   1,
    };

    mktime(&t);
    TEST_ASSERT_EQUAL_INT(47531, rtc_mktime(&t));

    t.tm_mday += 40;
    mktime(&t);
    TEST_ASSERT_EQUAL_INT(3503531, rtc_mktime(&t));

    t.tm_year += 3;
    mktime(&t);
    TEST_ASSERT_EQUAL_INT(98197931, rtc_mktime(&t));
}

static void test_mktime(void)
{
    /* second 1 of 1970 is defined 1 second of unixtime */
    struct tm t  = {
        .tm_sec  =  1,
        .tm_min  =  0,
        .tm_hour =  0,
        .tm_mday =  1,
        .tm_mon  =  0,
        .tm_year = 70,
        .tm_wday =   0,
        .tm_yday =   1,
    };
    /* This test will fail if mktime is applying timezone information
     * that is not 0 seconds offset from UTC (GMT) and therefore returning unixtime.
     * A usual reason for failure might be that nativ is run in a local not "UTC" timezone,
     * a "Fix" for this failure is setting the environment variable TZ to "UTC" */
    TEST_ASSERT_EQUAL_INT(1, mktime(&t));

    t  = (struct tm){
        .tm_sec  =  11,
        .tm_min  =  12,
        .tm_hour =  13,
        .tm_mday =  1,
        .tm_mon  =  0,
        .tm_year = 120,
        .tm_wday =   0,
        .tm_yday =   1,
    };

    TEST_ASSERT_EQUAL_INT(1577884331, mktime(&t));
}

static void test_rtc_localtime(void)
{
    struct tm t;

    const struct tm t1  = {
        .tm_sec  =  11,
        .tm_min  =  12,
        .tm_hour =  13,
        .tm_mday =  1,
        .tm_mon  =  0,
        .tm_year = 120,
        .tm_wday =   3,
        .tm_yday =   0
    };

    const struct tm t2  = {
        .tm_sec  =  11,
        .tm_min  =  12,
        .tm_hour =  13,
        .tm_mday =  10,
        .tm_mon  =  1,
        .tm_year = 120,
        .tm_wday =   1,
        .tm_yday =  40,
    };

    const struct tm t3  = {
        .tm_sec  =  11,
        .tm_min  =  12,
        .tm_hour =  13,
        .tm_mday =  10,
        .tm_mon  =  1,
        .tm_year = 123,
        .tm_wday =   5,
        .tm_yday =  40,
    };

    rtc_localtime(100, &t);
    TEST_ASSERT_EQUAL_INT(40, t.tm_sec);
    TEST_ASSERT_EQUAL_INT(1, t.tm_min);
    TEST_ASSERT_EQUAL_INT(120, t.tm_year);

    rtc_localtime(47531, &t);
    _test_equal_tm(&t1, &t);

    rtc_localtime(3503531, &t);
    _test_equal_tm(&t2, &t);

    rtc_localtime(98197931, &t);
    _test_equal_tm(&t3, &t);
}

Test *tests_rtc_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_rtc_compat),
        new_TestFixture(test_rtc_sec_wrap),
        new_TestFixture(test_rtc_lyear),
        new_TestFixture(test_rtc_nyear),
        new_TestFixture(test_rtc_ywrap),
        new_TestFixture(test_rtc_year),
        new_TestFixture(test_rtc_compare),
        new_TestFixture(test_rtc_mktime),
        new_TestFixture(test_mktime),
        new_TestFixture(test_rtc_localtime),
    };

    EMB_UNIT_TESTCALLER(rtc_tests, NULL, NULL, fixtures);

    return (Test *)&rtc_tests;
}

void tests_rtc(void)
{
    TESTS_RUN(tests_rtc_tests());
}
/** @} */
