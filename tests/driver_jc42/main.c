/*
 * Copyright (C) 2017 Koen Zandberg <koen@bergzand.net>
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
 * @brief       Test application for the jc42 sensor driver
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#ifndef TEST_I2C
#error "TEST_I2C not defined"
#endif

#ifndef TEST_I2C_ADDR
#error "TEST_I2C_ADDR not defined"
#endif

#ifndef TEST_I2C_SPEED
#error "TEST_I2C_SPEED not defined"
#endif

#include <stdio.h>

#include "xtimer.h"

#include "periph/i2c.h"

#include "jc42.h"

int main(void)
{
    jc42_t dev;
    jc42_params_t params = {
        .i2c = TEST_I2C,
        .addr = TEST_I2C_ADDR,
        .speed = TEST_I2C_SPEED,
    };

    puts("JC42 temperature sensor test application\n");

    /* initialize the sensor */
    printf("Initializing sensor...");

    if (jc42_init(&dev, &params) == 0) {
        puts("[OK]");
    }
    else {
        puts("[Failed]");
        return 1;
    }

    /* read temperature every 1 seconds */
    int16_t temperature;
    while (1) {
        printf("Testing sensor communication...");
        if (jc42_get_temperature(&dev, &temperature) == 0) {
            puts("[OK]");
        }
        else {
            puts("[Failed]");
            return 1;
        }
        /* display results */
        printf("temperature: %d.%02d C\n", temperature / 100, temperature % 100);

        /* sleep between measurements */
        xtimer_usleep(1000 * US_PER_MS);
    }

    return 0;
}
