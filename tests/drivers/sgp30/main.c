/*
 * Copyright (C) 2021 Inria
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
 * @brief       SGP30 driver test application
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include <stdio.h>

#include "sgp30.h"
#include "sgp30_params.h"

#include "ztimer.h"

int main(void)
{
    sgp30_t sensor;

    puts("SGP30 test application\n");

    printf("+------------Initializing------------+\n");

    /* initialize the sensor with default configuration parameters */
    if (sgp30_init(&sensor, &sgp30_params[0]) != 0) {
        puts("Initialization failed\n");
        return 1;
    }

    printf("\n+--------Starting Measurements--------+\n");

    while (1) {
        sgp30_data_t data;

        /* read the data and print them on success */
        int ret = sgp30_read_measurements(&sensor, &data);
        if (ret == 0) {
            printf("TVOC [ppb]: %d\neCO2 [ppm]: %d\n", data.tvoc, data.eco2);
            puts("+-------------------------------------+");
        }
        else if (ret == -EAGAIN) {
            printf("Device is not yet ready\n");
        }
        else {
            printf("Could not read data from sensor\n");
        }
        ztimer_sleep(ZTIMER_USEC, 100000LU);
    }

    return 0;
}
