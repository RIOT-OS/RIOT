/*
 * Copyright (C) 2015 Freie Universität Berlin
 *               2018 Eistec AB
 *               2024 HAW Hamburg
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
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *              Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *              Bennet Blischke <bennet.blischke@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "atomic_utils.h"
#include "architecture.h"
#include "clk.h"
#include "mutex.h"
#include "periph/timer.h"
#include "test_utils/expect.h"
#include "time_units.h"

/**
 * @brief   Make sure, the maximum number of timers is defined
 */
#ifndef TIMER_NUMOF
#error "TIMER_NUMOF not defined!"
#endif

/* backward compatibility with legacy drivers */
#if !defined(TIMER_CHANNEL_NUMOF) && !IS_USED(MODULE_PERIPH_TIMER_QUERY_FREQS)
#define TIMER_CHANNEL_NUMOF 10U
#endif

#define CHAN_OFFSET_MS      5U          /* fire channels with 5 ms offset */
/* The minimum timeout to set and still being able to clear the timer
 * before it fires. This should be conservative, as wasting a few milliseconds
 * in the test is less annoying than false test failures */
#define MINIMUM_TIMEOUT_MS  2
#define COOKIE              (100U)      /* for checking if arg is passed */
/* Setting a timer for less than two ticks may cause it to fire right away,
 * e.g. when the timer was about to tick anyway */
#define MINIMUM_TICKS       2

#ifndef TEST_ITERATIONS
#define TEST_ITERATIONS     (10000ul)
#endif

static uint8_t fired;
static uint32_t sw_count;
static uint32_t timeouts[TIMER_CHANNEL_NUMOF];
static unsigned args[TIMER_CHANNEL_NUMOF];

typedef struct {
    unsigned long counter;
    tim_t dev;
    mutex_t mtx;
} test_ctx_t;

static void cb_incr(void *arg, int chan)
{
    (void)chan;
    test_ctx_t *ctx = arg;

    ctx->counter++;
    if (ctx->counter < TEST_ITERATIONS) {
        /* Rescheduling the timer like this will trigger a bug in the lptmr
         * implementation in Kinetis */
        timer_set(ctx->dev, chan, 20000u);
        timer_set(ctx->dev, chan, 0);
    }
    mutex_unlock(&ctx->mtx);
}

static void cb(void *arg, int chan)
{
    timeouts[chan] = sw_count;
    args[chan] = (uintptr_t)arg + chan;
    fired++;
}

static void cb_not_to_be_executed(void *arg, int chan)
{
    (void)arg;
    (void)chan;

    fired = 1;
}

static uword_t query_channel_numof(tim_t dev)
{
    if (IS_USED(MODULE_PERIPH_TIMER_QUERY_FREQS)) {
        return timer_query_channel_numof(dev);
    }

    return TIMER_CHANNEL_NUMOF;
}

static unsigned milliseconds_to_ticks(uint32_t timer_freq, unsigned millisecs)
{
    /* Use 64 bit arithmetic to avoid overflows for high frequencies. */
    unsigned result = ((uint64_t)millisecs * US_PER_MS * timer_freq) / US_PER_SEC;

    /* Never return less than MINIMUM_TICKS ticks */
    return (result >= MINIMUM_TICKS) ? result : MINIMUM_TICKS;
}

static int test_timer(unsigned num, uint32_t timer_freq)
{
    int set = 0;

    /* reset state */
    atomic_store_u32(&sw_count, 0);
    atomic_store_u8(&fired, 0);

    for (unsigned i = 0; i < TIMER_CHANNEL_NUMOF; i++) {
        timeouts[i] = 0;
        args[i] = UINT_MAX;
    }

    printf("  - Calling timer_init(%u, %" PRIu32 ")\n    ",
           num, timer_freq);
    /* initialize and halt timer */
    if (timer_init(TIMER_DEV(num), timer_freq, cb, (void *)(uintptr_t)(COOKIE * num)) != 0) {
        printf("ERROR: timer_init() failed\n\n");
        return 0;
    }
    else {
        printf("initialization successful\n");
    }

    timer_stop(TIMER_DEV(num));
    printf("  - timer_stop(%u): stopped\n", num);
    unsigned chan_offset_ticks = milliseconds_to_ticks(timer_freq, CHAN_OFFSET_MS);

    /* set each available channel */
    for (unsigned i = 0; i < query_channel_numof(TIMER_DEV(num)); i++) {
        unsigned timeout = ((i + 1) * chan_offset_ticks);
        printf("  - timer_set(%u, %u, %u)\n    ", num, i, timeout);
        if (timer_set(TIMER_DEV(num), i, timeout) < 0) {
            printf("ERROR: Couldn't set timeout %u for channel %u\n",
                   timeout, i);
            /* If the timer supports the periph_timer_query_freqs feature, we
             * expect it to correctly report the number of supported channels
             */
            if (IS_USED(MODULE_PERIPH_TIMER_QUERY_FREQS)) {
                return 0;
            }
            break;
        }
        else {
            ++set;
            printf("Successfully set timeout %u for channel %u\n",
                   timeout, i);
        }
    }

    if (set == 0) {
        printf("  ERROR setting timeout failed for *ALL* channels\n\n");
        return 0;
    }

    /* start the timer */
    printf("  - timer_start(%u)\n", num);
    timer_start(TIMER_DEV(num));

    /* wait for all channels to fire */
    do {
        semi_atomic_fetch_add_u32(&sw_count, 1);
    } while (atomic_load_u8(&fired) != set);

    /* collect results */
    printf("  - Results:\n");
    for (int i = 0; i < set; i++) {
        if (args[i] != ((COOKIE * num) + i)) {
            printf("    ERROR: Callback for channel %u on timer %u has incorrect argument\n",
                   i, num);
            return 0;
        }
        printf("    - channel %i fired at SW count %8u",
               i, (unsigned)timeouts[i]);
        if (i == 0) {
            printf("      - init: %8" PRIu32 "\n", atomic_load_u32(&timeouts[i]));
        }
        else {
            printf("      - diff: %8" PRIu32 "\n",
                   atomic_load_u32(&timeouts[i]) - atomic_load_u32(&timeouts[i - 1]));
        }
    }

    /* test for spurious timer IRQs */
    printf("  - Validating no spurious IRQs are triggered:\n");
    expect(0 == timer_init(TIMER_DEV(num), timer_freq, cb_not_to_be_executed, NULL));

    const unsigned duration = milliseconds_to_ticks(timer_freq, MINIMUM_TIMEOUT_MS);
    unsigned target = timer_read(TIMER_DEV(num)) + duration;

    expect(0 == timer_set_absolute(TIMER_DEV(num), 0, target));
    expect(0 == timer_clear(TIMER_DEV(num), 0));
    atomic_store_u8(&fired, 0);
    while ((target - timer_read(TIMER_DEV(num))) <= duration) {
        /* busy waiting for the timer to reach it timeout. Timer must not fire,
         * it was cleared */
    }
    if (atomic_load_u8(&fired)) {
        printf("    ERROR: Spurious timer fired (1/2)\n");
        return 0;
    }

    /* checking again to make sure that any IRQ pending bit that may just was
     * mask doesn't trigger a timer IRQ on the next set */
    target = timer_read(TIMER_DEV(num)) + duration;

    while ((target - timer_read(TIMER_DEV(num))) <= duration) {
        /* busy waiting for the timer to reach it timeout. Timer must not fire,
         * it was cleared */
    }
    if (atomic_load_u8(&fired)) {
        printf("    ERROR: Spurious timer fired (2/2)\n");
        return 0;
    }

    printf("    [OK] (no spurious IRQs)\n");

    return 1;
}


/* This test is designed to catch an implementation bug where a timer callback is
 * called directly from inside timer_set if the given timeout=0, leading to a
 * stack overflow if timer_set is called from within the callback of the same
 * timer.
 *
 * The test will attempt to initialize each timer in the system and set a non-zero
 * timeout at first. The callback function provided will then attempt to set a new
 * timeout=0 until we have called the callback TEST_ITERATIONS times (default 10000).
 * The expected behavior is that the timer will trigger again as soon as the timer
 * callback function returns. If the timer driver implementation is broken, then
 * the callback will be called again by timer_set, causing a stack overflow after a
 * number of iterations. */
static int test_timer_timeout(unsigned num, uint32_t timer_freq)
{
    /* initialize and halt timer */
    unsigned long switches = 0;
    test_ctx_t ctx = {
        .counter = 0,
        .dev = TIMER_DEV(num),
        .mtx = MUTEX_INIT_LOCKED
    };

    printf("  - Testing timeout=0 in callback:\n");

    if (timer_init(ctx.dev, timer_freq, cb_incr, &ctx) < 0) {
        printf("    TIMER_DEV(%u) init failed.\n", num);
        return 0;
    }
    /* Send the initial trigger for the timer */
    timer_set(ctx.dev, 0, 100);
    /* Wait until we have executed the zero timeout callback enough times */
    while (ctx.counter < TEST_ITERATIONS) {
        mutex_lock(&ctx.mtx);
        ++switches;
    }

    /* verify results */
    if (ctx.counter != TEST_ITERATIONS) {
        printf("    TIMER_DEV(%u) counter mismatch, expected: %lu, actual: %lu\n",
               num, TEST_ITERATIONS, ctx.counter);
        return 0;
    }
    printf("    OK (timer timeout successful)\n");
    return 1;
}

static uword_t query_freq_numof(tim_t dev)
{
    if (IS_USED(MODULE_PERIPH_TIMER_QUERY_FREQS)) {
        return timer_query_freqs_numof(dev);
    }

    return 1;
}

static uint32_t query_freq(tim_t dev, uword_t index)
{
    if (IS_USED(MODULE_PERIPH_TIMER_QUERY_FREQS)) {
        return timer_query_freqs(dev, index);
    }

    /* Fallback implementation when periph_timer_query_freqs is not
     * implemented */
    if (index) {
        return 0;
    }

    return TIMER_SPEED;
}

static void print_supported_frequencies(tim_t dev)
{
    if (!IS_USED(MODULE_PERIPH_TIMER_QUERY_FREQS)) {
        printf("  - feature periph_timer_query_freqs unsupported\n");
        return;
    }

    uword_t end = query_freq_numof(dev);

    printf("  - supported frequencies:\n");
    for (uword_t i = 0; i < MIN(end, 3); i++) {
        printf("    %u: %" PRIu32 "\n", (unsigned)i, timer_query_freqs(dev, i));
    }

    if (end > 3) {
        printf("    ....\n"
               "    %u: %" PRIu32 "\n",
               (unsigned)(end - 1), timer_query_freqs(dev, end - 1));
    }
}

static void test_querying(tim_t dev)
{
    uword_t last = query_freq_numof(dev) - 1;

    puts("Testing timer_get_closest_freq()...");
    for (uword_t i = 0; i <= MIN(last, 255); i++) {
        uint32_t closest;
        uint32_t freq = timer_query_freqs(dev, i);

        /* Searching for a supported freq should yield the supported freq: */
        expect(freq == timer_get_closest_freq(dev, freq));

        /* Assuming that no other frequency close to `freq` are supported,
         * we would assume that asking for `freq - 1` and for `freq + 1` would
         * also return `freq`. There are some corner cases, though. Let's look
         * at asking for `freq - 1` here first:
         *
         * - `freq - 1` is actually also supported. In that case
         *   `timer_get_closest_freq(dev, freq - 1)` should actually return
         *   `freq - 1`
         * - `freq - 2` is also supported (but not `freq - 1`). In this case
         *   returning either `freq - 2` or `freq` would be valid for
         *   `timer_get_closest_freq(dev, freq - 1)`.
         *
         * Therefore, we just except `freq - 2`, `freq - 1`, and `freq` as
         * results for `timer_get_closest_freq(dev, freq - 1)`. */
        closest = timer_get_closest_freq(dev, freq - 1);
        expect((freq >= closest) && closest >= freq - 2);

        /* Now same with `freq + 1` as target. Due to the same corner cases,
         * we accept `freq`, `freq + 1`, and `freq + 2` here. */
        closest = timer_get_closest_freq(dev, freq + 1);
        expect((freq <= closest) && closest <= freq + 2);
    }

    puts("[OK]\n"
         "Testing timer_query_freqs() for out of bound index...");
    expect(timer_query_freqs(dev, last + 1) == 0);
    puts("[OK]");
}

int main(void)
{
    puts("\nTest for peripheral TIMERs\n");

    printf("Available timers: %i\n", TIMER_NUMOF);

    int failed = 0;

    /* test all configured timers */
    for (unsigned i = 0; i < TIMER_NUMOF; i++) {
        printf("\nTIMER %u\n"
               "=======\n\n", i);
        print_supported_frequencies(TIMER_DEV(i));

        /* test querying of frequencies, but only if supported by the driver */
        if (IS_USED(MODULE_PERIPH_TIMER_QUERY_FREQS)) {
            test_querying(TIMER_DEV(i));
        }

        uword_t end = query_freq_numof(TIMER_DEV(i));

        /* Test only up to three frequencies and only the fastest once.
         * (Some timers support really low frequencies and even when limiting
         * to only three frequencies tested, the test will take ages to
         * complete */
        end = MIN(end, 3);
        for (uword_t j = 0; j < end; j++) {
            uint32_t freq = query_freq(TIMER_DEV(i), j);
            if (!test_timer(i, freq)) {
                failed = 1;
            }
            if (!test_timer_timeout(i, freq)) {
                failed = 1;
            }
        }
    }

    /* draw conclusion */
    if (!failed) {
        puts("\nTEST SUCCEEDED");
    }
    else {
        puts("\nTEST FAILED");
    }

    return 0;
}
