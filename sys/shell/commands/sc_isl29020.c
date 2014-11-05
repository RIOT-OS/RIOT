/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup shell_commands
 * @{
 *
 * @file
 * @brief       Provides shell commands to poll the ISL29020 sensor
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "board.h"
#include "isl29020.h"

#ifdef MODULE_ISL29020

#define MODE        ISL29020_MODE_AMBIENT
#define RANGE       ISL29020_RANGE_16K

static isl29020_t isl29020_dev;

void _get_isl29020_init_handler(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    int res;

    res = isl29020_init(&isl29020_dev, ISL29020_I2C, ISL29020_ADDR, RANGE, MODE);

    if (res) {
        puts("Error initializing ISL29020 sensor.");
    }
    else {
        puts("Initialized ISL29020 sensor with default values");
    }
}

void _get_isl29020_read_handler(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    int val;

    if (!isl29020_dev.address) {
        puts("Error: please call `isl29020_init` first!");
    }

    val = isl29020_read(&isl29020_dev);
    if (val < 0) {
        puts("Error reading brightness value from ISL29020.");
        return;
    }
    else {
        printf("ISL29020: brightness %i LUX\n", val);
    }
}

#endif /* MODULE_ISL29020 */
