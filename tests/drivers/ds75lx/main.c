/*
 * SPDX-FileCopyrightText: 2019 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the DS75LX temperature sensor
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "ds75lx.h"
#include "ds75lx_params.h"
#include "xtimer.h"

int main(void)
{
    ds75lx_t dev;
    int result;

    puts("DS75LX test application\n");

    printf("+------------Initializing------------+\n");
    result = ds75lx_init(&dev, &ds75lx_params[0]);
    if (result != DS75LX_OK) {
        puts("[Error] Failed to initialize DS75LX sensor");
        return 1;
    }

    puts("Initialization successful\n");

    printf("\n+--------Starting Measurements--------+\n");
    int16_t temperature;
    while (1) {
        ds75lx_wakeup(&dev);
        /* Get temperature in degrees celsius */
        ds75lx_read_temperature(&dev, &temperature);
        ds75lx_shutdown(&dev);

        bool negative = (temperature < 0);
        if (negative) {
            temperature = -temperature;
        }

        printf("Temperature [°C]: %c%d.%02d\n",
               negative ? '-': ' ',
               (int)(temperature / 100),
               (int)(temperature % 100));

        xtimer_sleep(2);
    }

    return 0;
}
