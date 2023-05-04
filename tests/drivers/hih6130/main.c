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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "xtimer.h"
#include "hih6130.h"

#define SLEEP_USEC  (100 * 1000U)

int main(void)
{
    hih6130_t dev;

    puts("HIH6130 sensor driver test application\n");

    printf("Initializing HIH6130 sensor at I2C_%i, address 0x%02x... ",
        TEST_HIH6130_I2C, TEST_HIH6130_ADDR);
    hih6130_init(&dev, TEST_HIH6130_I2C, TEST_HIH6130_ADDR);
    puts("[OK]");

    while (1) {
        float hum = 0.f;
        float temp = 0.f;
        int status;
        float integral = 0.f;
        float fractional;

        xtimer_usleep(SLEEP_USEC);

        status = hih6130_get_humidity_temperature_float(&dev, &hum, &temp);
        if (status < 0) {
            printf("Communication error: %d\n", status);
            continue;
        } else if (status == 1) {
            puts("Stale values");
        }
        /* Several platforms usually build with nano.specs, (without float printf) */
        /* Split value into two integer parts for printing. */
        fractional = modff(hum, &integral);
        printf("humidity: %4d.%04u %%",
            (int)integral, (unsigned int)abs((int)(fractional * 10000.f)));
        fractional = modff(temp, &integral);
        printf("  temperature: %4d.%04u C\n",
            (int)integral, (unsigned int)abs((int)(fractional * 10000.f)));
    }

    return 0;
}
