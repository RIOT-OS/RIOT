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

#include "board.h"
#include "xtimer.h"

#define WAIT_INTERVAL       (500 * MS_IN_USEC)

int main(void)
{
    puts("On-board LED test\n");
    puts("You should now see the 'red' and the 'green' LED lighting up in a\n"
         "500ms interval, alternating of each other.");

    /* turn off all LEDs */
    LED_RED_OFF;
    LED_GREEN_OFF;

    while (1) {
        LED_RED_ON;
        puts("LED_RED_ON");
        xtimer_usleep(WAIT_INTERVAL);
        LED_RED_OFF;
        LED_GREEN_ON;
        puts("LED_GREEN_ON");
        xtimer_usleep(WAIT_INTERVAL);
        LED_GREEN_OFF;

        LED_RED_TOGGLE;
        puts("LED_RED_TOGGLE");
        xtimer_usleep(WAIT_INTERVAL);
        LED_RED_TOGGLE;
        LED_GREEN_TOGGLE;
        puts("LED_GREEN_TOGGLE");
        xtimer_usleep(WAIT_INTERVAL);
        LED_GREEN_TOGGLE;
    }

    return 0;
}
