/*
 * SPDX-FileCopyrightText: 2014 Hamburg University of Applied Sciences
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the SRF08 ultrasonic range sensor
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 * @author      Kevin Weiss <kevin.weiss@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>

#include "xtimer.h"
#include "srf08.h"
#include "srf08_params.h"
#include "periph/i2c.h"

#ifndef TEST_MODE
#define TEST_MODE           (SRF08_MODE_CM)
#endif
#ifndef TEST_NUM_ECHOS
#define TEST_NUM_ECHOS      (3U)
#endif

#define SLEEP_USEC          (1000 * US_PER_MS)

static srf08_t srf08_0;

int main(void)
{
    puts("SRF08 ultrasonic ranger test application\n");
    printf("Initializing SRF08 sensor at I2C_%i... ", srf08_params[0].i2c);

    int res;

    res = srf08_init(&srf08_0, &srf08_params[0]);

    if (res < 0) {
        printf("[Failed]");
        return 1;
    }

    else {
        puts("[Ok]\n");

        while(1) {
            uint16_t range_array[TEST_NUM_ECHOS];
            int echo_number = srf08_get_distances(&srf08_0, range_array,
                                                  TEST_NUM_ECHOS, TEST_MODE);

            if (echo_number > 0) {
                for (int i = 0; i < echo_number; i++) {
                    printf("stored distance = %i cm , echo%i\n",
                           range_array[i], i + 1);
                }
                puts("--------------------------------------------");
            }

            else {
                puts("An error occurred");
            }
            xtimer_usleep(SLEEP_USEC);
        }
    }
}
