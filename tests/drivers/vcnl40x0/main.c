/*
 * SPDX-FileCopyrightText: 2017 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the VNCL40X0 proximity and ambient light sensor.
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "vcnl40x0.h"
#include "vcnl40x0_params.h"
#include "xtimer.h"
#include "board.h"

#define SLEEP_2S   (2U) /* 2 seconds delay between printf */

int main(void)
{
    vcnl40x0_t dev;
    int result;

    puts("VCNL40X0 test application\n");

    printf("+------------Initializing------------+\n");
    result = vcnl40x0_init(&dev, &vcnl40x0_params[0]);
    if (result == -VCNL40X0_ERR_I2C) {
        puts("[Error] The given i2c is not enabled");
        return 1;
    }
    else if (result == -VCNL40X0_ERR_NODEV) {
        puts("[Error] The sensor did not answer correctly on the given address");
        return 1;
    }
    else {
        printf("Initialization successful\n\n");
    }

    printf("\n+--------Starting Measurements--------+\n");
    while (1) {
        printf("Proximity [cts]: %d\n"
               "Ambient light [cts]: %d\n"
               "Illuminance [lx]: %d\n"
               "\n+-------------------------------------+\n",
               vcnl40x0_read_proximity(&dev),
               vcnl40x0_read_ambient_light(&dev),
               vcnl40x0_read_illuminance(&dev));

        xtimer_sleep(SLEEP_2S);
    }

    return 0;
}
