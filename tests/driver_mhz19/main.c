/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 * Copyright (C) 2018 Beduino Master Projekt - University of Bremen
 * Copyright (C) 2020 Bas Stottelaar <basstottelaar@gmail.com>
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
 * @author      Christian Manal <manal@uni-bremen.de>
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#define TEST_MODE_UART 1
#define TEST_MODE_PWM 2

#ifndef TEST_MODE
#error "TEST_MODE not defined"
#endif

#include <stdio.h>
#include "ztimer.h"
#include "mhz19.h"
#include "mhz19_params.h"

int main(void)
{
    mhz19_t dev;

#if TEST_MODE == TEST_MODE_UART
    mhz19_params_t params = { .uart = TEST_UART };
#endif
#if TEST_MODE == TEST_MODE_PWM
    mhz19_params_t params = { .pin = TEST_PWM };
#endif

    puts("MH-Z19 CO2 sensor test application\n");

    /* initialize the sensor */
#if TEST_MODE == TEST_MODE_UART
    printf("Initializing sensor in UART mode...");
#endif
#if TEST_MODE == TEST_MODE_PWM
    printf("Initializing sensor in PWM mode...");
#endif

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
        ztimer_sleep(ZTIMER_MSEC, 1000);
    }

    return 0;
}
