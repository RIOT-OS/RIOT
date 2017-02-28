/*
 * Copyright (C) 2017 Inria
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
 * @brief       Test application for the VEML6070 UV sensor
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "veml6070.h"
#include "veml6070_params.h"
#include "xtimer.h"
#include "board.h"

#define SLEEP_2S   (2 * 1000 * 1000u) /* 2 seconds delay between printf */

int main(void)
{
    veml6070_t dev;
    int result;

    puts("VEML6070 test application\n");

    printf("+------------Initializing------------+\n");
    result = veml6070_init(&dev, &veml6070_params[0]);
    if (result == VEML6070_ERR_I2C) {
        puts("[Error] The given i2c is not enabled");
        return 1;
    } else {
        printf("Initialization successful\n\n");
    }

    printf("\n+--------Starting Measurements--------+\n");
    while (1) {
        printf("UV indive: %d"
               "\n+-------------------------------------+\n",
               veml6070_read_uv(&dev));

        xtimer_usleep(SLEEP_2S);
    }

    return 0;
}
