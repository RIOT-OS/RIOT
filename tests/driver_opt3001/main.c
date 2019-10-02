/*
 * Copyright (C) 2019 HAW Hamburg
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
 * @brief       Test application for the OPT3001 sensor driver.
 *
 * @author      Jannes Volkens <jannes.volkens@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "opt3001_params.h"
#include "opt3001.h"
#include "xtimer.h"

#define OPT3001_TEST_TIME 1000000

int main(void){

    opt3001_t dev;
    uint32_t convlux;

    puts("OPT3001 test application\n");
    printf("Initializing OPT3001 sensor at I2C_%i ... ", opt3001_params[0].i2c_dev);

    if (opt3001_init(&dev, &opt3001_params[0]) != OPT3001_OK) {
        puts("init device [ERROR]");
        return -1;
    }

    if (opt3001_set_active(&dev) != OPT3001_OK) {
        puts("set active [ERROR]");
        return -1;
    }

    puts("[SUCCESS]\n");

    printf("\n+--------Starting Measurements--------+\n");

    while(1){
        opt3001_read_lux(&dev, &convlux);

        printf("MilliLux: %" PRIu32 "\n", convlux);
        printf("\n+-------------------------------------+\n");

        xtimer_usleep(OPT3001_TEST_TIME);
    }

    return 0;
}
