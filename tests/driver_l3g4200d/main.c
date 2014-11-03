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
 * @brief       Test application for the L3G4200 gyroscope driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#ifndef TEST_L3G4200D_I2C
#error "TEST_L3G4200D_I2C not defined"
#endif
#ifndef TEST_L3G4200D_ADDR
#error "TEST_L3G4200D_ADDR not defined"
#endif
#ifndef TEST_L3G4200D_INT
#error "TEST_L3G4200D_INT not defined"
#endif
#ifndef TEST_L3G4200D_DRDY
#error "TEST_L3G4200D_DRDY not defined"
#endif

#include <stdio.h>

#include "vtimer.h"
#include "l3g4200d.h"

#define MODE        L3G4200D_MODE_100_25
#define SCALE       L3G4200D_SCALE_500DPS
#define SLEEP       (100 * 1000U)

int main(void)
{
    l3g4200d_t dev;
    l3g4200d_data_t acc_data;

    puts("L3G4200 gyroscope driver test application\n");
    printf("Initializing L3G4200 sensor at I2C_%i... ", TEST_L3G4200D_I2C);
    if (l3g4200d_init(&dev, TEST_L3G4200D_I2C, TEST_L3G4200D_ADDR,
                      TEST_L3G4200D_INT, TEST_L3G4200D_DRDY, MODE, SCALE) == 0) {
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

        vtimer_usleep(SLEEP);
    }

    return 0;
}
