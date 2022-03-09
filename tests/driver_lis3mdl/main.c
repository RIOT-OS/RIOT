
/*
 * Copyright (C) 2015 HAW Hamburg
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
 * @brief       Test application for the LIS3MDL 3-axis magnetometer
 *
 * @author      René Herthel <rene-herthel@outlook.de>
 *
 * @}
 */

#include <stdio.h>

#include "timex.h"
#include "ztimer.h"
#include "lis3mdl.h"
#include "lis3mdl_params.h"

#define SLEEP_MS    (640U)

int main(void)
{
    lis3mdl_t dev;

    puts("LIS3MDL test application");
    puts("Initializing LIS3MDL sensor");

    if (lis3mdl_init(&dev, &lis3mdl_params[0]) == 0) {
        puts("[ OK ]\n");
    }
    else {
        puts("[ FAIL ]\n");
        return 1;
    }

    while(1) {
        lis3mdl_3d_data_t mag_value;
        lis3mdl_read_mag(&dev, &mag_value);
        printf("Magnetometer [G]:\tX: %2d\tY: %2d\tZ: %2d\n",
               mag_value.x_axis,
               mag_value.y_axis,
               mag_value.z_axis);

        int16_t temp_value;
        lis3mdl_read_temp(&dev, &temp_value);
        printf("Temperature:\t\t%i°C\n", temp_value);

        ztimer_sleep(ZTIMER_MSEC, SLEEP_MS);
    }

    return 0;
}
