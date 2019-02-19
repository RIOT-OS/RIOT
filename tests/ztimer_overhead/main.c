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
#include <inttypes.h>

#include "ztimer.h"
#include "ztimer/overhead.h"

#define BASE    1000
#define SAMPLES 1024

int main(void)
{
    uint32_t total = 0;

    uint16_t min = 0xFFFF;
    uint16_t max = 0;

    /* unset configured adjustment */
    /* ZTIMER_USEC->adjust = 0; */

    unsigned n = SAMPLES;
    while (n--) {
        unsigned overhead = ztimer_overhead(ZTIMER_USEC, BASE);
        total += overhead;
        if (overhead < min) {
            min = overhead;
        }
        else if (overhead > max) {
            max = overhead;
        }
    }

    printf("min=%u max=%u avg=%" PRIu32 "\n", min, max, (total / SAMPLES));

    return 0;
}
