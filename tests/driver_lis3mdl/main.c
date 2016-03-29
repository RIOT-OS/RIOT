
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
#ifndef TEST_LIS3MDL_I2C
#error "TEST_LIS3MDL_I2C not defined"
#endif
#ifndef TEST_LIS3MDL_MAG_ADDR
#error "TEST_LIS3MDL_MAG_ADDR not defined"
#endif

#include <stdio.h>

#include "xtimer.h"
#include "lis3mdl.h"

#define SLEEP       (800 * 800U)

int main(void)
{
    lis3mdl_t dev;
    lis3mdl_3d_data_t mag_value;
    int16_t temp_value = 0;

    puts("\nLIS3MDL test application");
    printf("Initializing LIS3MDL sensor at I2C_%i ... \n", TEST_LIS3MDL_I2C);

    if (lis3mdl_init(&dev, TEST_LIS3MDL_I2C, TEST_LIS3MDL_MAG_ADDR,
                     LIS3MDL_XY_MODE_MEDIUM,
                     LIS3MDL_Z_MODE_MEDIUM, LIS3MDL_ODR_10Hz,
                     LIS3MDL_SCALE_4G, LIS3MDL_OP_CONT_CONV) == 0) {
        puts("[ OK ]\n");
    }
    else {
        puts("[ FAIL ]\n");
        return 1;
    }

    while(1){
        lis3mdl_read_mag(&dev, &mag_value);
        printf("Magnetometer [G]:\tX: %2d\tY: %2d\tZ: %2d\n", mag_value.x_axis,
                                                         mag_value.y_axis,
                                                         mag_value.z_axis);
        lis3mdl_read_temp(&dev, &temp_value);
        printf("Temperature:\t\t%i°C\n", temp_value);

        xtimer_usleep(SLEEP);
    }

    return 0;
}
