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
 * @brief       Test application for the LSM6DS3 3D accelerometer/gyroscope
 *
 * @author      René Herthel <rene-herthel@outlook.de>
 *
 * @}
 */
#ifndef TEST_LSM6DS3_I2C
#error "TEST_LSM6DS3_I2C not defined"
#endif
#ifndef TEST_LSM6DS3_DEV_ADDR
#error "TEST_LSM6DS3_DEV_ADDR not defined"
#endif

#include <stdio.h>

#include "xtimer.h"
#include "lsm6ds3.h"

#define SLEEP       (300 * 1000U)
#define ACC_S_RATE  LSM6DS3_ACC_SAMPLE_RATE_416HZ
#define ACC_SCALE   LSM6DS3_ACC_SCALE_2G
#define GYR_S_RATE  LSM6DS3_GYR_SAMPLE_RATE_416HZ
#define GYR_SCALE   LSM6DS3_GYR_SCALE_245DPS

int main(void)
{
    lsm6ds3_t dev;
    lsm6ds3_3d_data_t acc_value;
    lsm6ds3_3d_data_t gyr_value;
    int16_t temp_value;

    puts("\nLSM6DS3 test application\n");
    printf("Initializing LSM6DS3 sensor at I2C_%i ... ", TEST_LSM6DS3_I2C);

    if (lsm6ds3_init(&dev, TEST_LSM6DS3_I2C, TEST_LSM6DS3_DEV_ADDR,
                     ACC_S_RATE, ACC_SCALE, GYR_S_RATE, GYR_SCALE) == 0) {
        puts("[ OK ]\n");
    }
    else {
        puts("[ FAIL ]\n");
        return 1;
    }

    while (1) {
        lsm6ds3_read_acc(&dev, &acc_value);
        lsm6ds3_read_gyr(&dev, &gyr_value);
        lsm6ds3_read_temp(&dev, &temp_value);

        printf("[ Accelerometer ] \t[ Gyroscope ]\n");
        printf("X: %5.i\t\tX: %5.i\n", acc_value.x_axis, gyr_value.x_axis);
        printf("Y: %5.i\t\tY: %5.i\n", acc_value.y_axis, gyr_value.y_axis);
        printf("Z: %5.i\t\tZ: %5.i\n", acc_value.z_axis, gyr_value.z_axis);
        printf("@%2.i°C\n\n", temp_value);

        xtimer_usleep(SLEEP);
    }

    return 0;
}
