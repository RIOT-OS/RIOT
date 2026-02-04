/*
 * SPDX-FileCopyrightText: 2014 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2014 PHYTEC Messtechnik GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
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

#include <stdio.h>

#include "xtimer.h"
#include "mma8x5x.h"
#include "mma8x5x_params.h"

#define SLEEP_USEC  (100 * 1000U)

static mma8x5x_t dev;

int main(void)
{
    mma8x5x_data_t data;

    puts("MMA8652 accelerometer driver test application\n");
    printf("Initializing MMA8652 accelerometer at I2C_DEV(%i)... ",
           mma8x5x_params->i2c);

    if (mma8x5x_init(&dev, mma8x5x_params) == MMA8X5X_OK) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return -1;
    }

    while (1) {
        mma8x5x_read(&dev, &data);
        printf("Acceleration [in mg]: X: %d Y: %d Z: %d\n",
                data.x, data.y, data.z);
        xtimer_usleep(SLEEP_USEC);
    }

    return 0;
}
