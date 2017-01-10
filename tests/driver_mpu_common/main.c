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
 * @brief       Test application for the MPU common driver
 *
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 * @author      smlng <s@mlng.net>
 *
 * @}
 */

#ifndef TEST_I2C
#error "TEST_I2C not defined"
#endif
#ifndef TEST_HW_ADDR
#error "TEST_HW_ADDR not defined"
#endif
#ifdef MPU_WITH_COMPASS
#ifndef TEST_COMP_ADDR
#error "TEST_COMP_ADDR not defined"
#endif
#endif /* MPU_WITH_COMPASS */

#include <stdio.h>
#include <inttypes.h>

#include "mpu_common.h"
#include "xtimer.h"
#include "board.h"

#define SLEEP   (1000 * MS_IN_USEC)

int main(void)
{
    mpu_dev_t dev;
    mpu_results_t measurement;
    int32_t temperature;
    int result;

    puts("MPU-COMMON test application\n");

    printf("+------------Initializing------------+\n");
#ifdef MPU_WITH_COMPASS
    result = mpu_init(&dev, TEST_I2C, TEST_HW_ADDR, TEST_COMP_ADDR);
#else
    result = mpu_init(&dev, TEST_I2C, TEST_HW_ADDR);
#endif

    if (result == -1) {
        puts("[Error] The given i2c is not enabled");
        return 1;
    }
    else if (result == -2) {
        puts("[Error] The compass did not answer correctly on the given address");
        return 1;
    }

    mpu_set_sample_rate(&dev, 200);
    if (dev.conf.sample_rate != 200) {
        puts("[Error] The sample rate was not set correctly");
        return 1;
    }
#ifdef MPU_WITH_COMPASS
    mpu_set_compass_sample_rate(&dev, 100);
    if (dev.conf.compass_sample_rate != 100) {
        puts("[Error] The compass sample rate was not set correctly");
        return 1;
    }
#endif /* MPU_WITH_COMPASS */
    printf("Initialization successful\n\n");
    printf("+------------Configuration------------+\n");
    printf("Sample rate: %"PRIu16" Hz\n", dev.conf.sample_rate);
    printf("Gyro full-scale range: 2000 DPS\n");
    printf("Accel full-scale range: 2 G\n");
#ifdef MPU_WITH_COMPASS
    printf("Compass sample rate: %"PRIu8" Hz\n", dev.conf.compass_sample_rate);
    printf("Compass X axis factory adjustment: %"PRIu8"\n", dev.conf.compass_x_adj);
    printf("Compass Y axis factory adjustment: %"PRIu8"\n", dev.conf.compass_y_adj);
    printf("Compass Z axis factory adjustment: %"PRIu8"\n", dev.conf.compass_z_adj);
#endif /* MPU_WITH_COMPASS */

    printf("\n+--------Starting Measurements--------+\n");
    while (1) {
        /* Get accel data in milli g */
        mpu_read_accel(&dev, &measurement);
        printf("Accel data [milli g] - X: %"PRId16"   Y: %"PRId16"   Z: %"PRId16"\n",
                measurement.x_axis, measurement.y_axis, measurement.z_axis);
        /* Get gyro data in dps */
        mpu_read_gyro(&dev, &measurement);
        printf("Gyro data [dps] - X: %"PRId16"   Y: %"PRId16"   Z: %"PRId16"\n",
                measurement.x_axis, measurement.y_axis, measurement.z_axis);
#ifdef MPU_WITH_COMPASS
        /* Get compass data in mikro Tesla */
        mpu_read_compass(&dev, &measurement);
        printf("Compass data [mikro T] - X: %"PRId16"   Y: %"PRId16"   Z: %"PRId16"\n",
                measurement.x_axis, measurement.y_axis, measurement.z_axis);
#endif /* MPU_WITH_COMPASS */
        /* Get temperature in milli degrees celsius */
        mpu_read_temperature(&dev, &temperature);
        printf("Temperature [milli deg] : %"PRId32"\n", temperature);
        printf("\n+-------------------------------------+\n");

        xtimer_usleep(SLEEP);
    }

    return 0;
}
