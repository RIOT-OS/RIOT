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
 * @brief       xtimer_now64 continuity test application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>

#include "xtimer.h"

#define ITERATIONS (100000LU)
#define MAXDIFF 1000

int main(void)
{
    uint32_t n = ITERATIONS;
    uint64_t now;
    uint64_t before = xtimer_now64();

    while(--n) {
        now = xtimer_now64();
        if ((now-before) > MAXDIFF) {
            puts("TEST FAILED.");
            break;
        }
        before = now;
    }

    if (!n) {
        puts("TEST SUCCESSFUL.");
    }

    return 0;
}
