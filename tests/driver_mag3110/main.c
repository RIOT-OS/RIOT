/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
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
 * @brief       Test application for the MPL3115A2 magnetometer driver.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Johann Fischer <j.fischer@phytec.de>
 *
 * @}
 */

#ifndef TEST_MAG3110_I2C
#error "TEST_MAG3110_I2C not defined"
#endif
#ifndef TEST_MAG3110_ADDR
#error "TEST_MAG3110_ADDR not defined"
#endif

#include <stdio.h>

#include "xtimer.h"
#include "mag3110.h"

#define SLEEP       (1000 * 1000U)

int main(void)
{
    mag3110_t dev;
    int8_t temp;
    int16_t x, y, z;
    uint8_t status;

    puts("MAG3110 magnetometer driver test application\n");
    printf("Initializing MAG3110 magnetometer at I2C_%i... ", TEST_MAG3110_I2C);
    if (mag3110_init(&dev, TEST_MAG3110_I2C, TEST_MAG3110_ADDR, MAG3110_DROS_DEFAULT) == 0) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return -1;
    }

    if (mag3110_set_user_offset(&dev, TEST_MAG3110_USER_OFFSET_X,
                                     TEST_MAG3110_USER_OFFSET_Y,
                                     TEST_MAG3110_USER_OFFSET_Z )) {
        puts("Set user offset correction failed.");
        return -1;
    }

    if (mag3110_set_active(&dev)) {
        puts("Measurement start failed.");
        return -1;
    }

    while (1) {
        xtimer_usleep(SLEEP);
        mag3110_read(&dev, &x, &y, &z, &status);
        printf("Field strength: X: %d Y: %d Z: %d S: %2x\n", x, y, z, status);
        mag3110_read_dtemp(&dev, &temp);
        printf("Die Temperature T: %d\n", temp);
    }

    return 0;
}
