/*
 * Copyright (C) 2016 Inria
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
 * @brief       Test application for the TSL2561 Lux sensor
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "xtimer.h"
#include "periph/i2c.h"

#include "tsl4531x.h"

#define _100ms_in_us   (100 * 1000u) /* 1 second delay between printf */

int main(void)
{
    tsl4531x_t dev;
    int err;

    puts("TSL4531x test application. Initializing...");

    if((err = tsl4531x_init(&dev, TSL4531_I2C_PORT, TSL4531x_INTEGRATE_200ms)) < 0) {
        printf("Error setting up device. %d (%s)\n", err, strerror(err));
        return 1;
    }

    puts("Initialized, will begin measurements.");
    while (1) {
        int err;
        uint16_t lux;

        if ((err = tsl4531x_read(&dev, &lux)) < 0) {
            printf("Error reading from device. %d (%s)\n", err, strerror(err));
        } else {
            printf("Illuminance [lx]: %u\n", lux);
        }

        xtimer_usleep(_100ms_in_us);
    }

    return 0;
}
