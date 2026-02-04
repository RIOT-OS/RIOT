/*
 * SPDX-FileCopyrightText: 2020 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-FileCopyrightText: 2020 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2020 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
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
