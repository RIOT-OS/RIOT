/*
 * Copyright (C) 2020 Deutsches Zentrum für Luft- und Raumfahrt e.V. (DLR)
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
 * @brief       Test application for the HSC pressure and temperature sensor
 *
 * @author      Quang Pham <phhr_quang@live.com>
 *
 * @}
 */

#include <stdlib.h>
#include <stdio.h>

#include "errno.h"
#include "string.h"
#include "hsc.h"
#include "hsc_params.h"
#include "xtimer.h"

#define SLEEP_USEC  (1UL * US_PER_SEC)

int main(void)
{
    hsc_t dev;
    int retval;

    xtimer_usleep(SLEEP_USEC);

    puts("HSC test application\n");

    printf("+------------Initializing------------+\n");
    do {
        retval = hsc_init(&dev, &hsc_params[0]);
        if (retval < 0) {
            printf("Initialization error with code: %s\n", strerror(-retval));
        }
        xtimer_usleep(SLEEP_USEC);
    } while (retval);

    puts("Initialization successful\n");

    printf("+--------Starting Measurements--------+\n");

    while (1) {
        /* Get temperature in degrees celsius */
        int16_t temperature;
        if ((retval = hsc_read_temperature(&dev, &temperature)) != 0) {
            printf("Get temp fail with code %d\n", retval);
        }

        /* Get pressure in ubar */
        int32_t pressure;
        if ((retval = hsc_read_pressure(&dev, &pressure)) != 0) {
            printf("Get press fail with code %d\n", retval);
        }

        printf("Measured pressure is %" PRId32 "ubar\n", pressure);
        printf("    Measured temp is %d.%d °C\n", temperature/10, temperature%10);

        xtimer_usleep(SLEEP_USEC);
    }

    return 0;
}
