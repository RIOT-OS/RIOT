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
 *          actual µsec timer. The test fails if the duration is off
 *          by more than a large factor. Since the duration of a loop
 *          iteration can depend on the alignment of the loop, it is
 *          measured at different offsets and the minimum, the maximum
 *          and a recommended value are printed.
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
#include "compiler_hints.h"

#include "macros/utils.h"
#include "ztimer/stopwatch.h"

/* For the unrolling check, busy_wait() is called SMALL_LOOPS_REPEAT times in
 * a row. If a single call takes c CPU clock cycles, all calls together take
 * SMALL_LOOPS_REPEAT * c / CLOCK_CORECLOCK seconds, which is
 * c * SMALL_LOOPS_USEC_PER_CPU_CYCLE µs. So the measured time in µs divided
 * by this value is the number of CPU clock cycles per call, independent of
 * the CPU speed. 100 µs is long enough for the timer resolution, but short
 * enough to be rarely disturbed. */
#define SMALL_LOOPS_USEC_PER_CPU_CYCLE  (100U)

/* Number of repetitions of busy_wait() for the unrolling check */
#define SMALL_LOOPS_REPEAT  ((uint32_t)(CLOCK_CORECLOCK / \
                                        (US_PER_SEC / SMALL_LOOPS_USEC_PER_CPU_CYCLE)))

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

/* Like SMALL_LOOPS_USEC_PER_CPU_CYCLE, but for a single loop iteration with
 * OFFSET_LOOPS iterations. */
#define OFFSET_USEC_PER_CPU_CYCLE  (1000U)

/* Number of loop iterations for the measurement at each offset */
#define OFFSET_LOOPS    ((unsigned)(CLOCK_CORECLOCK / \
                                    (US_PER_SEC / OFFSET_USEC_PER_CPU_CYCLE)))

/* Take the fastest run at each offset, see SMALL_LOOPS_RUNS */
#define OFFSET_RUNS     (5U)

/* The duration of a loop iteration can depend on its alignment, e.g. due to
 * flash prefetch buffers or cache lines of up to 32 bytes. Each copy of
 * busy_wait() starts at a 32 byte boundary and is shifted by one more nop.
 * The distance between the function start and the loop is unknown, but the
 * same for all copies, so 16 nops of 2 bytes cover all offsets. The alignment
 * is done for the function and not with `.balign` inside of it, as the padding
 * would be executed and it is not filled with nops on all architectures.
 * `flatten` inlines busy_wait(), otherwise all copies would call the same
 * out-of-line loop. */
#define BUSY_WAIT_AT_OFFSET(n)                                          \
    __attribute__((noinline, flatten, aligned(32)))                     \
    static void _busy_wait_at_offset_##n(unsigned loops)                \
    {                                                                   \
        __asm__ __volatile__ (".rept " #n "\nnop\n.endr");               \
        busy_wait(loops);                                               \
    }

BUSY_WAIT_AT_OFFSET(0)
BUSY_WAIT_AT_OFFSET(1)
BUSY_WAIT_AT_OFFSET(2)
BUSY_WAIT_AT_OFFSET(3)
BUSY_WAIT_AT_OFFSET(4)
BUSY_WAIT_AT_OFFSET(5)
BUSY_WAIT_AT_OFFSET(6)
BUSY_WAIT_AT_OFFSET(7)
BUSY_WAIT_AT_OFFSET(8)
BUSY_WAIT_AT_OFFSET(9)
BUSY_WAIT_AT_OFFSET(10)
BUSY_WAIT_AT_OFFSET(11)
BUSY_WAIT_AT_OFFSET(12)
BUSY_WAIT_AT_OFFSET(13)
BUSY_WAIT_AT_OFFSET(14)
BUSY_WAIT_AT_OFFSET(15)

MAYBE_UNUSED
static void (* const _busy_wait_at_offset[])(unsigned) = {
    _busy_wait_at_offset_0,  _busy_wait_at_offset_1,
    _busy_wait_at_offset_2,  _busy_wait_at_offset_3,
    _busy_wait_at_offset_4,  _busy_wait_at_offset_5,
    _busy_wait_at_offset_6,  _busy_wait_at_offset_7,
    _busy_wait_at_offset_8,  _busy_wait_at_offset_9,
    _busy_wait_at_offset_10, _busy_wait_at_offset_11,
    _busy_wait_at_offset_12, _busy_wait_at_offset_13,
    _busy_wait_at_offset_14, _busy_wait_at_offset_15,
};

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
        /* Like busy_wait(): keep the loop for loops == 0, avoid unrolling. */
        __asm__ __volatile__ ("" : "+r" (i));
    }
    return ztimer_stopwatch_measure(clock);
}

static inline bool _check_unrolling(ztimer_stopwatch_t *clock, unsigned loops)
{
    for (unsigned attempt = 0; attempt < SMALL_LOOPS_ATTEMPTS; attempt++) {
        uint32_t usec_empty = UINT32_MAX;
        uint32_t usec_busy = UINT32_MAX;

        for (unsigned run = 0; run < SMALL_LOOPS_RUNS; run++) {
            uint32_t usec_tmp = _measure_small_loops(clock, 0);
            usec_empty = MIN(usec_empty, usec_tmp);
            usec_tmp = _measure_small_loops(clock, loops);
            usec_busy = MIN(usec_busy, usec_tmp);
        }

        /* additional time of busy_wait(), see SMALL_LOOPS_USEC_PER_CPU_CYCLE */
        uint32_t usec_diff = (usec_busy > usec_empty) ? usec_busy - usec_empty : 0;

        printf("busy_wait(%u): %" PRIu32 ".%02" PRIu32 " CPU cycles\n", loops,
               usec_diff / SMALL_LOOPS_USEC_PER_CPU_CYCLE,
               usec_diff % SMALL_LOOPS_USEC_PER_CPU_CYCLE);

        /* Each loop iteration takes at least one CPU cycle, typically three
         * or more, while an unrolled loop takes none. Half a cycle per
         * iteration leaves enough headroom to both sides. */
        if (usec_diff * 2 >= loops * SMALL_LOOPS_USEC_PER_CPU_CYCLE) {
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

    uint32_t usec_min = UINT32_MAX;
    uint32_t usec_max = 0;

    for (unsigned n = 0; n < ARRAY_SIZE(_busy_wait_at_offset); n++) {
        uint32_t usec_once = UINT32_MAX;
        uint32_t usec_twice = UINT32_MAX;

        for (unsigned run = 0; run < OFFSET_RUNS; run++) {
            ztimer_stopwatch_start(&clock);
            _busy_wait_at_offset[n](OFFSET_LOOPS);
            uint32_t usec_tmp = ztimer_stopwatch_measure(&clock);
            usec_once = MIN(usec_once, usec_tmp);

            ztimer_stopwatch_start(&clock);
            _busy_wait_at_offset[n](2 * OFFSET_LOOPS);
            usec_tmp = ztimer_stopwatch_measure(&clock);
            usec_twice = MIN(usec_twice, usec_tmp);
        }

        /* The difference is the time of OFFSET_LOOPS iterations without the
         * overhead of the call and the stopwatch, see
         * OFFSET_USEC_PER_CPU_CYCLE. */
        uint32_t usec_diff = (usec_twice > usec_once) ? usec_twice - usec_once : 0;
        usec_min = MIN(usec_min, usec_diff);
        usec_max = MAX(usec_max, usec_diff);
    }

    printf("CPU cycles per loop: min %" PRIu32 ".%02" PRIu32
           ", max %" PRIu32 ".%02" PRIu32 "\n",
           usec_min / OFFSET_USEC_PER_CPU_CYCLE,
           usec_min % OFFSET_USEC_PER_CPU_CYCLE / 10,
           usec_max / OFFSET_USEC_PER_CPU_CYCLE,
           usec_max % OFFSET_USEC_PER_CPU_CYCLE / 10);

    /* Based on the fastest offset, so that busy_wait_us() rather waits too
     * long than too short. */
    uint32_t cycles = (usec_min + OFFSET_USEC_PER_CPU_CYCLE / 2) /
                      OFFSET_USEC_PER_CPU_CYCLE;
    cycles = MAX(cycles, 1);

    ok &= _check_small_loops(&clock);

    printf("\nCPU_CYCLES_PER_LOOP is %u, recommended: %" PRIu32 "\n",
           (unsigned)CPU_CYCLES_PER_LOOP, cycles);

    puts(ok ? "[SUCCESS]" : "[FAILED]");
#endif

    return 0;
}
