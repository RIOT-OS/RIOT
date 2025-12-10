/*
 * SPDX-FileCopyrightText: 2022 SSV Software Systems GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Benchmarks ztimer_acquire() and ztimer_release()
 *
 * @author      Juergen Fitschen <me@jue.yt>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>
#include "ztimer.h"
#include "ztimer/config.h"
#include "periph/timer.h"
#include "periph_conf.h"

#if IS_ACTIVE(MODULE_ZTIMER_SEC)
#  define CLOCK ZTIMER_SEC
#elif IS_ACTIVE(MODULE_ZTIMER_MSEC)
#  define CLOCK ZTIMER_MSEC
#elif IS_ACTIVE(MODULE_ZTIMER_USEC)
#  define CLOCK ZTIMER_USEC
#  define CLOCK_IS_ZTIMER_USEC
#else
#  error "No ztimer clock selected!"
#endif

#ifndef COMPARE_TIMER_DEV
#  if IS_ACTIVE(MODULE_ZTIMER_PERIPH_TIMER)
#    if CONFIG_ZTIMER_USEC_DEV == TIMER_DEV(0)
#      define COMPARE_TIMER_DEV TIMER_DEV(1)
#    else
#      define COMPARE_TIMER_DEV TIMER_DEV(0)
#    endif
#  else
#    define COMPARE_TIMER_DEV TIMER_DEV(0)
#  endif
#endif

#ifndef COMPARE_TIMER_FREQ
#  define COMPARE_TIMER_FREQ 1000000U
#endif

static inline uint32_t bench_start(uint32_t adjust)
{
    return timer_read(COMPARE_TIMER_DEV) + adjust;
}

static inline uint32_t bench_finish(const char *name, uint32_t start)
{
    uint32_t stop = timer_read(COMPARE_TIMER_DEV);
    uint32_t diff = stop - start;
    if (name) {
        printf(" - %s took %"PRIu32" ticks\n", name, diff);
    }
    return diff;
}

static inline uint32_t bench_overhead(void) {
    uint32_t start = bench_start(0);
    uint32_t diff = bench_finish(NULL, start);
    return diff;
}

int main(void)
{
    uint32_t start, adjust;
    uint32_t first_acquire, second_acquire;
    uint32_t poweron_diff;

    timer_init(COMPARE_TIMER_DEV, COMPARE_TIMER_FREQ, NULL, NULL);

    adjust = bench_overhead();

    printf("Compare timer frequency: %"PRIu32"Hz\n", (uint32_t) COMPARE_TIMER_FREQ);
    printf("Compare timer overhead: %"PRIu32" ticks (will be taken into account)\n", adjust);

    puts("---");

    puts("Benchmarking:");

    start = bench_start(adjust);
    ztimer_acquire(CLOCK);
    first_acquire = bench_finish("ztimer_acquire() on inactive clock", start);

    start = bench_start(adjust);
    ztimer_acquire(CLOCK);
    second_acquire = bench_finish("ztimer_acquire() on active clock", start);

    start = bench_start(adjust);
    ztimer_release(CLOCK);
    bench_finish("ztimer_release() with users left", start);

    start = bench_start(adjust);
    ztimer_release(CLOCK);
    bench_finish("ztimer_release() with no users left", start);

    puts("---");
    poweron_diff = first_acquire - second_acquire;

#if defined(CLOCK_IS_ZTIMER_USEC) && COMPARE_TIMER_FREQ == 1000000U
    printf("Add this to you board.h:\n");
    printf("   #define CONFIG_ZTIMER_USEC_ADJUST_CLOCK_START %"PRIu32"\n", poweron_diff);
#else
    printf("If the ztimer clock runs with with a frequency of %"PRIu32"Hz, set\n",
           (uint32_t) COMPARE_TIMER_FREQ);
    printf("   clock->adjust_clock_start = %"PRIu32"\n", poweron_diff);
#endif

    return 0;
}
