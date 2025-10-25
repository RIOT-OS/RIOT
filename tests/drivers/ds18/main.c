/*
 * SPDX-FileCopyrightText: 2018 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the DS18B20 1-Wire temperature sensor.
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "board.h"
#include "ds18.h"
#include "ds18_params.h"
#include "ztimer.h"

#define SAMPLING_PERIOD     2

int main(void)
{
    ds18_t dev;
    int result;

    puts("DS18B20 test application\n");

    printf("+------------Initializing------------+\n");
    result = ds18_init(&dev, &ds18_params[0]);
    if (result == DS18_ERROR) {
        puts("[Error] The sensor pin could not be initialized");
        return 1;
    }

    printf("\n+--------Starting Measurements--------+\n");
    while (1) {
        int16_t temperature;

        /* Get temperature in centidegrees celsius */
        if (ds18_get_temperature(&dev, &temperature) == DS18_OK) {
            bool negative = (temperature < 0);
            if (negative) {
                temperature = -temperature;
            }

            printf("Temperature [ºC]: %c%d.%02d"
                   "\n+-------------------------------------+\n",
                   negative ? '-': ' ',
                   temperature / 100,
                   temperature % 100);
        }
        else {
            puts("[Error] Could not read temperature");
        }

        ztimer_sleep(ZTIMER_SEC, SAMPLING_PERIOD);
    }

    return 0;
}
