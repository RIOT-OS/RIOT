/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for the DS1307 RTC driver
 *
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "ds1307.h"
#include "ds1307_params.h"
#include "embUnit.h"
#include "embUnit/embUnit.h"
#include "timex.h"
#include "xtimer.h"

#define TEST_STRING "This is a test"

static ds1307_t dev;

static struct tm init = {  /* Wed Sep 22 15:10:42 2010 is the author date of
                            * RIOT's initial commit ;-) */
    .tm_sec = 42,
    .tm_min = 10,
    .tm_hour = 15,
    .tm_wday = 3,
    .tm_mday = 22,
    .tm_mon = 8,
    .tm_year = 110
};

static int _tm_cmp(struct tm *a, struct tm *b)
{
    if (a->tm_year == b->tm_year) {
        if (a->tm_mon == b->tm_mon) {
            if (a->tm_mday == b->tm_mday) {
                /* ignoring week day */
                if (a->tm_hour == b->tm_hour) {
                    if (a->tm_min == b->tm_min) {
                        return a->tm_sec - b->tm_sec;
                    }
                    return a->tm_min - b->tm_min;
                }
                return a->tm_hour - b->tm_hour;
            }
            return a->tm_mday - b->tm_mday;
        }
        return a->tm_mon - b->tm_mon;
    }
    return a->tm_year - b->tm_year;
}

static void set_up(void)
{
    ds1307_set_time(&dev, &init);
}

static void test_nvram(void)
{
    struct tm time;
    uint8_t buf[DS1307_NVRAM_MAX_SIZE] = { 0 };

    ds1307_halt(&dev);
    TEST_ASSERT_NOT_NULL(dev.nvram.read);
    TEST_ASSERT_NOT_NULL(dev.nvram.write);
    TEST_ASSERT(dev.nvram.read(&dev.nvram, buf, 0, DS1307_NVRAM_MAX_SIZE + 1) < 0);
    TEST_ASSERT(dev.nvram.write(&dev.nvram, buf, 0, DS1307_NVRAM_MAX_SIZE + 1) < 0);
    TEST_ASSERT(dev.nvram.read(&dev.nvram, buf, 1, DS1307_NVRAM_MAX_SIZE) < 0);
    TEST_ASSERT(dev.nvram.write(&dev.nvram, buf, 1, DS1307_NVRAM_MAX_SIZE) < 0);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING),
                          dev.nvram.write(&dev.nvram, (uint8_t *)TEST_STRING, 0,
                                          sizeof(TEST_STRING)));
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING),
                          dev.nvram.read(&dev.nvram, buf, 0,
                                          sizeof(TEST_STRING)));
    TEST_ASSERT_EQUAL_STRING(TEST_STRING, (char *)buf);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING) - 1,
                          dev.nvram.read(&dev.nvram, buf, 5,
                                          sizeof(TEST_STRING) - 1));
    TEST_ASSERT_EQUAL_STRING("is a test", (char *)buf);
    ds1307_get_time(&dev, &time);
    TEST_ASSERT_EQUAL_INT(0, _tm_cmp(&init, &time));
}

static void test_get_time(void)
{
    for (int i = 0; i < 5; i++) {
        struct tm time;

        xtimer_sleep(1);
        ds1307_get_time(&dev, &time);
        TEST_ASSERT(_tm_cmp(&init, &time) <= 0);
    }
}

static void test_halt(void)
{
    ds1307_halt(&dev);
    for (int i = 0; i < 3; i++) {
        struct tm time;

        xtimer_sleep(1);
        ds1307_get_time(&dev, &time);
        TEST_ASSERT_EQUAL_INT(0, _tm_cmp(&init, &time));
    }
}

static Test *tests_ds1307(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_nvram),
        new_TestFixture(test_get_time),
        /* set tested in set_up */
        new_TestFixture(test_halt),
    };

    EMB_UNIT_TESTCALLER(tests, set_up, NULL, fixtures);
    return (Test *)&tests;
}

int main(void)
{
    int res;

    puts("DS1307 RTC test\n");

    /* initialize the device */
    res = ds1307_init(&dev, (&ds1307_params[0]));
    if (res != 0) {
        puts("error: unable to initialize RTC [I2C initialization error]");
        return 1;
    }

    TESTS_START();
    TESTS_RUN(tests_ds1307());
    TESTS_END();

    return 0;
}
