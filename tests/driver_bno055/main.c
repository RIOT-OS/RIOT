/*
 * Copyright (C) 2019 Wouter Horlings
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_bno055
 * @{
 *
 * @file
 * @brief       Device driver interface for the BNO055 9-axis sensor
 *
 * @author      Wouter Horlings <wouter@horlin.gs>
 * @}
 */



#include <stdio.h>
#include "bno055.h"
#include "bno055_params.h"
#include "xtimer.h"

int main(void)
{
    xtimer_sleep(1);
    printf("Test for BNO055 device\n");
    int result;
    bno055_t dev;

    printf("Initialize BNO055\n");
    result = bno055_init(&dev, &bno055_params[0]);
    if (result != BNO055_OK) {
        printf("[Error] Failed to connect with the i2c device\n");
    }

    printf("Initialized succesful\n");

    quat_t quat_data;
    eul_t eul_data;
    euclid_t grv_data;
    euclid_t lia_data;
    euclid_t mag_data;
    euclid_t gyr_data;
    euclid_t acc_data;
    while (true) {
        xtimer_sleep(1);
        if (bno055_quat_float(&dev, &quat_data)) {
            printf("data read failed\n");
        }
        else {
            printf("Quaternions: %f + %fi + %fj +%fk\n", quat_data.w,
                   quat_data.x, quat_data.y, quat_data.z);
        }
        if (bno055_eul_float(&dev, &eul_data)) {
            printf("data read failed\n");
        }
        else {
            printf("Euler rotation: Heading: %.3f, Roll %.3f, Pitch %.3f\n",
                   eul_data.heading, eul_data.roll, eul_data.pitch);
        }
        if (bno055_grv_float(&dev, &grv_data)) {
            printf("data read failed\n");
        }
        else {
            printf("Gravitation Vector: x: %.2f, y: %.2f, z:%.2f\n", grv_data.x,
                   grv_data.y, grv_data.z);
        }
        if (bno055_lia_float(&dev, &lia_data)) {
            printf("data read failed\n");
        }
        else {
            printf("Linear Acceleration: x: %.2f, y: %.2f, z:%.2f\n",
                   lia_data.x, lia_data.y, lia_data.z);
        }
        if (bno055_mag_float(&dev, &mag_data)) {
            printf("data read failed\n");
        }
        else {
            printf("Magnetometer: x: %.2f, y: %.2f, z:%.2f\n", mag_data.x,
                   mag_data.y, mag_data.z);
        }
        if (bno055_acc_float(&dev, &acc_data)) {
            printf("data read failed\n");
        }
        else {
            printf("Accelerometer: x: %.2f, y: %.2f, z:%.2f\n", acc_data.x,
                   acc_data.y, acc_data.z);
        }
        if (bno055_gyr_float(&dev, &gyr_data)) {
            printf("data read failed\n");
        }
        else {
            printf("Gyroscope: x: %.2f, y: %.2f, z:%.2f\n", gyr_data.x,
                   gyr_data.y, gyr_data.z);
        }
    }
}
