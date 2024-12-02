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
 * @brief       Test application for a 4x7seg
 *
 * @author      Pierre Le Meur <pierre1.lemeur@orange.com>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "digit7seg_params.h"
#include "digit7seg.h"
#include "timex.h"
#include "ztimer.h"

#define TEST_DELAY_US   (1U * US_PER_SEC)  /* 1 seconds delay between each test */

int main(void)
{
    digit7seg_t dev;

    puts("Initializing the digit 7 seg device.");
    if (digit7seg_init(&dev, &digit7seg_params[0]) != 0) {
        puts("[Error] Cannot initialize DSP0401 display.");
        return 1;
    }
    puts("Initialization successful.");

    ztimer_sleep(ZTIMER_USEC, TEST_DELAY_US);
    gpio_set(dev.params.dig1);
    gpio_set(dev.params.dig2);
    gpio_set(dev.params.dig3);
    gpio_set(dev.params.dig4);
    gpio_set(dev.params.data_a); ztimer_sleep(ZTIMER_USEC, TEST_DELAY_US);
    gpio_set(dev.params.data_b); ztimer_sleep(ZTIMER_USEC, TEST_DELAY_US);
    gpio_set(dev.params.data_c); ztimer_sleep(ZTIMER_USEC, TEST_DELAY_US);
    gpio_set(dev.params.data_d); ztimer_sleep(ZTIMER_USEC, TEST_DELAY_US);
    gpio_set(dev.params.data_e); ztimer_sleep(ZTIMER_USEC, TEST_DELAY_US);
    gpio_set(dev.params.data_f); ztimer_sleep(ZTIMER_USEC, TEST_DELAY_US);
    gpio_set(dev.params.data_g); ztimer_sleep(ZTIMER_USEC, TEST_DELAY_US);
    gpio_set(dev.params.data_dp);

    gpio_clear(dev.params.dig2);
    gpio_clear(dev.params.dig3);
    gpio_clear(dev.params.dig4);

    ztimer_sleep(ZTIMER_USEC, TEST_DELAY_US);
    /*    R        I         O       T     */
    /* 01110111 00110000 00111111 01110000 */
    uint32_t binary_riot = 0b01110111001100000011111101110000;
    digit7seg_set_all_value(&dev, binary_riot);

    if (digit7seg_poweron(&dev) == 0) {
        puts("Launched...");
    }
    else {
        puts("Error");
    }

    ztimer_sleep(ZTIMER_USEC, TEST_DELAY_US * 3);

    digit7seg_poweroff(&dev);

    return 0;
}
