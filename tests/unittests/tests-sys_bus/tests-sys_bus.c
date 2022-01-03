/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
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

#include "embUnit.h"

#include "periph/rtc.h"

extern int32_t sys_bus_rtc_compute_diff_ms(
        const struct tm *tm_before, uint32_t us_before,
        const struct tm *tm_after, uint32_t us_after);

static void test_sys_bus_rtc_compute_diff_ms(void)
{
    /* zero */
    struct tm tm_before;
    struct tm tm_after;
    rtc_localtime(1000, &tm_before);
    rtc_localtime(1000, &tm_after);
    TEST_ASSERT_EQUAL_INT(0, sys_bus_rtc_compute_diff_ms(
                    &tm_before, 0, &tm_after, 0));
    rtc_localtime(0, &tm_before);
    rtc_localtime(0, &tm_after);
    TEST_ASSERT_EQUAL_INT(0, sys_bus_rtc_compute_diff_ms(
                    &tm_before, 1234, &tm_after, 1234));

    /* positive, within range */
    rtc_localtime(100, &tm_before);
    rtc_localtime(10000, &tm_after);
    TEST_ASSERT_EQUAL_INT(9900000, sys_bus_rtc_compute_diff_ms(
                    &tm_before, 1234, &tm_after, 1234));
    TEST_ASSERT_EQUAL_INT(9899999, sys_bus_rtc_compute_diff_ms(
                    &tm_before, 1234, &tm_after, 0));
    TEST_ASSERT_EQUAL_INT(9900001, sys_bus_rtc_compute_diff_ms(
                    &tm_before, 0, &tm_after, 1234));
    TEST_ASSERT_EQUAL_INT(9900123, sys_bus_rtc_compute_diff_ms(
                    &tm_before, 0, &tm_after, 123400));
    rtc_localtime((INT32_MAX / 1000) + 99, &tm_after);
    TEST_ASSERT_EQUAL_INT(2147482123, sys_bus_rtc_compute_diff_ms(
                    &tm_before, 0, &tm_after, 123400));

    /* negative, within range */
    rtc_localtime(10000, &tm_before);
    rtc_localtime(100, &tm_after);
    TEST_ASSERT_EQUAL_INT(-9900000, sys_bus_rtc_compute_diff_ms(
                    &tm_before, 1234, &tm_after, 1234));
    TEST_ASSERT_EQUAL_INT(-9899999, sys_bus_rtc_compute_diff_ms(
                    &tm_before, 0, &tm_after, 1234));
    TEST_ASSERT_EQUAL_INT(-9900001, sys_bus_rtc_compute_diff_ms(
                    &tm_before, 1234, &tm_after, 0));
    TEST_ASSERT_EQUAL_INT(-9900123, sys_bus_rtc_compute_diff_ms(
                    &tm_before, 123400, &tm_after, 0));
    rtc_localtime((INT32_MAX / 1000) + 99, &tm_before);
    TEST_ASSERT_EQUAL_INT(-2147482123, sys_bus_rtc_compute_diff_ms(
                    &tm_before, 123400, &tm_after, 0));

    /* positive, out of range */
    rtc_localtime(100, &tm_before);
    rtc_localtime((INT32_MAX / 1000) + 100, &tm_after);
    TEST_ASSERT_EQUAL_INT(RTC_EVENT_OUT_OF_RANGE, sys_bus_rtc_compute_diff_ms(
                    &tm_before, 0, &tm_after, 123400));
    rtc_localtime(0, &tm_before);
    TEST_ASSERT_EQUAL_INT(RTC_EVENT_OUT_OF_RANGE, sys_bus_rtc_compute_diff_ms(
                    &tm_before, 0, &tm_after, 123400));

    /* negative, out of range */
    rtc_localtime((INT32_MAX / 1000) + 100, &tm_before);
    rtc_localtime(100, &tm_after);
    TEST_ASSERT_EQUAL_INT(RTC_EVENT_OUT_OF_RANGE, sys_bus_rtc_compute_diff_ms(
                    &tm_before, 123400, &tm_after, 0));
    rtc_localtime(0, &tm_after);
    TEST_ASSERT_EQUAL_INT(RTC_EVENT_OUT_OF_RANGE, sys_bus_rtc_compute_diff_ms(
                    &tm_before, 123400, &tm_after, 0));
}

Test *tests_sys_bus_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_sys_bus_rtc_compute_diff_ms),
    };

    EMB_UNIT_TESTCALLER(sys_bus_tests, NULL, NULL, fixtures);

    return (Test *)&sys_bus_tests;
}

void tests_sys_bus(void)
{
    TESTS_RUN(tests_sys_bus_tests());
}
/** @} */
