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

#include "dbgpin.h"
#include "ztimer.h"

#define TICK_MS         5U

int main(void)
{
    printf("Found %i configured debug pin(s)\n", dbgpin_count());

    for (unsigned p = 0; p < dbgpin_count(); p++) {
        printf("Testing pin %u\n", p);
        dbgpin_set(p);
        ztimer_sleep(ZTIMER_MSEC, TICK_MS);
        dbgpin_clear(p);
        ztimer_sleep(ZTIMER_MSEC, TICK_MS);
        dbgpin_toggle(p);
        ztimer_sleep(ZTIMER_MSEC, 2 * TICK_MS);
        dbgpin_toggle(p);
        ztimer_sleep(ZTIMER_MSEC, TICK_MS);
        dbgpin_pulse(p);
        ztimer_sleep(ZTIMER_MSEC, TICK_MS);
        for (unsigned i = 2; i <= 5; i++) {
            dbgpin_signal(p, i);
            ztimer_sleep(ZTIMER_MSEC, TICK_MS);
        }
    }

    puts("Test successful.");

    return 0;
}
