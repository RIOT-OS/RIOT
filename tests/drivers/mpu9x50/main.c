/*
 * SPDX-FileCopyrightText: 2015 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2019 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the MPU-9X50 (MPU9150 and MPU9250) Nine-Axis driver
 *
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 * @author      Jannes Volkens <jannes.volkens@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "timex.h"
#include "ztimer.h"
#include "board.h"

#include "mpu9x50.h"
#include "mpu9x50_params.h"

int main(void)
{
    mpu9x50_t dev;
    mpu9x50_results_t measurement;
    int32_t temperature;
    int result;

    puts("MPU-9X50 test application\n");

    printf("+------------Initializing------------+\n");
    result = mpu9x50_init(&dev, &mpu9x50_params[0]);

    if (result == -1) {
        puts("[Error] The given i2c is not enabled");
        return 1;
    }
    else if (result == -2) {
        puts("[Error] The compass did not answer correctly on the given address");
        return 1;
    }

    mpu9x50_set_sample_rate(&dev, 200);
    if (dev.conf.sample_rate != 200) {
        puts("[Error] The sample rate was not set correctly");
        return 1;
    }
    mpu9x50_set_compass_sample_rate(&dev, 100);
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
        mpu9x50_read_accel(&dev, &measurement);
        printf("Accel data [milli g] - X: %"PRId16"   Y: %"PRId16"   Z: %"PRId16"\n",
                measurement.x_axis, measurement.y_axis, measurement.z_axis);
        /* Get gyro data in dps */
        mpu9x50_read_gyro(&dev, &measurement);
        printf("Gyro data [dps] - X: %"PRId16"   Y: %"PRId16"   Z: %"PRId16"\n",
                measurement.x_axis, measurement.y_axis, measurement.z_axis);
        /* Get compass data in mikro Tesla */
        mpu9x50_read_compass(&dev, &measurement);
        printf("Compass data [micro T] - X: %"PRId16"   Y: %"PRId16"   Z: %"PRId16"\n",
                measurement.x_axis, measurement.y_axis, measurement.z_axis);
        /* Get temperature in milli degrees celsius */
        mpu9x50_read_temperature(&dev, &temperature);
        printf("Temperature [milli deg] : %"PRId32"\n", temperature);
        printf("\n+-------------------------------------+\n");

        ztimer_sleep(ZTIMER_MSEC, MS_PER_SEC);  /* 1s delay */
    }

    return 0;
}
