/*
 * SPDX-FileCopyrightText: 2019 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
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
#include "periph_cpu.h"
#include "macros/units.h"
#include "thread.h"
#include "thread_flags.h"

#include "periph/timer.h"

#ifndef TEST_TIMER_DEV
# ifdef XTIMER_DEV
#  define TEST_TIMER_DEV XTIMER_DEV
# else
#  define TEST_TIMER_DEV TIMER_DEV(0)
# endif
#endif

#ifndef TEST_TIMER_MAX
# ifdef XTIMER_WIDTH
#  define TEST_TIMER_MAX   ((1UL << XTIMER_WIDTH) - 1)
# elif TIMER_0_MAX_VALUE
#  define TEST_TIMER_MAX   TIMER_0_MAX_VALUE
# else
#  error "Expected either TEST_TIMER_MAX, XTIMER_WIDTH or TIMER_0_MAX_VALUE"
# endif
#endif

#ifndef TEST_MAX_DIFF
#define TEST_MAX_DIFF   (1000LU)
#endif

static uint32_t query_freq(void)
{
#ifdef TEST_TIMER_FREQ
    return TEST_TIMER_FREQ;
#elif MODULE_PERIPH_TIMER_QUERY_FREQS
    return timer_query_freqs(TEST_TIMER_DEV, 0);
#elif XTIMER_HZ
    return XTIMER_HZ;
#else
    #error "Expected either TEST_TIMER_FREQ, MODULE_PERIPH_TIMER_QUERY_FREQS or XTIMER_HZ"
#endif
}

static void cb(void *arg, int chan)
{
    (void)chan;
    thread_flags_set(arg, 1);
}

int main(void)
{
    uint32_t test_timer_freq = query_freq();
    puts("\nTest for peripheral TIMER short timer_set()\n");

    printf("This test tries timer_set() with decreasing intervals down to 0.\n"
           "You should see lines like 'interval <n>: OK', followed by a success"
           " message.\n"
           "On failure, this test prints an error message.\n\n");

    printf("testing periph_timer %u, freq %" PRIu32 ", max = 0x%lx\n", TEST_TIMER_DEV, test_timer_freq,
           (unsigned long) TEST_TIMER_MAX);

    if (timer_init(TEST_TIMER_DEV, test_timer_freq, cb, thread_get_active()) != 0) {
        puts("ERROR: timer_init() failed.\nTEST FAILED");
        return EXIT_FAILURE;
    }

    uint32_t interval = 16;
    const unsigned max_repetitions = 128;
    while (interval--) {
        for (unsigned rep = 0; rep < max_repetitions; rep++) {
            uint32_t before = timer_read(TEST_TIMER_DEV);
            timer_set(TEST_TIMER_DEV, 0, interval);
            while (!thread_flags_clear(1)) {
                uint32_t diff = (timer_read(TEST_TIMER_DEV) - before) & TEST_TIMER_MAX;
                if (diff > TEST_MAX_DIFF) {
                    printf("ERROR: too long delay, aborted after %" PRIu32
                           " (TEST_MAX_DIFF=%lu) on repetition %u\n"
                           "TEST FAILED\n",
                           diff, TEST_MAX_DIFF, rep);
                    return EXIT_FAILURE;
                }
            }
        }
        printf("interval %" PRIu32 ": OK\n", interval);
    }

    puts("\nTEST SUCCEEDED");

    return EXIT_SUCCESS;
}
