/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test for the on-board LED macros
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>

#ifdef MODULE_ONBOARD_LED
#include "onboard_led.h"
#endif

#ifdef CLOCK_CORECLOCK
#define DELAY_SHORT         (CLOCK_CORECLOCK / 50)
#else
#define DELAY_SHORT         (500000UL)
#endif
#define DELAY_LONG          (DELAY_SHORT * 4)

void dumb_delay(uint32_t delay)
{
    for (uint32_t i = 0; i < delay; i++) {
        __asm__("nop");
    }
}

int main(void)
{
    puts("On-board LED test\n");

#ifndef MODULE_ONBOARD_LED

    puts("No on-board LED(s) available");

#else

    printf("Available LEDs: %u\n\n", LED_NUMOF);
    puts("Will now light up each LED once short and twice long in a loop");

    for (unsigned i = 0; i < LED_NUMOF; i++) {
        onboard_led_on(i);
        dumb_delay(DELAY_LONG);
        onboard_led_off(i);
        dumb_delay(DELAY_LONG);
        onboard_led_toggle(i);
        dumb_delay(DELAY_SHORT);
        onboard_led_toggle(i);
        dumb_delay(DELAY_SHORT);
        onboard_led_toggle(i);
        dumb_delay(DELAY_SHORT);
        onboard_led_toggle(i);
        dumb_delay(DELAY_LONG);
    }

#endif

    return 0;
}
