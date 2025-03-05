/*
 * Copyright (C) 2024 Nico Behrens <nifrabe@outlook.de>
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
 * @brief       Test application for TM1637 4-digit 7-segment display driver
 *
 * @author      Nico Behrens <nifrabe@outlook.de>
 *
 * @}
 */

#include "tm1637.h"
#include "tm1637_params.h"

#include "periph/gpio.h"
#include "ztimer.h"
#include "embUnit.h"

#include <stdio.h>

/**
 * @brief device descriptor of the display
 */
static tm1637_t dev;

static void test_setup(void)
{
    tm1637_init(&dev, &tm1637_params[0]);
}

/**
 * @brief Display a number with different settings
 */
static void test_number(void)
{
    int number = -42;
    TEST_ASSERT_EQUAL_INT(0, tm1637_write_number(&dev, number, TM1637_PW_14_16, false, false));
    ztimer_sleep(ZTIMER_SEC, 1);
    TEST_ASSERT_EQUAL_INT(0, tm1637_write_number(&dev, number, TM1637_PW_14_16, false, true));
    ztimer_sleep(ZTIMER_SEC, 1);
    TEST_ASSERT_EQUAL_INT(0, tm1637_write_number(&dev, number, TM1637_PW_14_16, true, false));
    ztimer_sleep(ZTIMER_SEC, 1);
    TEST_ASSERT_EQUAL_INT(0, tm1637_write_number(&dev, number, TM1637_PW_14_16, true, true));
    ztimer_sleep(ZTIMER_SEC, 1);
    TEST_ASSERT_EQUAL_INT(0, tm1637_clear(&dev));
    ztimer_sleep(ZTIMER_SEC, 1);
}

/**
 * @brief Shows all digits on the display
 */
static void test_all_digits(void)
{
    for (int i = 0; i < 10; i++) {
        TEST_ASSERT_EQUAL_INT(0, tm1637_write_number(&dev, i, TM1637_PW_14_16, false, false));
        ztimer_sleep(ZTIMER_SEC, 1);
    }
    TEST_ASSERT_EQUAL_INT(0, tm1637_clear(&dev));
}

/**
 * @brief Test all brightness levels
 */
static void test_brightness(void)
{
    int number = 8888;
    /* the brightness goes from 0 to 7 */
    for (int i = 0; i <= 7; i++) {
        TEST_ASSERT_EQUAL_INT(0, tm1637_write_number(&dev, number, i, false, false));
        ztimer_sleep(ZTIMER_SEC, 1);
    }
    TEST_ASSERT_EQUAL_INT(0, tm1637_clear(&dev));
    ztimer_sleep(ZTIMER_SEC, 1);
}

static Test *tests_tm1637_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures){
        new_TestFixture(test_number),
        new_TestFixture(test_all_digits),
        new_TestFixture(test_brightness),
    };

    EMB_UNIT_TESTCALLER(tm1637_tests, test_setup, NULL, fixtures);

    return (Test *)&tm1637_tests;
}

int main(void)
{
    puts("Starting TM1637 driver test application");

    TESTS_START();
    TESTS_RUN(tests_tm1637_tests());
    TESTS_END();

    puts("Ending TM1637 driver test application");

    return 0;
}
