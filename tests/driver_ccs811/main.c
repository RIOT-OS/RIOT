/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @brief       Test application for the AMS CCS811 device driver
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 *
 * The test application demonstrates the use of the CCS811 using
 *
 * - data-ready status function ```ccs811_data_ready``` to wait for
 *   new data and
 * - default configuration parameters, that is, the measurement mode
 *   ```CCS811_MODE_1S``` with one measurement per second.
 *
 * Please refer ```$(RIOTBASE)/tests/driver_ccs811_full``` to learn how
 * to use the CCS811 with interrupts.
 */

#include <stdio.h>
#include <string.h>

#include "thread.h"
#include "xtimer.h"

#include "ccs811.h"
#include "ccs811_params.h"

int main(void)
{
    ccs811_t sensor;

    puts("CCS811 test application\n");

    printf("+------------Initializing------------+\n");

    /* initialize the sensor with default configuration parameters */
    if (ccs811_init (&sensor, &ccs811_params[0]) != CCS811_OK) {
        puts("Initialization failed\n");
        return 1;
    }

    printf("\n+--------Starting Measurements--------+\n");

    while (1) {
        uint16_t tvoc;
        uint16_t eco2;

        /* wait and check for for new data every 10 ms */
        while (ccs811_data_ready (&sensor) != CCS811_OK) {
            xtimer_usleep(10000);
        }

        /* read the data and print them on success */
        if (ccs811_read_iaq(&sensor, &tvoc, &eco2, 0, 0) == CCS811_OK) {
            printf("TVOC [ppb]: %d\neCO2 [ppm]: %d\n", tvoc, eco2);
            puts("+-------------------------------------+");
        }
        else {
            printf("Could not read data from sensor\n");
        }
    }

    return 0;
}
