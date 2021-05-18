/*
 * Copyright (C) 2020 Benjamin Valentin
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
#include "periph/rtt.h"
#include "timex.h"
#include "rtt_rtc.h"

void rtt_add_ticks(uint64_t ticks);

static void test_set_time(void)
{
    struct tm t1 = {
        .tm_sec  = 42,
        .tm_min  = 37,
        .tm_hour = 13,
        .tm_mday = 23,
        .tm_mon  =  5,
        .tm_year = 120,
        .tm_wday = 0,
        .tm_yday = 0,
        .tm_isdst= 0,
    };
    struct tm now;

    rtc_set_time(&t1);
    rtc_get_time(&now);
    TEST_ASSERT_EQUAL_INT(0, rtc_tm_compare(&t1, &now));

    rtt_add_ticks(60LU * RTT_FREQUENCY);
    t1.tm_min++;
    rtc_get_time(&now);
    TEST_ASSERT_EQUAL_INT(t1.tm_sec, now.tm_sec);
    TEST_ASSERT_EQUAL_INT(t1.tm_min, now.tm_min);
    TEST_ASSERT_EQUAL_INT(0, rtc_tm_compare(&t1, &now));

    rtt_add_ticks(60LU * 60 * RTT_FREQUENCY);
    t1.tm_hour++;
    rtc_get_time(&now);
    TEST_ASSERT_EQUAL_INT(t1.tm_sec, now.tm_sec);
    TEST_ASSERT_EQUAL_INT(t1.tm_min, now.tm_min);
    TEST_ASSERT_EQUAL_INT(t1.tm_hour, now.tm_hour);
    TEST_ASSERT_EQUAL_INT(0, rtc_tm_compare(&t1, &now));

    rtt_add_ticks(60LU * RTT_FREQUENCY);
    t1.tm_min++;
    rtc_get_time(&now);
    TEST_ASSERT_EQUAL_INT(t1.tm_sec, now.tm_sec);
    TEST_ASSERT_EQUAL_INT(t1.tm_min, now.tm_min);
    TEST_ASSERT_EQUAL_INT(0, rtc_tm_compare(&t1, &now));
}

static void _alarm_cb(void *arg)
{
    struct tm now, *expt = arg;

    rtc_get_time(&now);

    TEST_ASSERT_EQUAL_INT(expt->tm_sec, now.tm_sec);
    TEST_ASSERT_EQUAL_INT(expt->tm_min, now.tm_min);
    TEST_ASSERT_EQUAL_INT(0, rtc_tm_compare(expt, &now));

    /* use dst to signal how often cb was called */
    expt->tm_isdst++;
}

static void test_set_alarm(void)
{
    struct tm t1 = {
        .tm_sec  = 42,
        .tm_min  = 37,
        .tm_hour = 13,
        .tm_mday = 23,
        .tm_mon  =  5,
        .tm_year = 120,
        .tm_wday = 0,
        .tm_yday = 0,
        .tm_isdst= 0,
    };
    struct tm now, alarm = t1;
    alarm.tm_hour++;

    rtc_set_time(&t1);
    rtc_set_alarm(&alarm, _alarm_cb, &alarm);

    rtt_add_ticks(60UL * RTT_FREQUENCY);
    t1.tm_min++;
    rtc_get_time(&now);
    TEST_ASSERT_EQUAL_INT(t1.tm_sec, now.tm_sec);
    TEST_ASSERT_EQUAL_INT(t1.tm_min, now.tm_min);
    TEST_ASSERT_EQUAL_INT(0, rtc_tm_compare(&t1, &now));

    rtt_add_ticks(24*60*60UL * RTT_FREQUENCY);
    t1.tm_mday++;
    rtc_get_time(&now);
    TEST_ASSERT_EQUAL_INT(t1.tm_sec, now.tm_sec);
    TEST_ASSERT_EQUAL_INT(t1.tm_min, now.tm_min);
    TEST_ASSERT_EQUAL_INT(0, rtc_tm_compare(&t1, &now));

    TEST_ASSERT_EQUAL_INT(1, alarm.tm_isdst);
}

static void test_set_alarm_short(void)
{
    struct tm t1 = {
        .tm_sec  = 42,
        .tm_min  = 37,
        .tm_hour = 13,
        .tm_mday = 23,
        .tm_mon  =  5,
        .tm_year = 120,
        .tm_wday = 0,
        .tm_yday = 0,
        .tm_isdst= 0,
    };
    struct tm now, alarm = t1;
    alarm.tm_sec += 10;

    rtc_set_time(&t1);
    rtc_set_alarm(&alarm, _alarm_cb, &alarm);

    rtt_add_ticks(60LU * RTT_FREQUENCY);
    t1.tm_min++;
    rtc_get_time(&now);
    TEST_ASSERT_EQUAL_INT(t1.tm_sec, now.tm_sec);
    TEST_ASSERT_EQUAL_INT(t1.tm_min, now.tm_min);
    TEST_ASSERT_EQUAL_INT(0, rtc_tm_compare(&t1, &now));

    rtt_add_ticks(60LU * 60 * RTT_FREQUENCY);
    t1.tm_hour++;
    rtc_get_time(&now);
    TEST_ASSERT_EQUAL_INT(t1.tm_sec, now.tm_sec);
    TEST_ASSERT_EQUAL_INT(t1.tm_min, now.tm_min);
    TEST_ASSERT_EQUAL_INT(0, rtc_tm_compare(&t1, &now));

    TEST_ASSERT_EQUAL_INT(1, alarm.tm_isdst);
}

static void test_set_alarm_set_time(void)
{
    struct tm t1 = {
        .tm_sec  = 42,
        .tm_min  = 37,
        .tm_hour = 13,
        .tm_mday = 23,
        .tm_mon  =  5,
        .tm_year = 120,
        .tm_wday = 0,
        .tm_yday = 0,
        .tm_isdst= 0,
    };
    struct tm now, alarm = t1;
    alarm.tm_hour += 5;

    rtc_set_time(&t1);
    rtc_set_alarm(&alarm, _alarm_cb, &alarm);

    t1.tm_hour += 4;
    rtc_set_time(&t1);

    rtt_add_ticks(60LU * RTT_FREQUENCY);
    t1.tm_min++;
    rtc_get_time(&now);
    TEST_ASSERT_EQUAL_INT(t1.tm_sec, now.tm_sec);
    TEST_ASSERT_EQUAL_INT(t1.tm_min, now.tm_min);
    TEST_ASSERT_EQUAL_INT(0, rtc_tm_compare(&t1, &now));

    rtt_add_ticks(60LU * 60 * RTT_FREQUENCY);
    t1.tm_hour++;
    rtc_get_time(&now);
    TEST_ASSERT_EQUAL_INT(t1.tm_sec, now.tm_sec);
    TEST_ASSERT_EQUAL_INT(t1.tm_min, now.tm_min);
    TEST_ASSERT_EQUAL_INT(0, rtc_tm_compare(&t1, &now));

    TEST_ASSERT_EQUAL_INT(1, alarm.tm_isdst);

    rtc_set_alarm(&alarm, _alarm_cb, &alarm);

    t1.tm_hour--;
    rtc_set_time(&t1);
    rtt_add_ticks(60LU * 60 * RTT_FREQUENCY);
    t1.tm_hour++;
    rtc_get_time(&now);
    TEST_ASSERT_EQUAL_INT(t1.tm_sec, now.tm_sec);
    TEST_ASSERT_EQUAL_INT(t1.tm_min, now.tm_min);
    TEST_ASSERT_EQUAL_INT(0, rtc_tm_compare(&t1, &now));

    TEST_ASSERT_EQUAL_INT(2, alarm.tm_isdst);

    rtt_add_ticks(60LU * 60 * RTT_FREQUENCY);
    TEST_ASSERT_EQUAL_INT(2, alarm.tm_isdst);
}

static void test_rtt_rtc_settimeofday(void)
{
    uint32_t s = 10, sec;
    uint32_t us = US_PER_SEC / 8, micro_sec;

    rtt_rtc_settimeofday(s, us);
    rtt_rtc_gettimeofday(&sec, &micro_sec);

    TEST_ASSERT_EQUAL_INT(s, sec);
    TEST_ASSERT_EQUAL_INT(us, micro_sec);

    rtt_add_ticks(1LU * RTT_FREQUENCY);
    s++;
    rtt_rtc_gettimeofday(&sec, &micro_sec);
    TEST_ASSERT_EQUAL_INT(s, sec);

    rtt_add_ticks(RTT_FREQUENCY / 4);
    us += US_PER_SEC / 4;
    rtt_rtc_gettimeofday(&sec, &micro_sec);
    TEST_ASSERT_EQUAL_INT(s, sec);
    TEST_ASSERT_EQUAL_INT(us, micro_sec);
}

Test *tests_rtt_rtt_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_set_time),
        new_TestFixture(test_set_alarm),
        new_TestFixture(test_set_alarm_short),
        new_TestFixture(test_set_alarm_set_time),
        new_TestFixture(test_rtt_rtc_settimeofday),
    };

    EMB_UNIT_TESTCALLER(rtt_rtc_tests, NULL, NULL, fixtures);

    return (Test *)&rtt_rtc_tests;
}

void tests_rtt_rtc(void)
{
    rtc_init();
    rtt_add_ticks(10);
    TESTS_RUN(tests_rtt_rtt_tests());
}
/** @} */
