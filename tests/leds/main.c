/*
 * SPDX-FileCopyrightText: 2016 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
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

#include "clk.h"
#include "led.h"
#include "periph/gpio.h"

#define DELAY_SHORT         (coreclk() / 50)
#define DELAY_LONG          (DELAY_SHORT * 4)

void dumb_delay(uint32_t delay)
{
    for (uint32_t i = 0; i < delay; i++) {
        __asm__("nop");
    }
}

int main(void)
{
    /* get the number of available LED's and turn them all off*/
    unsigned numof = LED_NUMOF;
    for (unsigned i = 0; i < numof; i++) {
        led_off(i);
    }

    puts("On-board LED test\n");
    /* cppcheck-suppress knownConditionTrueFalse
     * (reason: board-dependent ifdefs) */
    if (numof == 0) {
        puts("NO LEDs AVAILABLE");
    }
    else {
        printf("Available LEDs: %i\n\n", numof);
        puts("Will now light up each LED once short and twice long");
    }

    for (unsigned i = 0; i < 4; ++i) {
#ifdef LED0_IS_PRESENT
        LED0_ON;
        dumb_delay(DELAY_LONG);
        LED0_OFF;
        dumb_delay(DELAY_LONG);
        LED0_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED0_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED0_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED0_TOGGLE;
        dumb_delay(DELAY_LONG);
#endif
#ifdef LED1_IS_PRESENT
        LED1_ON;
        dumb_delay(DELAY_LONG);
        LED1_OFF;
        dumb_delay(DELAY_LONG);
        LED1_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED1_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED1_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED1_TOGGLE;
        dumb_delay(DELAY_LONG);
#endif
#ifdef LED2_IS_PRESENT
        LED2_ON;
        dumb_delay(DELAY_LONG);
        LED2_OFF;
        dumb_delay(DELAY_LONG);
        LED2_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED2_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED2_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED2_TOGGLE;
        dumb_delay(DELAY_LONG);
#endif
#ifdef LED3_IS_PRESENT
        LED3_ON;
        dumb_delay(DELAY_LONG);
        LED3_OFF;
        dumb_delay(DELAY_LONG);
        LED3_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED3_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED3_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED3_TOGGLE;
        dumb_delay(DELAY_LONG);
#endif
#ifdef LED4_IS_PRESENT
        LED4_ON;
        dumb_delay(DELAY_LONG);
        LED4_OFF;
        dumb_delay(DELAY_LONG);
        LED4_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED4_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED4_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED4_TOGGLE;
        dumb_delay(DELAY_LONG);
#endif
#ifdef LED5_IS_PRESENT
        LED5_ON;
        dumb_delay(DELAY_LONG);
        LED5_OFF;
        dumb_delay(DELAY_LONG);
        LED5_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED5_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED5_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED5_TOGGLE;
        dumb_delay(DELAY_LONG);
#endif
#ifdef LED6_IS_PRESENT
        LED6_ON;
        dumb_delay(DELAY_LONG);
        LED6_OFF;
        dumb_delay(DELAY_LONG);
        LED6_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED6_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED6_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED6_TOGGLE;
        dumb_delay(DELAY_LONG);
#endif
#ifdef LED7_IS_PRESENT
        LED7_ON;
        dumb_delay(DELAY_LONG);
        LED7_OFF;
        dumb_delay(DELAY_LONG);
        LED7_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED7_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED7_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED7_TOGGLE;
        dumb_delay(DELAY_LONG);
#endif
    }

    puts("Mapping each LED to a button (if available)");

    while (1) {
#if defined(LED0_PIN) && defined(BTN0_PIN)
        gpio_write(LED0_PIN, gpio_read(BTN0_PIN));
#endif
#if defined(LED1_PIN) && defined(BTN1_PIN)
        gpio_write(LED1_PIN, gpio_read(BTN1_PIN));
#endif
#if defined(LED2_PIN) && defined(BTN2_PIN)
        gpio_write(LED2_PIN, gpio_read(BTN2_PIN));
#endif
#if defined(LED3_PIN) && defined(BTN3_PIN)
        gpio_write(LED3_PIN, gpio_read(BTN3_PIN));
#endif
#if defined(LED4_PIN) && defined(BTN4_PIN)
        gpio_write(LED4_PIN, gpio_read(BTN4_PIN));
#endif
#if defined(LED5_PIN) && defined(BTN5_PIN)
        gpio_write(LED5_PIN, gpio_read(BTN5_PIN));
#endif
#if defined(LED6_PIN) && defined(BTN6_PIN)
        gpio_write(LED6_PIN, gpio_read(BTN6_PIN));
#endif
#if defined(LED7_PIN) && defined(BTN7_PIN)
        gpio_write(LED7_PIN, gpio_read(BTN7_PIN));
#endif
    }

    return 0;
}
