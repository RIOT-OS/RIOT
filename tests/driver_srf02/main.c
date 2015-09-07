/*
 * Copyright (C) 2014 Hamburg University of Applied Sciences
 *               2015 Freie Universität Berlin
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
 * @brief       Test application for the SRF02 ultrasonic range sensor
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#ifndef TEST_SRF02_I2C
#error "TEST_SRF02_I2C not defined"
#endif
#ifndef TEST_MODE
#error "TEST_MODE not defined"
#endif

#include <stdio.h>

#include "xtimer.h"
#include "srf02.h"
#include "periph/i2c.h"

#define SAMPLE_PERIOD       (1000 * 1000U)

int main(void)
{
    int res;
    srf02_t dev;
    uint32_t wakeup = xtimer_now();

    puts("\nSRF02 Ultrasonic Range Sensor Test\n");
    puts("This test will sample the sensor once per second and display the\n"
         "result\n");

    printf("Initializing SRF02 sensor at I2C_%i... ", TEST_SRF02_I2C);
    res = srf02_init(&dev, TEST_SRF02_I2C, SRF02_DEFAULT_ADDR);
    if (res < 0) {
        puts("[Failed]");
        return 1;
    }
    else {
        puts("[Ok]\n");
    }

    while(1) {
        xtimer_usleep_until(&wakeup, SAMPLE_PERIOD);
        uint16_t distance = srf02_get_distance(&dev, TEST_MODE);
        printf("distance = %i cm\n", distance);
    }
}
