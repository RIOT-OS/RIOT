/*
 * SPDX-FileCopyrightText: 2023 Silke Hofstra
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Phydat Unix timestamp tests
 *
 * @author      Silke Hofstra <silke@slxh.eu>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "phydat.h"
#include "embUnit.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifndef PRIi64
#define PRIi64 "lli"
#endif

typedef struct {
    phydat_t date;
    phydat_t time;
    int32_t offset;
    int64_t ts;
} test_t;

static test_t tests[] = {
    /* Test various ways of writing 0 */
    {
        .date = { { 1, 1, 1970 }, UNIT_DATE, 0 },
        .time = { { 0, 0, 0 }, UNIT_TIME, 0 },
        .ts = 0,
    },
    {
        .date = { { 1, 1, 1970 }, UNIT_DATE, 0 },
        .time = { { 0, 0, 2 }, UNIT_TIME, 0 },
        .offset = 7200,     /* UTC +0200 */
        .ts = 0,
    },
    {
        .date = { { 31, 12, 1969 }, UNIT_DATE, 0 },
        .time = { { 0, 0, 22 }, UNIT_TIME, 0 },
        .offset = -7200,     /* UTC -0200 */
        .ts = 0,
    },
    {
        .date = { { 1, 1, 1970 }, UNIT_DATE, 0 },
        .time = { { 3600, 60, 0 }, UNIT_TIME, 0 },
        .offset = 7200,       /* UTC +0200 */
        .ts = 0,
    },
    {
        .date = { { 31, 12, 1969 }, UNIT_DATE, 0 },
        .time = { { 3600, 120, 19 }, UNIT_TIME, 0 },
        .offset = -7200,       /* UTC -0200 */
        .ts = 0,
    },

    /* Test well-known dates */
    {
        .date = { { 28, 4, 2021 }, UNIT_DATE, 0 },
        .time = { { 0, 0, 0 }, UNIT_TIME, 0 },
        .ts = 1619568000,
    },
    {
        .date = { { 29, 2, 2020 }, UNIT_DATE, 0 },
        .time = { { 0, 0, 0 }, UNIT_TIME, 0 },
        .ts = 1582934400,
    },
    {
        .date = { { 1, 3, 2020 }, UNIT_DATE, 0 },
        .time = { { 0, 0, 0 }, UNIT_TIME, 0 },
        .ts = 1583020800,
    },

    /* Test the first of every month */
    {
        .date = { { 1, 1, 1900 }, UNIT_DATE, 0 },
        .time = { { 0, 0, 0 }, UNIT_TIME, 0 },
        .ts = -2208988800,
    },
    {
        .date = { { 1, 2, 1900 }, UNIT_DATE, 0 },
        .time = { { 0, 0, 0 }, UNIT_TIME, 0 },
        .ts = -2206310400,
    },
    {
        .date = { { 1, 3, 1900 }, UNIT_DATE, 0 },
        .time = { { 0, 0, 0 }, UNIT_TIME, 0 },
        .ts = -2203891200,
    },
    {
        .date = { { 1, 4, 1900 }, UNIT_DATE, 0 },
        .time = { { 0, 0, 0 }, UNIT_TIME, 0 },
        .ts = -2201212800,
    },
    {
        .date = { { 1, 5, 1900 }, UNIT_DATE, 0 },
        .time = { { 0, 0, 0 }, UNIT_TIME, 0 },
        .ts = -2198620800,
    },
    {
        .date = { { 1, 6, 1900 }, UNIT_DATE, 0 },
        .time = { { 0, 0, 0 }, UNIT_TIME, 0 },
        .ts = -2195942400,
    },
    {
        .date = { { 1, 7, 1900 }, UNIT_DATE, 0 },
        .time = { { 0, 0, 0 }, UNIT_TIME, 0 },
        .ts = -2193350400,
    },
    {
        .date = { { 1, 8, 1900 }, UNIT_DATE, 0 },
        .time = { { 0, 0, 0 }, UNIT_TIME, 0 },
        .ts = -2190672000,
    },
    {
        .date = { { 1, 9, 1900 }, UNIT_DATE, 0 },
        .time = { { 0, 0, 0 }, UNIT_TIME, 0 },
        .ts = -2187993600,
    },
    {
        .date = { { 1, 10, 1900 }, UNIT_DATE, 0 },
        .time = { { 0, 0, 0 }, UNIT_TIME, 0 },
        .ts = -2185401600,
    },
    {
        .date = { { 1, 11, 1900 }, UNIT_DATE, 0 },
        .time = { { 0, 0, 0 }, UNIT_TIME, 0 },
        .ts = -2182723200,
    },
    {
        .date = { { 1, 12, 1900 }, UNIT_DATE, 0 },
        .time = { { 0, 0, 0 }, UNIT_TIME, 0 },
        .ts = -2180131200,
    },

    /* Test scale correction */
    {
        .date = { { 1, 1, 197 }, UNIT_DATE, 1 },
        .time = { { 0, 0, 0 }, UNIT_TIME, 0 },
        .ts = 24364800,
    },
    {
        .date = { { 10, 10, 19700 }, UNIT_DATE, -1 },
        .time = { { 0, 0, 0 }, UNIT_TIME, 0 },
        .ts = 0,
    },
    {
        .date = { { 1, 1, 1970 }, UNIT_DATE, 0 },
        .time = { { 36, 0, 0 }, UNIT_TIME, 2 },
        .ts = 3600,
    },

    /* An invalid date that might go out of bounds on the day of the year lookup table */
    {
        .date = { { 1, 13, 1969 }, UNIT_DATE, 0 },
        .time = { { 0, 0, 0 }, UNIT_TIME, 0 },
        .ts = 0,
    },
};

void test_phydat_date_time_to_unix(void)
{
    for (size_t i = 0; i < ARRAY_SIZE(tests); i++) {
        int64_t result = phydat_date_time_to_unix(
            &(tests[i].date),  &(tests[i].time), tests[i].offset);

        int32_t offset_hours = tests[i].offset / 3600;
        int32_t offset_minutes = (tests[i].offset % 3600) / 60;

        DEBUG("Datetime: %04" PRIi16 "-%02" PRIi16 "-%02" PRIi16 "e%" PRIi16 " "
              "%02" PRIi16 ":%02" PRIi16 ":%02" PRIi16 "e%" PRIi16 " "
              "%+03" PRIi32 ":%02" PRIi32 " -> %" PRIi64 "\n",
              tests[i].date.val[2], tests[i].date.val[1], tests[i].date.val[0], tests[i].date.scale,
              tests[i].time.val[2], tests[i].time.val[1], tests[i].time.val[0], tests[i].time.scale,
              offset_hours, offset_minutes, result);

        TEST_ASSERT_EQUAL_INT(tests[i].ts, result);
    }
}

Test *tests_phydat_unix(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_phydat_date_time_to_unix),
    };
    EMB_UNIT_TESTCALLER(senml_tests, NULL, NULL, fixtures);
    return (Test *)&senml_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_phydat_unix());
    TESTS_END();
    return 0;
}
