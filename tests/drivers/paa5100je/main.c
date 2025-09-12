/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the PAA5100JE/PMW3901 optical flow sensor driver
 *
 * @author      Leonard Herbst <leonard.herbst@tu-dresden.de>
 *
 * @}
 */

#include <stdio.h>

#include "paa5100je.h"
#include "paa5100je_params.h"
#include "ztimer.h"

static paa5100je_t dev;

int main(void)
{
    puts("PAA5100JE/PMW3901 Optical Flow Sensor Test Application...");
    int ret = paa5100je_init(&dev, &paa5100je_params[0]);
    if (ret == 0) {
        puts("[OK]");
    }
    else if (ret == -ENXIO) {
        puts("Error: Invalid SPI device!");
        return -1;
    }
    else if (ret == -ENODEV) {
        puts("Error: Invalid SPI CS pin/line or wrong device id or revision!");
        return -1;
    }
    else {
        printf("Error: Unknown error during initialization! (%d)\n", ret);
        return -1;
    }

    puts("Setting LED brightness to maximum.");
    if (paa5100je_set_led_brightness(&dev, PAA5100JE_LED_MAX) != 0) {
        puts("Error: Could not set LED brightness!");
        return -1;
    }

    puts("Printing sensor state every second.");

    int16_t x = 0;
    int16_t y = 0;
    int16_t delta_x = 0;
    int16_t delta_y = 0;

    do {
        ret = paa5100je_get_motion_burst(&dev, &delta_x, &delta_y);
        if (ret == 0) {
            x += delta_x;
            y += delta_y;
            printf("Relative Motion  : x:%d, y:%d\n", delta_x, delta_y);
            printf("Absolute Position: x=%d, y=%d\n", x, y);
        }
        ztimer_sleep(ZTIMER_MSEC, 250);
    } while (1);

    return 0;
}
