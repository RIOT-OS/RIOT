/*
 * Copyright (C) 2018 UC Berkeley
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for the PULSE_COUNTER driver
 *
 * @author      Hyung-Sin Kim <hs.kim@cs.berkeley.edu>
 *
 * @}
 */

#include <stdio.h>

#include "xtimer.h"
#include "fxos8700_params.h"

int main(void)
{
    fxos8700_t dev;

    puts("FXOS8700 driver test application\n");

    /* Initialization */
    if (fxos8700_init(&dev, &fxos8700_params[0])) {
        puts("[Failed]");
        return 1;
    }
    else {
        puts("[OK]\n");
    }

    while (1) {
        /* Acceleration and Magnetic field reading */
        fxos8700_measurement_t acc, mag;
        if (fxos8700_read(&dev, &acc, &mag) != FXOS8700_OK) {
            puts("Read failure\n");
        }
        else {
            printf("acceleration: (%d, %d, %d)\n", acc.x, acc.y, acc.z);
            printf("magnetic field: (%d, %d, %d)\n", mag.x, mag.y, mag.z);
        }
        xtimer_usleep(US_PER_SEC);
    }

    return 0;
}
