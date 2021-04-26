/*
 * Copyright (C) 2015 Freie Universität Berlin
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
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>
#include <stdatomic.h>
#include <stdlib.h>

#include "periph/timer.h"

/**
 * @brief   Make sure, the maximum number of timers is defined
 */
#ifndef TIMER_NUMOF
#error "TIMER_NUMOF not defined!"
#endif

#define MAX_CHANNELS        (10U)
#define CHAN_OFFSET         (5000U)     /* fire every 5ms */
#define COOKIE              (100U)      /* for checking if arg is passed */

static atomic_uint fired;
static atomic_uint_fast32_t sw_count;
static atomic_uint_fast32_t timeouts[MAX_CHANNELS];
static atomic_uintptr_t args[MAX_CHANNELS];

static void cb(void *arg, int chan)
{
    atomic_store(&timeouts[chan], sw_count);
    atomic_store(&args[chan], (uintptr_t)arg + chan);
    atomic_fetch_add(&fired, 1);
}

static int test_timer(unsigned num, uint32_t speed)
{
    unsigned set = 0;

    /* reset state */
    atomic_store(&sw_count, 0);
    atomic_store(&fired, 0);
    for (unsigned i = 0; i < MAX_CHANNELS; i++) {
        atomic_store(&timeouts[i], 0);
        atomic_store(&args[i], UINTPTR_MAX);
    }

    /* initialize and halt timer */
    if (timer_init(TIMER_DEV(num), speed, cb, (void *)(COOKIE * num)) < 0) {
        printf("TIMER_%u: ERROR on initialization - skipping\n\n", num);
        return -1;
    }
    else {
        printf("TIMER_%u: initialization successful\n", num);
    }
    timer_stop(TIMER_DEV(num));
    printf("TIMER_%u: stopped\n", num);
    /* set each available channel */
    for (unsigned i = 0; i < MAX_CHANNELS; i++) {
        unsigned timeout = ((i + 1) * CHAN_OFFSET);
        if (timer_set(TIMER_DEV(num), i, timeout) < 0) {
            break;
        }
        else {
            ++set;
            printf("TIMER_%u: set channel %u to %u\n", num, i, timeout);
        }
    }
    if (set == 0) {
        printf("TIMER_%u: ERROR setting any channel\n\n", num);
        return -1;
    }
    /* start the timer */
    printf("TIMER_%u: starting\n", num);
    timer_start(TIMER_DEV(num));
    /* wait for all channels to fire */
    do {
        atomic_fetch_add(&sw_count, 1);
    } while (atomic_load(&fired) != set);
    /* collect results */
    unsigned fired_at_end = atomic_load(&fired);
    for (unsigned i = 0; i < fired_at_end; i++) {
        if (atomic_load(&args[i]) != ((COOKIE * num) + i)) {
            printf("TIMER_%u: ERROR callback argument mismatch\n\n", num);
            return 0;
        }
        uint32_t timeout = atomic_load(&timeouts[i]);
        printf("TIMER_%u: channel %i fired at SW count %8" PRIu32, num, i, timeout);
        if (i == 0) {
            printf(" - init: %8" PRIu32 "\n", timeout);
        }
        else {
            printf(" - diff: %8" PRIu32 "\n", timeout - atomic_load(&timeouts[i - 1]));
        }
    }
    return 0;
}

static uint32_t timer_query_freq_wrapper(tim_t dev, uword_t index)
{
    if (IS_USED(MODULE_PERIPH_TIMER_QUERY_FREQ)) {
        return timer_query_freq(dev, index);
    }

    return (index) ? 0 : TIMER_SPEED;
}

int main(void)
{
    int failed = 0;

    puts("\nTest for peripheral TIMERs\n");

    printf("Available timers: %i\n", TIMER_NUMOF);

    /* test all configured timers */
    for (unsigned i = 0; i < TIMER_NUMOF; i++) {
        printf("\nTesting TIMER_%u:\n", i);
        uint32_t freq;
        for (uword_t j = 0; (freq = timer_query_freq_wrapper(TIMER_DEV(i), j)); j++) {
            printf("Frequency: %" PRIu32 " Hz\n", freq);
            if (test_timer(i, freq)) {
                failed = 0;
            }
        }
    }
    /* draw conclusion */
    if (failed) {
        puts("\nTEST FAILED");
    }
    else {
        puts("\nTEST SUCCEEDED");
    }

    return 0;
}
