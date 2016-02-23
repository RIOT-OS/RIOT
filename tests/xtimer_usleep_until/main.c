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
    puts("xtimer_usleep_until test application.\n");

    uint32_t interval = NUMOF;
    int32_t max_diff = INT32_MIN;
    int32_t min_diff = INT32_MAX;

    for (int i = 0; i < NUMOF; i++) {
        printf("Testing interval %u... (now=%"PRIu32")\n", (unsigned)interval, xtimer_now());
        uint32_t last_wakeup = xtimer_now();
        uint32_t before = last_wakeup;
        xtimer_usleep_until(&last_wakeup, (unsigned)interval);
        res[i] = (xtimer_now()-before)-interval;
        interval -= 1;
    }

    for (int i = 0; i < NUMOF; i++) {
        printf("%4d diff=%"PRIi32"\n", NUMOF-i, res[i]);

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

    printf("\nTest complete.\n");
    return 0;
}
