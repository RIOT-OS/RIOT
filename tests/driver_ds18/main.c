/*
 * Copyright (C) 2018 HAW Hamburg
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
#include "xtimer.h"

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

        xtimer_sleep(SAMPLING_PERIOD);
    }

    return 0;
}
