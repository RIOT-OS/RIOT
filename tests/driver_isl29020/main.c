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
 * @brief       Test application for the ISL29020 light sensor
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "xtimer.h"
#include "isl29020.h"
#include "isl29020_params.h"

#define SLEEP_USEC  (250 * 1000U)

int main(void)
{
    isl29020_t dev;

    puts("ISL29020 light sensor test application\n");
    puts("Initializing ISL29020 sensor");
    if (isl29020_init(&dev, &isl29020_params[0]) == 0) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return 1;
    }

    while (1) {
        printf("Light value: %5i LUX\n", isl29020_read(&dev));
        xtimer_usleep(SLEEP_USEC);
    }

    return 0;
}
