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
 * @brief       Test application for the LPD8808 LED strip driver
 *
 * Refer to the accompanying README.md for more information.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <string.h>
#include <stdio.h>

#include "xtimer.h"

#include "lpd8808.h"
#include "lpd8808_params.h"

/**
 * @brief   Switch to the next LED every 10ms
 */
#define STEP_MS     (10 * 1000U)

/**
 * @brief   Allocate the device descriptor
 */
static lpd8808_t dev;

/**
 * @brief   Allocate a color_rgb_t struct for each LED on the strip
 */
static color_rgb_t leds[LPD8808_PARAM_LED_CNT];

int main(void)
{
    xtimer_ticks32_t now = xtimer_now();
    int pos = 0;
    int step = 1;
    color_hsv_t col = { 0.0, 1.0, 1.0 };

    /* initialize all LED color values to black (off) */
    memset(leds, 0, sizeof(color_rgb_t) * LPD8808_PARAM_LED_CNT);

    /* initialize the driver */
    lpd8808_init(&dev, &lpd8808_params[0]);

    while (1) {

        /* change the active color by running around the hue circle */
        col.h += 1.0;
        if (col.h > 360.0) {
            col.h = 0;
        }

        /* set the active LED to the active color value */
        memset(&leds[pos], 0, sizeof(color_rgb_t));
        pos += step;
        color_hsv2rgb(&col, &leds[pos]);

        /* apply the values to the LED strip */
        lpd8808_load_rgb(&dev, leds);

        /* switch direction once reaching an end of the strip */
        if ((pos == (LPD8808_PARAM_LED_CNT - 1)) || (pos == 0)) {
            step *= -1;
        }

        xtimer_periodic_wakeup(&now, STEP_MS);
    }

    return 0;
}
