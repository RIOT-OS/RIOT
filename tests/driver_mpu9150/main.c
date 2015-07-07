/*
 * Copyright (C) 2015 Freie Universität Berlin
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
 * @brief       Test application for the MPU-9150 Nine-Axis driver
 *
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 *
 * @}
 */

#ifndef TEST_I2C
#error "TEST_I2C not defined"
#endif
#ifndef TEST_HW_ADDR
#error "TEST_HW_ADDR not defined"
#endif
#ifndef TEST_COMP_ADDR
#error "TEST_COMP_ADDR not defined"
#endif

#include <stdio.h>
#include <inttypes.h>

#include "mpu9150.h"
#include "vtimer.h"
#include "board.h"

#define SLEEP   (1000 * 1000u)

int main(void)
{
    mpu9150_t dev;
    mpu9150_results_t measurement;
    int32_t temperature;
    int result;

    puts("MPU-9150 test application\n");

    vtimer_init();

    printf("+------------Initializing------------+\n");
    result = mpu9150_init(&dev, TEST_I2C, TEST_HW_ADDR, TEST_COMP_ADDR);

    if (result == -1) {
        puts("[Error] The given i2c is not enabled");
        return 1;
    }
    else if (result == -2) {
        puts("[Error] The compass did not answer correctly on the given address");
        return 1;
    }

    mpu9150_set_sample_rate(&dev, 200);
    if (dev.conf.sample_rate != 200) {
        puts("[Error] The sample rate was not set correctly");
        return 1;
    }
    mpu9150_set_compass_sample_rate(&dev, 100);
    if (dev.conf.compass_sample_rate != 100) {
        puts("[Error] The compass sample rate was not set correctly");
        return 1;
    }

    printf("Initialization successful\n\n");
    printf("+------------Configuration------------+\n");
    printf("Sample rate: %"PRIu16" Hz\n", dev.conf.sample_rate);
    printf("Compass sample rate: %"PRIu8" Hz\n", dev.conf.compass_sample_rate);
    printf("Gyro full-scale range: 2000 DPS\n");
    printf("Accel full-scale range: 2 G\n");
    printf("Compass X axis factory adjustment: %"PRIu8"\n", dev.conf.compass_x_adj);
    printf("Compass Y axis factory adjustment: %"PRIu8"\n", dev.conf.compass_y_adj);
    printf("Compass Z axis factory adjustment: %"PRIu8"\n", dev.conf.compass_z_adj);

    printf("\n+--------Starting Measurements--------+\n");
    while (1) {
        /* Get accel data in milli g */
        mpu9150_read_accel(&dev, &measurement);
        printf("Accel data [milli g] - X: %d   Y: %d   Z: %d\n",
                measurement.x_axis, measurement.y_axis, measurement.z_axis);
        /* Get gyro data in dps */
        mpu9150_read_gyro(&dev, &measurement);
        printf("Gyro data [dps] - X: %d   Y: %d   Z: %d\n",
                measurement.x_axis, measurement.y_axis, measurement.z_axis);
        /* Get compass data in mikro Tesla */
        mpu9150_read_compass(&dev, &measurement);
        printf("Compass data [mikro T] - X: %d   Y: %d   Z: %d\n",
                measurement.x_axis, measurement.y_axis, measurement.z_axis);
        /* Get temperature in milli degrees celsius */
        mpu9150_read_temperature(&dev, &temperature);
        printf("Temperature [milli deg] : %ld\n", temperature);
        printf("\n+-------------------------------------+\n");

        vtimer_usleep(SLEEP);
    }

    return 0;
}
