/*
 * SPDX-FileCopyrightText: 2020 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       testing xtimer_now_usec function with irq disabled
 *
 *
 * @author      Julian Holzwarth <julian.holzwarth@fu-berlin.de>
 *
 */

#include <stdio.h>
#include "xtimer.h"
#include "irq.h"

#define TEST_COUNT 4

int main(void)
{
    if (XTIMER_WIDTH == 32) {
        puts("Nothing to do for 32 bit timers.\n");
    }
    else {
        puts("xtimer_now_irq test application.\n");
        uint8_t count = TEST_COUNT;
        while (count) {
            unsigned int state = irq_disable();
            uint32_t t1 = xtimer_now_usec();
            xtimer_spin(xtimer_ticks((uint32_t)(~XTIMER_MASK) / 2));
            uint32_t t2 = xtimer_now_usec();
            irq_restore(state);
            if (t2 < t1) {
                puts("ERROR: wrong time with interrupts disabled");
                return -1;
            }
            puts("OK");
            count --;
        }
    }

    puts("SUCCESS");
    return 0;
}
