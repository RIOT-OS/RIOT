/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
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
 * @brief       Test application for the MH-Z19 sensor driver
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#ifndef TEST_UART
#error "TEST_UART not defined"
#endif

#include <stdio.h>

#include "xtimer.h"

#include "periph/uart.h"

#include "mhz19.h"

int main(void)
{
    mhz19_t dev;
    mhz19_params_t params = {
        .uart = TEST_UART,
    };

    puts("MH-Z19 CO2 sensor test application\n");

    /* initialize the sensor */
    printf("Initializing sensor...");

    if (mhz19_init(&dev, &params) == 0) {
        puts("[OK]");
    }
    else {
        puts("[Failed]");
        return 1;
    }

    /* read CO2 level every 1 seconds */
    int16_t ppm;
    while (1) {
        printf("Testing sensor communication...");
        int res = mhz19_get_ppm(&dev, &ppm);
        if (res == 0) {
            puts("[OK]");
        }
        else {
            printf("[Failed]: %d\n", res);
        }

        /* display results */
        printf("CO2: %d ppm\n", ppm);

        /* sleep between measurements */
        xtimer_usleep(1000 * US_PER_MS);
    }

    return 0;
}
