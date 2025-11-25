/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the HIH6130 sensor driver
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#ifndef TEST_HIH6130_I2C
#error "TEST_HIH6130_I2C not defined"
#endif
#ifndef TEST_HIH6130_ADDR
#error "TEST_HIH6130_ADDR not defined"
#endif

#include "fmt.h"
#include "hih6130.h"
#include "ztimer.h"

#define SLEEP_MSEC  100

int main(void)
{
    hih6130_t dev;

    print_str("HIH6130 sensor driver test application\n");

    print_str("Initializing HIH6130 sensor at I2C_");
    print_u32_dec(TEST_HIH6130_I2C);
    print_str(", address 0x");
    print_u32_hex(TEST_HIH6130_ADDR);
    print_str("...\n");
    hih6130_init(&dev, TEST_HIH6130_I2C, TEST_HIH6130_ADDR);
    print_str("[OK]\n");

    while (1) {
        int32_t hum, temp;
        int status;

        ztimer_sleep(ZTIMER_MSEC, SLEEP_MSEC);

        status = hih6130_get_humidity_temperature(&dev, &hum, &temp);
        if (status < 0) {
            print_str("Communication error: ");
            print_s32_dec(status);
            print_str("\n");
            continue;
        }
        else if (status == 1) {
            print_str("Stale values\n");
        }

        print_str("humidity: ");
        char buf[20];
        print(buf, fmt_s32_dfp(buf, hum, -3));
        print_str(" %, temperature: ");
        print(buf, fmt_s32_dfp(buf, temp, -3));
        print_str(" °C\n");
    }

    return 0;
}
