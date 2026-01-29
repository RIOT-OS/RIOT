/*
 * Copyright (C) 2022 BISSELL Homecare, Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       light ws2812 example
 *
 * @author      Dave VanKampen <david.vankampen@bissell.com>
 *
 * @}
 */

#include <stdio.h>
#include "light_ws2812_cortex.h"
#include "xtimer.h"
#include "random.h"
#define SEED 234

#define NUM_LEDS    5
uint8_t leds[NUM_LEDS] = { 0x00};
int main(void)
{
    puts("Generated RIOT application: 'light_ws2812'");
    random_init(SEED);
    while (1) {
        for (uint8_t idx = 0; idx < NUM_LEDS; idx++) {
            leds[idx] = (uint8_t)random_uint32();
        }
        ws2812_sendarray(leds, NUM_LEDS);
        xtimer_usleep(50000);
    }
    return 0;
}
