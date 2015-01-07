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
 * @brief       Test application for the SRF08 ultrasonic range sensor
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */
#ifndef TEST_SRF08_I2C
#error "TEST_SRF08_I2C not defined"
#endif
#ifndef TEST_SRF08_SPEED
#error "TEST_SRF08_SPEED not defined"
#endif
#ifndef TEST_MODE
#error "TEST_MODE not defined"
#endif
#ifndef TEST_NUM_ECHOS
#error "TEST_NUM_ECHOS not defined"
#endif

#include <stdio.h>

#include "vtimer.h"
#include "srf08.h"
#include "periph/i2c.h"

 #define SLEEP       (1000 * 1000U)

 static srf08_t srf08_0;

int main(void)
{
    puts("SRF08 ultrasonic ranger test application\n");
    printf("Initializing SRF08 sensor at I2C_%i... ", TEST_SRF08_I2C);

    int res;
    uint16_t range_array[TEST_NUM_ECHOS];

    res = srf08_init(&srf08_0, TEST_SRF08_I2C, SRF08_DEFAULT_ADDR, TEST_SRF08_SPEED);

    if (res < 0) {
        printf("[Failed]");
        return 1;
    }

    else {
        puts("[Ok]\n");

        while(1) {

            int echo_number = srf08_get_distances(&srf08_0, range_array, TEST_NUM_ECHOS, TEST_MODE);

            if (echo_number > 0) {
                for (int i = 0; i < echo_number; i++) {
                    printf("stored distance = %i cm , echo%i\n", range_array[i], i + 1);
                }
                puts("--------------------------------------------");
            }

            else {
                puts("An error occured");
            }
            vtimer_usleep(SLEEP);
        }
    }
}
