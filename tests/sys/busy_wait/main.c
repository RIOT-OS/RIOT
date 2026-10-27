/*
 * SPDX-FileCopyrightText: 2024 ML!PA Consulting GmbH
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief   Busy Wait loop Test Application
 *
 *          This can be used to determine `CPU_CYCLES_PER_LOOP` by
 *          comparing the time the busy wait loop took with the
 *          actual µsec timer. A hint is printed if a different value
 *          would match better, and the test fails if the duration is
 *          off by more than a large factor.
 *
 *          It also checks that the compiler does not unroll the busy
 *          wait loop for small constant loop counts, which would
 *          remove the delay entirely.
 *
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include <stdbool.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include "busy_wait.h"
#include "macros/utils.h"
#include "ztimer/stopwatch.h"

/* Number of repetitions of a small busy wait loop for the unrolling check.
 * Scaled with the core clock so that each measurement takes about the
 * same time on every CPU: long enough for the timer resolution, but short
 * enough to be rarely disturbed. */
#define SMALL_LOOPS_REPEAT  ((uint32_t)(CLOCK_CORECLOCK / 10000))

/* Take the fastest run. Interrupts slow the measurement down, nothing would
 * speed it up. */
#define SMALL_LOOPS_RUNS    (25U)

/* Number of attempts before considering failure. */
#define SMALL_LOOPS_ATTEMPTS (5U)

/* The measured duration of `busy_wait_us()` may deviate from the requested
 * one by up to this factor. This is very generous and only catches grossly
 * wrong values of CLOCK_CORECLOCK or CPU_CYCLES_PER_LOOP. */
#define INTERVAL_MAX_FACTOR (10U)

/* Shorter intervals are not checked due to overhead dominating. */
#define INTERVAL_CHECK_MIN_USEC (1000U)

static const unsigned _intervals_usec[] = { 10, 100, 1000, 10000 };

static inline uint32_t _measure_interval(ztimer_stopwatch_t *clock,
                                         unsigned usec)
{
    uint32_t usec_real;

    printf("waiting for %u µs…\n", usec);
    ztimer_stopwatch_start(clock);
    busy_wait_us(usec);
    usec_real = ztimer_stopwatch_measure(clock);
    printf("took %" PRIu32 " µs (diff: %" PRId32 " µs)\n",
           usec_real, (int32_t)(usec_real - usec));

    return usec_real;
}

static inline uint32_t _measure_small_loops(ztimer_stopwatch_t *clock,
                                            unsigned loops)
{
    ztimer_stopwatch_start(clock);
    for (uint32_t i = 0; i < SMALL_LOOPS_REPEAT; i++) {
        busy_wait(loops);
    }
    return ztimer_stopwatch_measure(clock);
}

static inline bool _check_unrolling(ztimer_stopwatch_t *clock, unsigned loops)
{
    /* Reference: the loop count is read from a volatile at runtime, so the
     * compiler does not know it and cannot unroll the loop. */
    volatile unsigned loops_runtime = loops;

    for (unsigned attempt = 0; attempt < SMALL_LOOPS_ATTEMPTS; attempt++) {
        uint32_t usec_const = UINT32_MAX;
        uint32_t usec_runtime = UINT32_MAX;

        /* call with constant and volatile value */
        for (unsigned run = 0; run < SMALL_LOOPS_RUNS; run++) {
            uint32_t usec_tmp = _measure_small_loops(clock, loops);
            usec_const = MIN(usec_const, usec_tmp);
            usec_tmp = _measure_small_loops(clock, loops_runtime);
            usec_runtime = MIN(usec_runtime, usec_tmp);
        }

        printf("%" PRIu32 " x busy_wait(%u): constant %" PRIu32 " µs, "
               "runtime %" PRIu32 " µs\n",
               SMALL_LOOPS_REPEAT, loops, usec_const, usec_runtime);

        /* Calling `busy_wait()` with a constant can be a bit faster than with a
         * runtime variable. With unrolling, it would be about 50%, so add some
         * headroom. */
        if (usec_const * 100 >= usec_runtime * 75) {
            return true;
        }
    }

    puts("busy_wait() loop was (probably) unrolled!");
    return false;
}

/* `flatten` inlines all calls including busy_wait(), so that the loop
 * count is a compile time constant inside of busy_wait() like at
 * a typical call site. Otherwise the compiler may call an out-of-line
 * copy of busy_wait() and the unrolling would not be tested at all. */
__attribute__((flatten, unused))
static bool _check_small_loops(ztimer_stopwatch_t *clock)
{
    bool ok = true;

    ok &= _check_unrolling(clock, 1);
    ok &= _check_unrolling(clock, 2);
    ok &= _check_unrolling(clock, 4);
    ok &= _check_unrolling(clock, 8);

    return ok;
}

int main(void)
{
    ztimer_stopwatch_t clock;
    uint32_t usec_real[ARRAY_SIZE(_intervals_usec)];

    ztimer_stopwatch_init(ZTIMER_USEC, &clock);

    for (unsigned i = 0; i < ARRAY_SIZE(_intervals_usec); i++) {
        usec_real[i] = _measure_interval(&clock, _intervals_usec[i]);
    }

#ifdef CPU_NATIVE
    /* The timing on native is unreliable:
     * - CLOCK_CORECLOCK != real host CPU clock
     * - Scheduler preemption
     * - Loop execution time can depend on memory alignment
     * - Building with -Og prevents loop unrolling */
    (void)usec_real;
    puts("skipping timing test on native");
    puts("[SUCCESS]");
#else
    bool ok = true;

    for (unsigned i = 0; i < ARRAY_SIZE(_intervals_usec); i++) {
        uint32_t usec = _intervals_usec[i];

        if (usec < INTERVAL_CHECK_MIN_USEC) {
            continue;
        }
        if ((usec_real[i] * INTERVAL_MAX_FACTOR < usec) ||
            (usec_real[i] > usec * INTERVAL_MAX_FACTOR)) {
            printf("busy_wait_us(%" PRIu32 ") is off by more than "
                   "a factor of %u!\n", usec, INTERVAL_MAX_FACTOR);
            ok = false;
        }
    }

    /* The longest interval gives the most accurate estimate. */
    const unsigned last = ARRAY_SIZE(_intervals_usec) - 1;
    uint32_t cycles = ((uint32_t)CPU_CYCLES_PER_LOOP * usec_real[last] +
                       _intervals_usec[last] / 2) / _intervals_usec[last];
    cycles = MAX(cycles, 1);
    if (cycles != CPU_CYCLES_PER_LOOP) {
        printf("info: CPU_CYCLES_PER_LOOP is %u, but %" PRIu32 " would "
               "match the measurement better\n",
               (unsigned)CPU_CYCLES_PER_LOOP, cycles);
    }

    ok &= _check_small_loops(&clock);
    puts(ok ? "[SUCCESS]" : "[FAILED]");
#endif

    return 0;
}
