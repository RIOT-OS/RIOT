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

#include "board.h"
#include "periph_conf.h"

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
    int numof = 0;

    /* get the number of available LED's and turn them all off*/
#ifdef LED0_ON
    ++numof;
    LED0_OFF;
#endif
#ifdef LED1_ON
    ++numof;
    LED1_OFF;
#endif
#ifdef LED2_ON
    ++numof;
    LED2_OFF;
#endif
#ifdef LED3_ON
    ++numof;
    LED3_OFF;
#endif
#ifdef LED4_ON
    ++numof;
    LED4_OFF;
#endif
#ifdef LED5_ON
    ++numof;
    LED5_OFF;
#endif
#ifdef LED6_ON
    ++numof;
    LED6_OFF;
#endif
#ifdef LED7_ON
    ++numof;
    LED7_OFF;
#endif

    puts("On-board LED test\n");
    /* cppcheck-suppress knownConditionTrueFalse
     * (reason: board-dependent ifdefs) */
    if (numof == 0) {
        puts("NO LEDs AVAILABLE");
    }
    else {
        printf("Available LEDs: %i\n\n", numof);
        puts("Will now light up each LED once short and twice long in a loop");
    }

    while (1) {
#ifdef LED0_ON
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
#ifdef LED1_ON
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
#ifdef LED2_ON
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
#ifdef LED3_ON
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
#ifdef LED4_ON
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
#ifdef LED5_ON
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
#ifdef LED6_ON
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
#ifdef LED7_ON
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

    return 0;
}
