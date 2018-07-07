/*
 * Copyright (C) 2015 Freie Universität Berlin
 * Copyright 2018 Ville Hiltunen
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
 * @brief       Test application for the MPU-9250 Nine-Axis driver
 *
 * @note        Original code was for MPU-9150. Variables merely renamed.
 *
 * @author      Fabian Nack <nack@inf.fu-berlin.de>,
 *              modified by Ville Hiltunen <hiltunenvillej@gmail.com>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "xtimer.h"
#include "board.h"

#include "mpu9250.h"
#include "mpu9250_params.h"

#define SLEEP   (1000 * 1000u)

int main(void)
{
    mpu9250_t dev;
    mpu9250_results_t measurement;
    int32_t temperature;
    int result;

    puts("MPU-9250 test application\n");

    printf("+------------Initializing------------+\n");
    result = mpu9250_init(&dev, &mpu9250_params[0]);

    if (result == -1) {
        puts("[Error] The given i2c is not enabled");
        return 1;
    }
    else if (result == -2) {
        puts("[Error] The compass did not answer correctly on the given address");
        return 1;
    }

    mpu9250_set_sample_rate(&dev, 200);
    if (dev.conf.sample_rate != 200) {
        puts("[Error] The sample rate was not set correctly");
        return 1;
    }
    mpu9250_set_compass_sample_rate(&dev, 100);
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
        mpu9250_read_accel(&dev, &measurement);
        printf("Accel data [milli g] - X: %"PRId16"   Y: %"PRId16"   Z: %"PRId16"\n",
                measurement.x_axis, measurement.y_axis, measurement.z_axis);
        /* Get gyro data in dps */
        mpu9250_read_gyro(&dev, &measurement);
        printf("Gyro data [dps] - X: %"PRId16"   Y: %"PRId16"   Z: %"PRId16"\n",
                measurement.x_axis, measurement.y_axis, measurement.z_axis);
        /* Get compass data in mikro Tesla */
        mpu9250_read_compass(&dev, &measurement);
        printf("Compass data [micro T] - X: %"PRId16"   Y: %"PRId16"   Z: %"PRId16"\n",
                measurement.x_axis, measurement.y_axis, measurement.z_axis);
        /* Get temperature in milli degrees celsius */
        mpu9250_read_temperature(&dev, &temperature);
        printf("Temperature [milli deg] : %"PRId32"\n", temperature);
        printf("\n+-------------------------------------+\n");

        xtimer_usleep(SLEEP);
    }

    return 0;
}
