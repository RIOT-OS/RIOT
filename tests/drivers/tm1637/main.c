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

#include <stdio.h>

/**
 * @brief Display a number with different settings
 *
 * @param[in] dev device descriptor of the display
 * @param[in] number number to display
 */
static void test_number(const tm1637_t *dev, int16_t number)
{
    int error = 0;
    error += tm1637_write_number(dev, number, TM1637_PW_14_16, false, false);
    ztimer_sleep(ZTIMER_SEC, 1);
    error += tm1637_write_number(dev, number, TM1637_PW_14_16, false, true);
    ztimer_sleep(ZTIMER_SEC, 1);
    error += tm1637_write_number(dev, number, TM1637_PW_14_16, true, false);
    ztimer_sleep(ZTIMER_SEC, 1);
    error += tm1637_write_number(dev, number, TM1637_PW_14_16, true, true);
    ztimer_sleep(ZTIMER_SEC, 1);
    error += tm1637_clear(dev);
    ztimer_sleep(ZTIMER_SEC, 1);

    if (error) {
        puts("Number test failed");
    }
}

/**
 * @brief Shows all digits on the display
 *
 * @param[in] dev device descriptor of the display
 */
static void test_all_digits(const tm1637_t *dev)
{
    int error = 0;
    for (int i = 0; i < 10; i++) {
        error += tm1637_write_number(dev, i, TM1637_PW_14_16, false, false);
        ztimer_sleep(ZTIMER_SEC, 1);
    }
    error += tm1637_clear(dev);

    if (error) {
        puts("All digits test failed");
    }
}

/**
 * @brief Test all brightness levels
 *
 * @param[in] dev device descriptor of the display
 * @param[in] number number to display
 */
static void test_brightness(const tm1637_t *dev, uint16_t number)
{
    /* the brightness goes from 0 to 7 */
    int error = 0;

    for (int i = 0; i <= 7; i++) {
        error += tm1637_write_number(dev, number, i, false, false);
        ztimer_sleep(ZTIMER_SEC, 1);
    }
    error += tm1637_clear(dev);
    ztimer_sleep(ZTIMER_SEC, 1);

    if (error) {
        puts("Brightness test failed");
    }
}

int main(void)
{
    puts("Starting TM1637 driver test application");

    tm1637_t dev;
    tm1637_init(&dev, &tm1637_params[0]);

    test_number(&dev, 42);
    test_all_digits(&dev);
    test_brightness(&dev, 1111);

    puts("Ending TM1637 driver test application");

    return 0;
}
