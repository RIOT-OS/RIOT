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

#include "clk.h"
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

static volatile int fired;
static volatile uint32_t sw_count;
static volatile uint32_t timeouts[MAX_CHANNELS];
static volatile unsigned args[MAX_CHANNELS];

static void cb(void *arg, int chan)
{
    timeouts[chan] = sw_count;
    args[chan] = (unsigned)arg + chan;
    fired++;
}

static int test_timer(unsigned num)
{
    int set = 0;

    /* reset state */
    sw_count = 0;
    fired = 0;
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
        ++sw_count;
    } while (fired != set);
    /* collect results */
    for (int i = 0; i < fired; i++) {
        if (args[i] != ((COOKIE * num) + i)) {
            printf("TIMER_%u: ERROR callback argument mismatch\n\n", num);
            return 0;
        }
        printf("TIMER_%u: channel %i fired at SW count %8u",
               num, i, (unsigned)timeouts[i]);
        if (i == 0) {
            printf(" - init: %8u\n", (unsigned)timeouts[i]);
        }
        else {
            printf(" - diff: %8u\n", (unsigned)(timeouts[i] - timeouts[i - 1]));
        }
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
