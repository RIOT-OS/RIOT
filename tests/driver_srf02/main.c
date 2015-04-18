/*
 * Copyright (C) 2014 Hamburg University of Applied Sciences
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
 * @brief       Test application for the SRF02 ultrasonic range sensor
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * @}
 */
#ifndef TEST_SRF02_I2C
#error "TEST_SRF02_I2C not defined"
#endif
#ifndef TEST_SRF02_SPEED
#error "TEST_SRF02_SPEED not defined"
#endif
#ifndef TEST_MODE
#error "TEST_MODE not defined"
#endif

#include <stdio.h>

#include "vtimer.h"
#include "srf02.h"
#include "periph/i2c.h"

 #define SLEEP       (1000 * 1000U)

 static srf02_t srf02_0;

int main(void)
{
    int res;
    puts("SRF02 ultrasonic ranger test application\n");
    printf("Initializing SRF02 sensor at I2C_%i... ", TEST_SRF02_I2C);
    res = srf02_init(&srf02_0, TEST_SRF02_I2C, SRF02_DEFAULT_ADDR, TEST_SRF02_SPEED);

    if (res < 0) {
        printf("[Failed]");
        return 1;
    }
    else {
        puts("[Ok]\n");

        while(1) {
            uint16_t distance = srf02_get_distance(&srf02_0, TEST_MODE);
            printf("distance = %i cm\n", distance);
            vtimer_usleep(SLEEP);
        }
    }
}
