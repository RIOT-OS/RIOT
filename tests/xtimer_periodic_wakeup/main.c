/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @brief       timer test application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>
#include "xtimer.h"
#include "periph_conf.h"

#ifndef NUMOF
#define NUMOF   (256)
#endif

int32_t res[NUMOF];

int main(void)
{
    puts("xtimer_periodic_wakeup test application.\n");
    puts("[START]: xtimer_periodic_wakeup");

    uint32_t interval = NUMOF;
    int32_t max_diff = INT32_MIN;
    int32_t min_diff = INT32_MAX;

    xtimer_ticks32_t last_wakeup = xtimer_now();
    for (int i = 0; i < NUMOF; i++) {
        xtimer_ticks32_t now = xtimer_now();
        printf("Testing interval %" PRIu32 "... (now=%" PRIu32 ")\n", interval, xtimer_usec_from_ticks(now));
        xtimer_ticks32_t before = last_wakeup;
        xtimer_periodic_wakeup(&last_wakeup, interval);
        xtimer_ticks32_t diff = xtimer_diff(before, last_wakeup);
        if (diff.ticks32 == 0) {
            puts("Test Failed: last_wakeup not updated.");
            return -1;
        }
        now = xtimer_now();
        res[i] = (xtimer_usec_from_ticks(now) - xtimer_usec_from_ticks(before)) - interval;
        interval -= 1;
    }

    for (int i = 0; i < NUMOF; i++) {
        printf("%4d diff=%" PRIi32 "\n", NUMOF - i, res[i]);

        if (res[i] > max_diff) {
            max_diff = res[i];
        }
        if (res[i] < min_diff) {
            min_diff = res[i];
        }
    }
    printf("\nMin/max error: %" PRId32 "/%" PRId32 "\n", min_diff, max_diff);

    if (min_diff < -1000 || max_diff > 1000) {
        puts("too large difference.\n");
        puts("Test Failed.\n");
        return 1;
    }

    /* test expired last_wakeup */
    last_wakeup = xtimer_now();
    xtimer_ticks32_t before = last_wakeup;
    xtimer_usleep(500);
    xtimer_periodic_wakeup(&last_wakeup, 250);
    xtimer_ticks32_t diff = xtimer_diff(before, last_wakeup);
    if (diff.ticks32 == 0) {
        puts("Test Failed: last_wakeup not updated.");
        return -1;
    }

    printf("\nTest complete.\n");
    puts("[SUCCESS]: xtimer_periodic_wakeup");
    return 0;
}
