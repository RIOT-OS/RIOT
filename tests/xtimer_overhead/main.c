/*
 * Copyright (C) 2020 Kaspar Schleiser <kaspar@schleiser.de>
 *               2020 Freie Universität Berlin
 *               2020 Inria
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
 * @brief       xtimer overhead test application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "xtimer.h"

#define BASE    1000
#define SAMPLES 1024

int32_t xtimer_overhead(uint32_t base);

int main(void)
{
    uint32_t total = 0;

    int32_t min = INT32_MAX;
    int32_t max = INT32_MIN;

    unsigned n = SAMPLES;
    while (n--) {
        int32_t overhead = xtimer_overhead(BASE);
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
