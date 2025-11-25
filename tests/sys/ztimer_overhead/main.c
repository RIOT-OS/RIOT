/*
 * Copyright (C) 2019 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     test
 * @{
 *
 * @file
 * @brief       ztimer overhead test application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "board.h"
#include "ztimer.h"
#include "ztimer/overhead.h"

#define BASE    1000
#define SAMPLES 1024

static int32_t _ztimer_usec_overhead(unsigned samples, unsigned base,
                                     int32_t (*overhead_fn)(ztimer_clock_t *clock, uint32_t base))
{
    uint32_t total = 0;

    int32_t min = INT32_MAX;
    int32_t max = INT32_MIN;

    unsigned n = samples;

    while (n--) {
        int32_t overhead = overhead_fn(ZTIMER_USEC, base);
        total += labs(overhead);
        if (overhead < min) {
            min = overhead;
        }
        else if (overhead > max) {
            max = overhead;
        }
    }

    printf("min=%" PRIi32 " max=%" PRIi32 " avg_diff=%" PRIi32 "\n", min, max,
           (total / SAMPLES));

    return min;
}

int main(void)
{
    /* unset configured adjustment */
    printf("ZTIMER_USEC auto_adjust params:\n");
    printf("    ZTIMER_USEC->adjust_set = %" PRIu16 "\n", ZTIMER_USEC->adjust_set);
    ZTIMER_USEC->adjust_set = 0;
    printf("    ZTIMER_USEC->adjust_sleep = %" PRIu16 "\n", ZTIMER_USEC->adjust_sleep);
    ZTIMER_USEC->adjust_sleep = 0;
    printf("ZTIMER_USEC auto_adjust params cleared\n");

    printf("zitmer_overhead_set...\n");
    ZTIMER_USEC->adjust_set = _ztimer_usec_overhead(SAMPLES, BASE, ztimer_overhead_set);
    printf("zitmer_overhead_sleep...\n");
    ZTIMER_USEC->adjust_sleep = _ztimer_usec_overhead(SAMPLES, BASE, ztimer_overhead_sleep);
    printf("ZTIMER_USEC adjust params for %s:\n", RIOT_BOARD);
    printf("    CONFIG_ZTIMER_USEC_ADJUST_SET    %" PRIi16 "\n", ZTIMER_USEC->adjust_set);
    printf("    CONFIG_ZTIMER_USEC_ADJUST_SLEEP  %" PRIi16 "\n", ZTIMER_USEC->adjust_sleep);

    return 0;
}
