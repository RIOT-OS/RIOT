/*
 * SPDX-FileCopyrightText: 2020 Locha Inc
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the GP2Y10xx Compact Dust Density Sensors.
 *
 * @author      Jean Pierre Dudey <jeandudey@hotmail.com>
 * @}
 */

#include <stdio.h>

#include "xtimer.h"
#include "timex.h"
#include "gp2y10xx.h"
#include "gp2y10xx_params.h"

static gp2y10xx_t dev;

int main(void)
{
    int res;
    uint16_t density;

    puts("GP2Y10xx driver test application\n");
    printf("Initializing GP2Y10xx at ADC_LINE(%i)...\n",
           gp2y10xx_params->aout);

    if ((res = gp2y10xx_init(&dev, gp2y10xx_params)) == GP2Y10XX_OK) {
        puts("[OK]");
    }
    else {
        printf("[Failed] res=%i\n", res);
        return -1;
    }

    while (1) {
        res = gp2y10xx_read_density(&dev, &density);
        if (res == GP2Y10XX_OK) {
            printf("Dust density %"PRIu16" ug/m3\n", density);
        }
        else {
            printf("Error reading data. err: %d\n", res);
        }

        xtimer_msleep(250);
    }

    return 0;
}
