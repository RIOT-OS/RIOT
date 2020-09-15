/*
 * Copyright (C) 2020 Freie Universität Berlin
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
 * @brief       Test for the dbgpin module
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "xtimer.h"

#define TICK        (5 * US_PER_MS)     /* -> 5ms */

int main(void)
{
    printf("Found %i configured debug pin(s)\n", dbgpin_count());

    for (unsigned p = 0; p < dbgpin_count(); p++) {
        printf("Testing pin %u\n", p);
        dbgpin_set(p);
        xtimer_usleep(TICK);
        dbgpin_clear(p);
        xtimer_usleep(TICK);
        dbgpin_toggle(p);
        xtimer_usleep(2 * TICK);
        dbgpin_toggle(p);
        xtimer_usleep(TICK);
        dbgpin_pulse(p);
        xtimer_usleep(TICK);
        for (unsigned i = 2; i <= 5; i++) {
            dbgpin_signal(p, i);
            xtimer_usleep(TICK);
        }
    }

    puts("Test successful.");

    return 0;
}
