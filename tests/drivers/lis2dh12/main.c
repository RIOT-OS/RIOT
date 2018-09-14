/*
 * Copyright (C) 2018 Freie Universität Berlin
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
 * @brief       Test application for the LIS2DH12 accelerometer driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "fmt.h"
#include "xtimer.h"
#include "lis2dh12.h"
#include "lis2dh12_params.h"


#define DELAY       (100UL * US_PER_MS)

/* allocate some memory to hold one formated string for each sensor output, so
 * one string for the X, Y, and Z reading, respectively */
static char str_out[3][8];

/* allocate device descriptor */
static lis2dh12_t dev;

int main(void)
{
    puts("LIS2DH12 accelerometer driver test application\n");

    puts("Initializing LIS2DH12 sensor... ");
    if (lis2dh12_init(&dev, &lis2dh12_params[0]) == LIS2DH12_OK) {
        puts("[OK]");
    }
    else {
        puts("[Failed]\n");
        return 1;
    }

    xtimer_ticks32_t last_wakeup = xtimer_now();
    while (1) {
        /* read sensor data */
        int16_t data[3];
        if (lis2dh12_read(&dev, data) != LIS2DH12_OK) {
            puts("error: unable to retrieve data from sensor, quitting now");
            return 1;
        }

        /* format data */
        for (int i = 0; i < 3; i++) {
            size_t len = fmt_s16_dfp(str_out[i], data[i], -3);
            str_out[i][len] = '\0';
        }

        /* print data to STDIO */
        printf("X: %8s Y: %8s Z: %8s\n", str_out[0], str_out[1], str_out[2]);

        xtimer_periodic_wakeup(&last_wakeup, DELAY);
    }

    return 0;
}
