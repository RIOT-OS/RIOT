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
 * @brief       Test application for the APDS9007 driver
 *
 * @author      Hyung-Sin Kim <hs.kim@cs.berkeley.edu>
 *
 * @}
 */

#include <stdio.h>

#include "xtimer.h"
#include "apds9007_params.h"
#include "apds9007.h"

#define SLEEP       US_PER_SEC

int main(void)
{
    apds9007_t dev;

    printf("APDS9007 driver test application\n");

    /* Initialization */
    apds9007_init(&dev, &apds9007_params[0]);

    while (1) {
        /* Pulse counter reading */
        int16_t light;
        if (apds9007_read(&dev, &light) != APDS9007_OK) {
            printf("Read failure\n");
        } else {
            printf("illuminance in raw voltage value: %d\n", light);
        }
        xtimer_usleep(SLEEP);
    }

    return 0;
}
