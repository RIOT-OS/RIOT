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

#include "ztimer.h"
#include "ztimer/overhead.h"

#define BASE    1000
#define SAMPLES 1024

int main(void)
{
    uint32_t total = 0;

    int32_t min = INT32_MAX;
    int32_t max = INT32_MIN;

    /* unset configured adjustment */
    /* ZTIMER_USEC->adjust_set = 0; */
    printf("ZTIMER_USEC->adjust_set = %" PRIu16 "\n", ZTIMER_USEC->adjust_set);

    unsigned n = SAMPLES;
    while (n--) {
        int32_t overhead = ztimer_overhead(ZTIMER_USEC, BASE);
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

    return 0;
}
