/*
 * SPDX-FileCopyrightText: 2016 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the TSL2561 Lux sensor
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "timex.h"
#include "ztimer.h"
#include "board.h"

#include "tsl2561.h"
#include "tsl2561_params.h"

int main(void)
{
    puts("TSL2561 test application\n");

    tsl2561_t dev;

    printf("+------------Initializing------------+\n");

    switch(tsl2561_init(&dev, &tsl2561_params[0])) {
        case TSL2561_NOI2C:
        puts("[Error] I2C not working: cannot initialize the sensor.\n");
        break;

        case TSL2561_BADDEV:
        puts("[Error] Not a TSL2561: cannot initialize the sensor.\n");
        break;

        case TSL2561_OK:
        puts("[Info] Initialization successful\n\n");
        break;
    }

    printf("\n+--------Starting Measurements--------+\n");
    while (1) {
        /* Get the illuminance in Lux */
        printf("Illuminance [lx]: %i\n"
               "\n+-------------------------------------+\n",
               (int)tsl2561_read_illuminance(&dev));

        ztimer_sleep(ZTIMER_MSEC, MS_PER_SEC); /* 1s delay */
    }

    return 0;
}
