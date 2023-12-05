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
#include <stdlib.h>

#include "atomic_utils.h"
#include "clk.h"
#include "periph/timer.h"
#include "test_utils/expect.h"
#include "time_units.h"

/**
 * @brief   Make sure, the maximum number of timers is defined
 */
#ifndef TIMER_NUMOF
#error "TIMER_NUMOF not defined!"
#endif

#define MAX_CHANNELS        (10U)
#define CHAN_OFFSET         (5000U)     /* fire every 5ms */
#define COOKIE              (100U)      /* for checking if arg is passed */

static uint8_t fired;
static uint32_t sw_count;
static uint32_t timeouts[MAX_CHANNELS];
static unsigned args[MAX_CHANNELS];

static void cb(void *arg, int chan)
{
    timeouts[chan] = sw_count;
    args[chan] = (unsigned)arg + chan;
    fired++;
}

static void cb_not_to_be_executed(void *arg, int chan)
{
    (void)arg;
    (void)chan;

    puts("Spurious timer fired");
    expect(0);
}

static int test_timer(unsigned num)
{
    int set = 0;

    /* reset state */
    atomic_store_u32(&sw_count, 0);
    atomic_store_u8(&fired, 0);

    for (unsigned i = 0; i < MAX_CHANNELS; i++) {
        timeouts[i] = 0;
        args[i] = UINT_MAX;
    }

    /* initialize and halt timer */
    if (timer_init(TIMER_DEV(num), TIMER_SPEED, cb, (void *)(COOKIE * num)) < 0) {
        printf("TIMER_%u: ERROR on initialization - skipping\n\n", num);
        return 0;
    }
    else {
        printf("TIMER_%u: initialization successful\n", num);
        if (IS_USED(MODULE_PERIPH_TIMER_FREQ)) {
            uint32_t actual_freq = timer_freq(TIMER_DEV(num));
            printf("(running at %" PRIu32 " Hz)\n", actual_freq);
        }
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
        return 0;
    }

    /* start the timer */
    printf("TIMER_%u: starting\n", num);
    timer_start(TIMER_DEV(num));

    /* wait for all channels to fire */
    do {
        semi_atomic_fetch_add_u32(&sw_count, 1);
    } while (atomic_load_u8(&fired) != set);

    /* collect results */
    for (int i = 0; i < set; i++) {
        if (args[i] != ((COOKIE * num) + i)) {
            printf("TIMER_%u: ERROR callback argument mismatch\n\n", num);
            return 0;
        }
        printf("TIMER_%u: channel %i fired at SW count %8u",
               num, i, (unsigned)timeouts[i]);
        if (i == 0) {
            printf(" - init: %8" PRIu32 "\n", atomic_load_u32(&timeouts[i]));
        }
        else {
            printf(" - diff: %8" PRIu32 "\n",
                   atomic_load_u32(&timeouts[i]) - atomic_load_u32(&timeouts[i - 1]));
        }
    }

    /* test for spurious timer IRQs */
    expect(0 == timer_init(TIMER_DEV(num), TIMER_SPEED, cb_not_to_be_executed, NULL));

    const unsigned duration = 2ULL * US_PER_MS * US_PER_SEC / TIMER_SPEED;
    unsigned target = timer_read(TIMER_DEV(num)) + duration;
    expect(0 == timer_set_absolute(TIMER_DEV(num), 0, target));
    expect(0 == timer_clear(TIMER_DEV(num), 0));
    while (timer_read(TIMER_DEV(num)) < target) {
        /* busy waiting for the timer to reach it timeout. Timer must not fire,
         * it was cleared */
    }

    /* checking again to make sure that any IRQ pending bit that may just was
     * mask doesn't trigger a timer IRQ on the next set */
    target = timer_read(TIMER_DEV(num)) + duration;
    timer_set_absolute(TIMER_DEV(num), 0, target);
    timer_clear(TIMER_DEV(num), 0);

    while (timer_read(TIMER_DEV(num)) < target) {
        /* busy waiting for the timer to reach it timeout. Timer must not fire,
         * it was cleared */
    }

    return 1;
}

int main(void)
{
    int res = 0;

    puts("\nTest for peripheral TIMERs\n");

    printf("Available timers: %i\n", TIMER_NUMOF);

    /* test all configured timers */
    for (unsigned i = 0; i < TIMER_NUMOF; i++) {
        printf("\nTesting TIMER_%u:\n", i);
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
