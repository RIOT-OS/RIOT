/*
 * Copyright (C) 2018 Inria
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
 * @brief       Test application for the CCS811 gas sensor
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "ccs811.h"
#include "ccs811_params.h"
#include "xtimer.h"
#include "board.h"

int main(void)
{
    ccs811_t dev;
    int result;

    puts("CCS811 test application\n");

    printf("+------------Initializing------------+\n");
    result = ccs811_init(&dev, &ccs811_params[0]);
    if (result != 0) {
        puts("[Error] Failed to initialize the CCS811 sensor");
        return 1;
    }

    puts("Initialization successful\n"
         "\n+--------Starting Measurements--------+");

    /* Measures are acquired every seconds */
    ccs811_set_drive_mode(&dev, CCS811_DRIVE_MODE_1);

    while (1) {
        /* Read eCO2 level */
        uint16_t eco2;
        int ret = ccs811_read_eco2(&dev, &eco2);
        if (ret == 0) {
            /* Print the value */
            printf("eCO2: %ippm\n", eco2);
        }
        else {
            printf("Data not ready\n\n");
        }

        xtimer_sleep(1);

        /* Read TVOC level */
        uint16_t tvoc;
        ret = ccs811_read_tvoc(&dev, &tvoc);
        if (ret == 0) {
            /* Print the value */
            printf("TVOC: %ippb\n\n", tvoc);
        }
        else {
            printf("Data not ready\n\n");
        }

        xtimer_sleep(1);
    }

    return 0;
}
