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

#include <stdio.h>

#include "xtimer.h"
#include "lsm303dlhc.h"
#include "lsm303dlhc_params.h"

#define SLEEP_USEC  (100 * 1000U)

int main(void)
{
    lsm303dlhc_t dev;

    puts("LSM303DLHC temperature test application\n");
    puts("Initializing LSM303DLHC sensor");

    if (lsm303dlhc_init(&dev, &lsm303dlhc_params[0]) == 0) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return 1;
    }

    while (1) {
        int16_t temp_value;
        lsm303dlhc_3d_data_t mag_value;
        lsm303dlhc_3d_data_t acc_value;

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

        xtimer_usleep(SLEEP_USEC);
    }

    return 0;
}
