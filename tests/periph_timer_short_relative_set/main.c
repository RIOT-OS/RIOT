/*
 * Copyright (C) 2019 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @brief       Peripheral timer test application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "board.h"
#include "thread.h"
#include "thread_flags.h"

#include "periph/timer.h"

#ifndef TEST_TIMER_DEV
# include "xtimer.h"
# define TEST_TIMER_DEV      XTIMER_DEV
# define TEST_TIMER_FREQ     XTIMER_HZ
# define TEST_TIMER_WIDTH    XTIMER_WIDTH
#else
# ifndef TEST_TIMER_FREQ
#  define TEST_TIMER_FREQ     (1000000LU)
# endif
#endif

#ifndef TEST_MAX_DIFF
#define TEST_MAX_DIFF   (1000LU)
#endif

#if TEST_TIMER_WIDTH == 32
# define TEST_TIMER_MAX      (0xFFFFFFFFLU)
#else
# define TEST_TIMER_MAX      ((1UL << TEST_TIMER_WIDTH) - 1)
#endif

static void cb(void *arg, int chan)
{
    (void)chan;
    thread_flags_set(arg, 1);
}

int main(void)
{
    puts("\nTest for peripheral TIMER short timer_set()\n");

    printf("This test tries timer_set() with decreasing intervals down to 0.\n"
           "You should see lines like 'interval <n> ok', followed by a success"
           " message.\n"
           "On failure, this test prints an error message.\n\n");

    printf("testing periph_timer %u, freq %lu\n", TEST_TIMER_DEV, TEST_TIMER_FREQ);
    timer_init(TEST_TIMER_DEV, TEST_TIMER_FREQ, cb, (thread_t *)sched_active_thread);

    uint32_t interval = 100;
    while (interval--) {
        uint32_t before = timer_read(TEST_TIMER_DEV);
        timer_set(TEST_TIMER_DEV, 0, interval);
        while(!thread_flags_clear(1)) {
            uint32_t diff = (timer_read(TEST_TIMER_DEV) - before)
                            & TEST_TIMER_MAX;
            if (diff > TEST_MAX_DIFF) {
                printf("ERROR: too long delay, aborted after %" PRIu32
                        " (TEST_MAX_DIFF=%lu)\n"
                        "TEST FAILED\n",
                        diff, TEST_MAX_DIFF);
                while(1) {}
            }
        }
        printf("interval %" PRIu32 " ok\n", interval);
    }

    puts("\nTEST SUCCEEDED");

    return 0;
}
