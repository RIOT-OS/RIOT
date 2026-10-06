/*
 * SPDX-FileCopyrightText: 2026 Mathis LECRIVAIN <lecrivain.mathis@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the MAX1161X ADC driver
 *
 * @author      Mathis Lécrivain <lecrivain.mathis@gmail.com>
 * @}
 */

#include <stdio.h>

#include "max1161x.h"
#include "max1161x_params.h"
#include "ztimer.h"

#define SLEEP_MS (1000)

static max1161x_t dev;

int main(void)
{
    int16_t data;

    puts("MAX1161X analog to digital driver test application\n");

    printf("Initializing MAX1161X analog to digital at I2C_DEV(%i)... ",
           max1161x_params[0].i2c);

    if (max1161x_init(&dev, &max1161x_params[0]) != 0) {
        puts("[Failed]");
        return 1;
    }
    puts("[OK]\n");

    while (1) {
        for (int chan = 0; chan < MAX1161X_NUM_CHANNELS; chan++) {
            if (max1161x_read_channel_raw(&dev, chan, &data) != 0) {
                printf("CH%d: read error\n", chan);
                continue;
            }
            printf("CH%d: %d\n", chan, data);
        }
        puts("---");

        ztimer_sleep(ZTIMER_MSEC, SLEEP_MS);
    }

    return 0;
}
