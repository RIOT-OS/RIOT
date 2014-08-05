/*
 * Copyright (C) 2014  René Kijewski  <rene.kijewski@fu-berlin.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


/**
 * @ingroup   tests
 * @{
 *
 * @file
 * @brief     Measure the speed of the function in bitarithm.c
 *
 * @author    René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "bitarithm.h"
#include "hwtimer.h"

#define TIMEOUT_S (5)
#define TIMEOUT_US (TIMEOUT_S * 1000 * 1000)
#define TIMEOUT (HWTIMER_TICKS(TIMEOUT_US))
#define PER_ITERATION (4)

static void callback(void *done_)
{
    volatile int *done = done_;
    *done = 1;
}

static void run_test(const char *name, unsigned (*test)(unsigned))
{
    volatile int done = 0;
    unsigned i = 0;
    unsigned long count = 0;

    hwtimer_set(TIMEOUT, callback, (void *) &done);
    do {
        if (i++ == -1u) {
            i = 1;
        }

        for (unsigned j = 0; j < PER_ITERATION; ++j) {
            volatile unsigned r;
            r = test(i);
            (void) r;
            r = test(-1u - i);
            (void) r;
            r = test(~i);
            (void) r;
            r = test(~(-1u - i));
            (void) r;
        }

        ++count;
    } while (done == 0);

    printf("+ %s: %lu iterations per second\r\n", name, (4*PER_ITERATION) * count / TIMEOUT_S);
}

#define run_test(test) run_test(#test, test)

int main(void)
{
    printf("Start.\r\n");

    run_test(bitarithm_msb);
    run_test(bitarithm_lsb);
    run_test(bitarithm_bits_set);

    printf("Done.\r\n");
    return 0;
}
