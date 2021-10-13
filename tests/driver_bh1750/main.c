/*
 * Copyright (C) 2016 Freie Universität Berlin
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
 * @brief       Test application for the BH1750FVI ambient light sensor driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "xtimer.h"
#include "bh1750fvi.h"
#include "bh1750fvi_params.h"

#define RATE        (200LU * US_PER_MS)      /* 200ms */

int main(void)
{
    int res;
    bh1750fvi_t dev;
    xtimer_ticks32_t last = xtimer_now();

    puts("BH1750FVI ambient light sensor test\n");

    /* initialize the device */
    res = bh1750fvi_init(&dev, &bh1750fvi_params[0]);
    if (res != BH1750FVI_OK) {
        puts("error: unable to initialize sensor [I2C initialization error]");
        return 1;
    }

    /* periodically sample the sensor */
    while(1) {
        uint16_t val = bh1750fvi_sample(&dev);
        printf("value: %5i lux\n", (int)val);
        xtimer_periodic_wakeup(&last, RATE);
    }

    return 0;
}
