/*
 * SPDX-FileCopyrightText: 2017 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the MMA7660 3 axis accelerometer driver.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>

#include "xtimer.h"
#include "mma7660.h"
#include "mma7660_params.h"

#define SLEEP_USEC  (100 * US_PER_MS)

static mma7660_t dev;

int main(void)
{
    puts("MMA7660 accelerometer driver test application\n");
    printf("Initializing MMA7660 accelerometer at I2C_DEV(%i)... ",
           mma7660_params->i2c);

    if (mma7660_init(&dev, &mma7660_params[0]) == MMA7660_OK) {
        puts("[OK]");
    }
    else {
        puts("[Failed]");
        return -1;
    }

    while (1) {
        mma7660_data_t data;
        mma7660_read(&dev, &data);
        printf("Acceleration [in mg]: X: %d Y: %d Z: %d\n",
               data.x, data.y, data.z);
        xtimer_usleep(SLEEP_USEC);
    }

    return 0;
}
