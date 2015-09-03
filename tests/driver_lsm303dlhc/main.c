/*
 * Copyright (C) 2014 Freie Universität Berlin
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
 * @brief       Test application for the LSM303DLHC accelerometer/magnetometer
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * @}
 */

#ifndef TEST_LSM303DLHC_I2C
#error "TEST_LSM303DLHC_I2C not defined"
#endif
#ifndef TEST_LSM303DLHC_MAG_ADDR
#error "TEST_LSM303DLHC_MAG_ADDR not defined"
#endif
#ifndef TEST_LSM303DLHC_ACC_ADDR
#error "TEST_LSM303DLHC_ACC_ADDR not defined"
#endif
#ifndef TEST_LSM303DLHC_ACC_PIN
#error "TEST_LSM303DLHC_ACC_PIN not defined"
#endif
#ifndef TEST_LSM303DLHC_MAG_PIN
#error "TEST_LSM303DLHC_MAG_PIN not defined"
#endif

#include <stdio.h>

#include "xtimer.h"
#include "lsm303dlhc.h"

#define SLEEP       (100 * 1000U)
#define ACC_S_RATE  LSM303DLHC_ACC_SAMPLE_RATE_10HZ
#define ACC_SCALE   LSM303DLHC_ACC_SCALE_2G
#define MAG_S_RATE  LSM303DLHC_MAG_SAMPLE_RATE_75HZ
#define MAG_GAIN    LSM303DLHC_MAG_GAIN_400_355_GAUSS

int main(void)
{
    lsm303dlhc_t dev;
    int16_t temp_value;
    lsm303dlhc_3d_data_t mag_value;
    lsm303dlhc_3d_data_t acc_value;

    puts("LSM303DLHC temperature test application\n");
    printf("Initializing LSM303DLHC sensor at I2C_%i... ", TEST_LSM303DLHC_I2C);

    if (lsm303dlhc_init(&dev, TEST_LSM303DLHC_I2C, TEST_LSM303DLHC_ACC_PIN, TEST_LSM303DLHC_MAG_PIN,
                        TEST_LSM303DLHC_ACC_ADDR, ACC_S_RATE, ACC_SCALE,
                        TEST_LSM303DLHC_MAG_ADDR, MAG_S_RATE, MAG_GAIN) == 0) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return 1;
    }

    while (1) {
        if (lsm303dlhc_read_acc(&dev, &acc_value) == 0) {
            printf("Accelerometer x: %i y: %i z: %i\n", acc_value.x_axis,
                                                        acc_value.y_axis,
                                                        acc_value.z_axis);
        }
        else {
            puts("\nFailed reading accelerometer values\n");
        }
        if (lsm303dlhc_read_temp(&dev, &temp_value) == 0) {
            printf("Temperature value: %i degrees\n", temp_value);
        }
        else {
            puts("\nFailed reading value\n");
        }

        if (lsm303dlhc_read_mag(&dev, &mag_value) == 0) {
            printf("Magnetometer x: %i y: %i z: %i\n", mag_value.x_axis,
                                                       mag_value.y_axis,
                                                       mag_value.z_axis);
        }
        else {
            puts("\nFailed reading magnetometer values\n");
        }

        xtimer_usleep(SLEEP);
    }

    return 0;
}
