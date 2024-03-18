/*
 * Copyright (C) 2018 Eistec AB
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
 * @brief       Peripheral timer regression test application
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "mutex.h"
#include "periph/timer.h"

#ifndef TIMER_NUMOF
#error "TIMER_NUMOF not defined!"
#endif

#ifndef TIM_TEST_FREQ
#define TIM_TEST_FREQ       (1000000ul)
#endif

#ifndef TEST_ITERATIONS
#define TEST_ITERATIONS     (10000ul)
#endif

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

/* List of frequencies to try for timer_init */
static const unsigned long timer_freqs[] = {
    TIM_TEST_FREQ,
    1000000ul,
    250000ul,
    32768ul,
    1000ul,
};

static int test_timer(unsigned num)
{
    /* initialize and halt timer */
    unsigned long switches = 0;
    test_ctx_t ctx = {
        .counter = 0,
        .dev = TIMER_DEV(num),
        .mtx = MUTEX_INIT_LOCKED
    };
    int res;

    for (unsigned k = 0; k < ARRAY_SIZE(timer_freqs); ++k) {
        res = timer_init(ctx.dev, timer_freqs[k], cb_incr, &ctx);
        if (res >= 0) {
            printf("TIMER_DEV(%u) running at %lu Hz\n", num, timer_freqs[k]);
            break;
        }
    }
    if (res < 0) {
        printf("TIMER_DEV(%u) init failed: %d\n", num, res);
        return -1;
    }
    /* Send the initial trigger for the timer */
    timer_set(ctx.dev, 0, 100);
    /* Wait until we have executed the zero timeout callback enough times */
    while (ctx.counter < TEST_ITERATIONS) {
        mutex_lock(&ctx.mtx);
        ++switches;
    }

    printf("(debug) TIMER_DEV(%u) switches: %lu\n", num, switches);
    /* verify results */
    if (ctx.counter != TEST_ITERATIONS) {
        printf("TIMER_DEV(%u) counter mismatch, expected: %lu, actual: %lu\n",
               num, TEST_ITERATIONS, ctx.counter);
        return 0;
    }
    return 1;
}

int main(void)
{
    int res = 0;

    puts("\nTest for timer_set with timeout=0\n");

    printf("Available timers: %i\n", TIMER_NUMOF);

    /* test all configured timers */
    for (unsigned i = 0; i < TIMER_NUMOF; i++) {
        printf("\nTesting TIMER_DEV(%u):\n", i);
        res += test_timer(i);
    }
    /* draw conclusion */
    if (res == TIMER_NUMOF) {
        puts("\nTEST SUCCEEDED");
    }
    else {
        puts("\nTEST FAILED");
    }

    return 0;
}
