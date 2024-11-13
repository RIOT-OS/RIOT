/*
 * Copyright (C) 2024 Orange
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
 * @brief       Test application for the SHT5461AS 4x7seg
 *
 * @author      Pierre Le Meur <pierre1.lemeur@orange.com>
 *
 * @}
 */


#include <stdio.h>
#include <inttypes.h>

#include "sha5461as_params.h"
#include "sha5461as.h"
#include "timex.h"
#include "ztimer.h"

#define TEST_DELAY_US   (1U * US_PER_SEC)  /* 2 seconds delay between each test */

int main(void)
{
    sha5461as_t dev;
    dev.digits = SHA5461AS_MAX_DIGITS;

    puts("Initializing SHA5461AS device.");
    if (sha5461as_init(&dev, &sha5461as_params[0]) != 0) {
        puts("[Error] Cannot initialize DSP0401 display.");
        return 1;
    }
    puts("Initialization successful.");

    ztimer_sleep(ZTIMER_USEC, TEST_DELAY_US);
    gpio_set(dev.params.dig1);
    gpio_set(dev.params.dig2);
    gpio_set(dev.params.dig3);
    gpio_set(dev.params.dig4);
    gpio_set(dev.params.a); ztimer_sleep(ZTIMER_USEC, TEST_DELAY_US);
    gpio_set(dev.params.b); ztimer_sleep(ZTIMER_USEC, TEST_DELAY_US);
    gpio_set(dev.params.c); ztimer_sleep(ZTIMER_USEC, TEST_DELAY_US);
    gpio_set(dev.params.d); ztimer_sleep(ZTIMER_USEC, TEST_DELAY_US);
    gpio_set(dev.params.e); ztimer_sleep(ZTIMER_USEC, TEST_DELAY_US);
    gpio_set(dev.params.f); ztimer_sleep(ZTIMER_USEC, TEST_DELAY_US);
    gpio_set(dev.params.g); ztimer_sleep(ZTIMER_USEC, TEST_DELAY_US);
    gpio_set(dev.params.dp);

    gpio_clear(dev.params.dig2);
    gpio_clear(dev.params.dig3);
    gpio_clear(dev.params.dig4);

    ztimer_sleep(ZTIMER_USEC, TEST_DELAY_US);
    /*    R        I         O       T     */
    /* 11101110 00001100 11111100 00001110 */
    uint32_t binary_riot = 0b11101110000011001111110000001110;
    sha5461as_set_all_value(&dev, binary_riot);

    if (sha5461as_display(&dev) == 0) {
        puts("Launched...");
    }
    else {
        puts("Error");
    }

    for (volatile uint32_t i = 0; i < CLOCK_CORECLOCK / 5; ++i) {}

    if (sha5461as_display(&dev) == 0) {
        puts("...Stopped");
    }
    else {
        puts("Error");
    }

    while (1) {}

    return 0;
}
