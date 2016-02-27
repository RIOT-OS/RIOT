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

#define RATE        (200 * MS_IN_USEC)      /* 200ms */

int main(void)
{
    bh1750fvi_t dev;
    uint32_t last = xtimer_now();

    puts("BH1750FVI ambient light sensor test\n");

    /* initialize the device */
    bh1750fvi_init(&dev, (bh1750fvi_params_t *)(&bh1750fvi_params));

    /* periodically sample the sensor */
    while(1) {
        uint16_t val = bh1750fvi_sample(&dev);
        printf("value: %5i lux\n", (int)val);
        xtimer_usleep_until(&last, RATE);
    }

    return 0;
}
