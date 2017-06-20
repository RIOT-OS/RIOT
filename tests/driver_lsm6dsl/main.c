/*
 * Copyright (C) 2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @brief       Test application for the LSM6DSL accelerometer/gyroscope driver.
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 *
 */

#include <stdio.h>

#include "xtimer.h"
#include "lsm6dsl.h"
#include "lsm6dsl_params.h"

#define SLEEP       (100 * 1000U)

int main(void)
{
    lsm6dsl_t dev;
    int16_t temp_value;
    lsm6dsl_3d_data_t mag_value;
    lsm6dsl_3d_data_t acc_value;

    puts("LSM6DSL test application\n");
    printf("Initializing LSM6DSL sensor at I2C_%i... ", lsm6dsl_params->i2c);

    if (lsm6dsl_init(&dev, lsm6dsl_params) == 0) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return 1;
    }

    while (1) {
        if (lsm6dsl_read_acc(&dev, &acc_value) == 0) {
            printf("Accelerometer x: %i y: %i z: %i\n", acc_value.x,
                                                        acc_value.y,
                                                        acc_value.z);
        }
        else {
            puts("\nFailed reading accelerometer values\n");
        }
        if (lsm6dsl_read_temp(&dev, &temp_value) == 0) {
            printf("Temperature value: %i degrees\n", temp_value);
        }
        else {
            puts("\nFailed reading value\n");
        }

        if (lsm6dsl_read_gyro(&dev, &mag_value) == 0) {
            printf("Gyroscope x: %i y: %i z: %i\n", mag_value.x,
                                                    mag_value.y,
                                                    mag_value.z);
        }
        else {
            puts("\nFailed reading Gyroscope values\n");
        }

        xtimer_usleep(SLEEP);
    }

    return 0;
}
