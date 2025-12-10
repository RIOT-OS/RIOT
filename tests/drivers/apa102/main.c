/*
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for the APA102 LED strip driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "xtimer.h"
#include "color.h"

#include "apa102.h"
#include "apa102_params.h"

/**
 * @brief   Switch to the next LED every 10ms
 */
#define STEP_MS     (200 * US_PER_MS)

/**
 * @brief   Interval for dimming colors
 */
#define DIM         (100 * US_PER_MS)

/**
 * @brief   Step through brightness levels (0-255) in 32 steps
 */
#define BSTEP       (8U)

/**
 * @brief   Allocate the device descriptor
 */
static apa102_t dev;

/**
 * @brief   Allocate a color_rgb_t struct for each LED on the strip
 */
static color_rgba_t leds[APA102_PARAM_LED_NUMOF];

static void setcolor(int color, uint8_t alpha)
{
    for (int i = 0; i < (int)APA102_PARAM_LED_NUMOF; i++) {
        leds[i].alpha = alpha;
        memset(&leds[i].color, 0, sizeof(color_rgb_t));
        switch (color) {
            case 0:
                leds[i].color.r = 255;
                break;
            case 1:
                leds[i].color.g = 255;
                break;
            case 2:
                leds[i].color.b = 255;
                break;
        }
    }
}

int main(void)
{
    int pos = 0;
    int step = 1;
    color_hsv_t col = { 0.0, 1.0, 1.0 };

    puts("APA102 Test App");

    /* initialize all LED color values to black (off) */
    memset(leds, 0, sizeof(color_rgba_t) * APA102_PARAM_LED_NUMOF);

    /* initialize the driver */
    apa102_init(&dev, &apa102_params[0]);

    puts("Initialization done.");

    /* set to each red, green, and blue, and fade each color in and out */
    for (int col = 0; col <= 2; col++) {
        int i = 0;
        for (; i <= (int)UINT8_MAX; i += BSTEP) {
            setcolor(col, (uint8_t)i);
            apa102_load_rgba(&dev, leds);
            xtimer_usleep(DIM);
        }
        i -= BSTEP;
        for (; i >= 0; i -= BSTEP) {
            setcolor(col, (uint8_t)i);
            apa102_load_rgba(&dev, leds);
            xtimer_usleep(DIM);
        }
    }

    puts("Color Fading done");

    /* reset color values */
    setcolor(-1, 255);
    apa102_load_rgba(&dev, leds);

    xtimer_ticks32_t now = xtimer_now();

    while (1) {
        /* change the active color by running around the hue circle */
        col.h += 1.0;
        if (col.h > 360.0) {
            col.h = 0.0;
        }

        /* set the active LED to the active color value */
        memset(&leds[pos].color, 0, sizeof(color_rgb_t));
        pos += step;
        color_hsv2rgb(&col, &leds[pos].color);

        /* apply the values to the LED strip */
        apa102_load_rgba(&dev, leds);

        /* switch direction once reaching an end of the strip */
        if ((pos == (APA102_PARAM_LED_NUMOF - 1)) || (pos == 0)) {
            step *= -1;
        }

        xtimer_periodic_wakeup(&now, STEP_MS);
    }

    return 0;
}
