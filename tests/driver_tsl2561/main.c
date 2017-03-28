/*
 * Copyright (C) 2016 Inria
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
 * @brief       Test application for the TSL2561 Lux sensor
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#ifndef TEST_I2C
#error "TEST_I2C not defined"
#endif

#ifndef TEST_ADDR
#error "TEST_ADDR not defined"
#endif

#ifndef TEST_GAIN
#error "TEST_GAIN not defined"
#endif

#ifndef TEST_INTEGRATION_TIME
#error "TEST_INTEGRATION_TIME not defined"
#endif

#include <stdio.h>
#include <inttypes.h>

#include "tsl2561.h"
#include "xtimer.h"
#include "board.h"

#define SLEEP_1S   (1 * 1000 * 1000u) /* 1 second delay between printf */

int main(void)
{
    puts("TSL2561 test application\n");

    tsl2561_t dev;

    printf("+------------Initializing------------+\n");

    switch(tsl2561_init(&dev, TEST_I2C, TEST_ADDR,
                        TEST_GAIN, TEST_INTEGRATION_TIME)) {
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

        xtimer_usleep(SLEEP_1S);
    }

    return 0;
}
