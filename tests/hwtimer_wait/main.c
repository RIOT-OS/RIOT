/*
 * Copyright (C) 2014 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief Hwtimer test application
 *
 * @author      Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <math.h>

#include "board_uart0.h"
#include "posix_io.h"
#include "hwtimer.h"
#include "thread.h"

int main(void)
{
    puts("This is a regression test for a race condition in hwtimer_wait.");
    puts("When the race condition is hit, the timer will wait for a very very long time.");

    int iterations = 10000;
    /* `#define I_CHANGED_START_DURATION 1` or update the constant if
     * you change start_duration */
    int start_duration = 256;

    long duration = iterations * (
            /* geometric series */
#if I_CHANGED_START_DURATION
            (1 - pow(2,(log2(start_duration) + 1)))
            / (1 - 2)
#else
            511.0
#endif
            );
    printf("The test should take about %li sec.\n", (HWTIMER_TICKS_TO_US(duration)/1000000));

    for (unsigned long r = iterations; r > 0; r--) {
        for (unsigned long i = start_duration; i; i = i >> 1) {
            hwtimer_wait(i);
        }
    }
    puts("success");

    return 0;
}
