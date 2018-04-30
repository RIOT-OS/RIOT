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

#define NUMOF   (3 * XTIMER_PERIODIC_SPIN)


int32_t res[NUMOF];

int main(void)
{
    printf("xtimer_periodic_wakeup test application runs %d\n.", NUMOF );

    uint32_t interval;
    int32_t max_diff = INT32_MIN;
    int32_t min_diff = INT32_MAX;

    puts("\nxtimer_periodic_wakeup test spinning.");
    /* Testing
     * 1. spinning */
    for (int i = 2 * XTIMER_PERIODIC_SPIN; i >= 0; i--) {
        interval = _xtimer_usec_from_ticks(i);
        xtimer_ticks32_t now = xtimer_now();
        printf("Testing interval %" PRIu32 " ticks (now=%" PRIu32 ")\n",
               _xtimer_ticks_from_usec(i), now.ticks32);
        xtimer_ticks32_t last_wakeup = xtimer_now();
        xtimer_ticks32_t before = last_wakeup;
        xtimer_periodic_wakeup(&last_wakeup, interval);
        now = xtimer_now();
        res[i] = (now.ticks32 - before.ticks32) - _xtimer_ticks_from_usec(interval);
    }

    /* Testing
     * 2. interval with timer interrupt and relative time stamp
     * 3. interval with timer interrupt and absolute time stamp
     */
    puts("xtimer_periodic_wakeup test timer interrupt.");
    for (int i = XTIMER_PERIODIC_SPIN - 1; i >= 0; i--) {
        interval = _xtimer_usec_from_ticks(XTIMER_PERIODIC_RELATIVE - XTIMER_PERIODIC_SPIN / 2 + i);
        xtimer_ticks32_t now = xtimer_now();
        printf("Testing interval %" PRIu32 " ticks (now=%" PRIu32 ")\n",
               _xtimer_ticks_from_usec(interval), now.ticks32);
        xtimer_ticks32_t last_wakeup = xtimer_now();
        xtimer_ticks32_t before = last_wakeup;
        xtimer_periodic_wakeup(&last_wakeup, interval);
        now = xtimer_now();
        res[2 * XTIMER_PERIODIC_SPIN + i] = (now.ticks32 - before.ticks32)
                                            - _xtimer_ticks_from_usec(interval);
    }
    for (int i = 0; i < NUMOF; i++) {

        if (res[i] > max_diff) {
            max_diff = res[i];
        }
        if (res[i] < min_diff) {
            min_diff = res[i];
        }

        if (i == 0) {
            puts("\n Results xtimer_periodic_wakeup test spinning.");
        }
        else if (i == 2 * XTIMER_PERIODIC_SPIN) {
            puts("\n Results xtimer_periodic_wakeup test timer interrupt.");
        }

        if (i < 2 * XTIMER_PERIODIC_SPIN) {
            interval = _xtimer_ticks_from_usec(_xtimer_usec_from_ticks(i));
            printf("Interval %" PRIu32 " ticks diff=%" PRIi32 "\n", interval, res[i]);
        }
        else {
            interval = XTIMER_PERIODIC_RELATIVE - XTIMER_PERIODIC_SPIN / 2
                       + i - 2 * XTIMER_PERIODIC_SPIN;
            printf("Interval %" PRIu32 " ticks diff=%" PRIi32 "\n", interval, res[i]);
        }
    }
    printf("\nMin/max error: %" PRId32 "/%" PRId32 "\n", min_diff, max_diff);

    if (min_diff < -1000 || max_diff > 1000) {
        puts("too large difference.\n");
        puts("[FAILED]");
        return 1;
    }

    printf("\nTest complete.\n");
    puts("[SUCCESS]");
    return 0;
}
