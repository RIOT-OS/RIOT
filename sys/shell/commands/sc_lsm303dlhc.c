/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 * @brief       Provides shell commands to poll lsm303dlhc sensor
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include "board.h"
#include "lsm303dlhc.h"

#ifdef MODULE_LSM303DLHC

#define ACC_S_RATE  LSM303DLHC_ACC_SAMPLE_RATE_10HZ
#define ACC_SCALE   LSM303DLHC_ACC_SCALE_2G
#define MAG_S_RATE  LSM303DLHC_MAG_SAMPLE_RATE_75HZ
#define MAG_GAIN    LSM303DLHC_MAG_GAIN_400_355_GAUSS

static lsm303dlhc_t lsm303_dev;

void _get_lsm303dlhc_init_handler(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    uint8_t error;

    error = lsm303dlhc_init(&lsm303_dev, LSM303DLHC_I2C,
                            LSM303DLHC_INT1, LSM303DLHC_DRDY,
                            LSM303DLHC_ACC_ADDR, ACC_S_RATE, ACC_SCALE,
                            LSM303DLHC_MAG_ADDR, MAG_S_RATE, MAG_GAIN);

    if (error) {
        puts("Error initializing lsm303dlhc sensor.");
    }
    else {
        puts("Initialized lsm303dlhc sensor with default values");
    }
}

void _get_lsm303dlhc_read_handler(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    uint8_t error;
    lsm303dlhc_3d_data_t data;

    if (!lsm303_dev.acc_address || !lsm303_dev.mag_address) {
        puts("Error: please call `lsm303dlhc_init` first!");
        return;
    }

    error = lsm303dlhc_read_acc(&lsm303_dev, &data);
    if (error) {
        puts("Error reading accelerometer data from lsm303dlhc.");
        return;
    }
    else {
        printf("lsm303dlhc: Accelerometer {%i, %i, %i} mg\n", data.x_axis, data.y_axis, data.z_axis);
    }

    error = lsm303dlhc_read_mag(&lsm303_dev, &data);
    if (error) {
        puts("Error reading magnetometer data from lsm303dlhc.");
        return;
    }
    else {
        printf("lsm303dlhc: Magnetometer {%i, %i, %i}/1100 gauss\n", data.x_axis, data.y_axis, data.z_axis);
    }

    error = lsm303dlhc_read_temp(&lsm303_dev, &(data.x_axis));
    if (error) {
        puts("Error reading temperature data from lsm303dlhc.");
        return;
    }
    else {
        printf("lsm303dlhc: Temperature %i\n", data.x_axis);
    }
}

#endif /* MODULE_LSM303DLHC */
