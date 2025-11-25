/*
 * Copyright (C) 2017 HAW Hamburg
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
 * @brief       Test application for the MY9221 LED controller
 *
 * @author      Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

#include <stdio.h>

#include "log.h"
#include "xtimer.h"
#include "my9221.h"

#define TEST_STEP       (5U)
#define TEST_WAIT       (42*US_PER_MS)

static my9221_params_t params = {
    .leds       = TEST_MY9221_NUM,
    .dir        = TEST_MY9221_DIR,
    .clk        = TEST_MY9221_CLK,
    .dat        = TEST_MY9221_DAT,
};

int main(void)
{
    my9221_t dev;
    /* init display */
    puts("[START]");
    if (my9221_init(&dev, &params) != 0) {
        puts("[FAILED]");
        return 1;
    }
    /* run some tests */
    LOG_INFO("- light up all LEDs one by one.\n");
    for (unsigned i=0; i < dev.params.leds; ++i) {
        my9221_set_led(&dev, i, MY9221_LED_ON);
        xtimer_usleep(TEST_WAIT);
        my9221_set_led(&dev, i, MY9221_LED_OFF);
    }
    xtimer_usleep(TEST_WAIT);
    for (unsigned j=dev.params.leds; j > 0 ; --j) {
        unsigned i = j-1;
        my9221_set_led(&dev, i, MY9221_LED_ON);
        xtimer_usleep(TEST_WAIT);
        my9221_set_led(&dev, i, MY9221_LED_OFF);
    }
    xtimer_usleep(TEST_WAIT);
    LOG_INFO("- light up all LEDs to 33%%.\n");
    for (unsigned i=0; i < dev.params.leds; ++i) {
        my9221_set_led(&dev, i, MY9221_LED_ON/3);
        xtimer_usleep(TEST_WAIT);
    }
    xtimer_usleep(TEST_WAIT);
    LOG_INFO("- light up all LEDs to 66%%.\n");
    for (unsigned i=0; i < dev.params.leds; ++i) {
        my9221_set_led(&dev, i, (MY9221_LED_ON/3)*2);
        xtimer_usleep(TEST_WAIT);
    }
    xtimer_usleep(TEST_WAIT);
    LOG_INFO("- light up all LEDs to 100%%.\n");
    for (unsigned i=0; i < dev.params.leds; ++i) {
        my9221_set_led(&dev, i, MY9221_LED_ON);
        xtimer_usleep(TEST_WAIT);
    }
    xtimer_usleep(TEST_WAIT);
    LOG_INFO("- turn off all LEDs\n");
    for (unsigned i=dev.params.leds; i > 0 ; --i) {
        my9221_toggle_led(&dev, i-1);
        xtimer_usleep(TEST_WAIT);
    }
    puts("[SUCCESS]");

    return 0;
}
