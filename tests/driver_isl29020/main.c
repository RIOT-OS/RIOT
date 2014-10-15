/*
 * Copyright (C) 2014 Freie Universität Berlin
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
 * @brief       Test application for the ISL29020 light sensor
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#ifndef TEST_ISL29020_I2C
#error "TEST_ISL29020_I2C not defined"
#endif
#ifndef TEST_ISL29020_ADDR
#error "TEST_ISL29020_ADDR not defined"
#endif

#include <stdio.h>

#include "vtimer.h"
#include "isl29020.h"

#define MODE        ISL29020_MODE_AMBIENT
#define RANGE       ISL29020_RANGE_16K
#define SLEEP       (250 * 1000U)

int main(void)
{
    isl29020_t dev;
    int value;

    puts("ISL29020 light sensor test application\n");
    printf("Initializing ISL29020 sensor at I2C_%i... ", TEST_ISL29020_I2C);
    if (isl29020_init(&dev, TEST_ISL29020_I2C, TEST_ISL29020_ADDR, RANGE, MODE) == 0) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return 1;
    }

    while (1) {
        value = isl29020_read(&dev);
        printf("Light value: %5i LUX\n", value);
        vtimer_usleep(SLEEP);
    }

    return 0;
}
