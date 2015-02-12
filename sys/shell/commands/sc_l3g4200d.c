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
 * @brief       Provides shell commands to poll the L3G4200D sensor
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "board.h"
#include "l3g4200d.h"

#ifdef MODULE_L3G4200D

#define MODE        L3G4200D_MODE_100_25
#define SCALE       L3G4200D_SCALE_500DPS

static l3g4200d_t l3g4200d_dev;

void _get_l3g4200d_init_handler(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    int res;

    res = l3g4200d_init(&l3g4200d_dev, L3G4200D_I2C, L3G4200D_ADDR,
                        L3G4200D_INT, L3G4200D_DRDY,
                        MODE, SCALE);

    if (res) {
        puts("Error initializing L3G4200D sensor.");
    }
    else {
        puts("Initialized L3G4200D sensor with default values");
    }
}

void _get_l3g4200d_read_handler(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    int res;
    l3g4200d_data_t data;

    if (!l3g4200d_dev.addr) {
        puts("Error: please call `l3g4200d_init` first!");
        return;
    }

    res = l3g4200d_read(&l3g4200d_dev, &data);
    if (res < 0) {
        puts("Error reading gyro values from L3G4200D.");
        return;
    }
    else {
        printf("L3G4200D: gyro values: roll(x): %6i   pitch(y): %6i   yaw(z): %6i\n",
               data.acc_x, data.acc_y, data.acc_z);
    }
}

#endif /* MODULE_L3G4200D */
