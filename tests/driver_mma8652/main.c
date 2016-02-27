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
 * @brief       Test application for the MMA8652 accelerometer driver.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Johann Fischer <j.fischer@phytec.de>
 *
 * @}
 */

#ifndef TEST_MMA8652_I2C
#error "TEST_MMA8652_I2C not defined"
#endif
#ifndef TEST_MMA8652_ADDR
#error "TEST_MMA8652_ADDR not defined"
#endif

#include <stdio.h>

#include "xtimer.h"
#include "mma8652.h"

#define SLEEP       (1000 * 1000U)

int main(void)
{
    mma8652_t dev;
    int16_t x, y, z;
    uint8_t status;

    puts("MMA8652 accelerometer driver test application\n");
    printf("Initializing MMA8652 accelerometer at I2C_%i... ", TEST_MMA8652_I2C);

    if (mma8652_init(&dev, TEST_MMA8652_I2C, TEST_MMA8652_ADDR,
                     MMA8652_DATARATE_DEFAULT,
                     MMA8652_FS_RANGE_DEFAULT) == 0) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return -1;
    }

    if (mma8652_set_user_offset(&dev, TEST_MMA8652_USER_OFFSET_X,
                                TEST_MMA8652_USER_OFFSET_Y,
                                TEST_MMA8652_USER_OFFSET_Z)) {
        puts("Set user offset correction failed.");
        return -1;
    }

    if (mma8652_set_active(&dev)) {
        puts("Measurement start failed.");
        return -1;
    }

    while (1) {
        mma8652_read(&dev, &x, &y, &z, &status);
        printf("Acceleration, in mg: X: %d Y: %d Z: %d S: %2x\n", x, y, z, status);
        xtimer_usleep(SLEEP);
    }

    return 0;
}
