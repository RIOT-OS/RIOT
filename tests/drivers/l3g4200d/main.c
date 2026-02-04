/*
 * SPDX-FileCopyrightText: 2014 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the L3G4200 gyroscope driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "xtimer.h"
#include "l3g4200d.h"
#include "l3g4200d_params.h"

#define SLEEP_USEC  (100 * 1000U)

int main(void)
{
    l3g4200d_t dev;
    l3g4200d_data_t acc_data;

    puts("L3G4200 gyroscope driver test application\n");
    puts("Initializing L3G4200 sensor");
    if (l3g4200d_init(&dev, &l3g4200d_params[0]) == 0) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return 1;
    }

    while (1) {
        l3g4200d_read(&dev, &acc_data);

        printf("Gyro data [dps] - X: %6i   Y: %6i   Z: %6i\n",
               acc_data.acc_x, acc_data.acc_y, acc_data.acc_z);

        xtimer_usleep(SLEEP_USEC);
    }

    return 0;
}
