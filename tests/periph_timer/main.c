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

#include "periph/timer.h"

/**
 * @brief   Make sure, the maximum number of timers is defined
 */
#ifndef TIMER_NUMOF
#error "TIMER_NUMOF not defined!"
#endif

#define MAX_CHANNELS        (10U)
#define TIM_SPEED           (1U)        /* try to run with 1MHz */
#define CHAN_OFFSET         (5000U)     /* fire every 5ms */

static volatile int fired;
static volatile uint32_t sw_count;
static volatile uint32_t timeouts[MAX_CHANNELS];

static void cb(int chan)
{
    timeouts[chan] = sw_count;
    fired++;
}

static int test_timer(int num)
{
    int set = 0;

    /* reset state */
    sw_count = 0;
    fired = 0;
    for (unsigned i = 0; i < MAX_CHANNELS; i++) {
        timeouts[i] = 0;
    }

    /* initialize and halt timer */
    if (timer_init(TIMER_DEV(num), TIM_SPEED, cb) < 0) {
        printf("TIMER_%i: ERROR on initialization - skipping\n\n", num);
        return 0;
    }
    else {
        printf("TIMER_%i: initialization successful\n", num);
    }
    timer_stop(TIMER_DEV(num));
    printf("TIMER_%i: stopped\n", num);
    /* set each available channel */
    for (unsigned i = 0; i < MAX_CHANNELS; i++) {
        unsigned timeout = ((i + 1) * CHAN_OFFSET);
        if (timer_set(TIMER_DEV(num), i, timeout) < 0) {
            break;
        }
        else {
            ++set;
            printf("TIMER_%i: set channel %u to %u\n", num, i, timeout);
        }
    }
    if (set == 0) {
        printf("TIMER_%i: ERROR setting any channel\n\n", num);
        return 0;
    }
    /* start the timer */
    printf("TIMER_%i: starting\n", num);
    timer_start(TIMER_DEV(num));
    /* wait for all channels to fire */
    do {
        ++sw_count;
    } while (fired != set);
    /* collect results */
    for (int i = 0; i < fired; i++) {
        printf("TIMER_%i: channel %i fired at SW count %8u",
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
    puts("This test will test all configured peripheral timers of the\n"
         "targeted platform. For each timer, it will set each channel with\n"
         "an incrementing timeout. CH0 set to 5ms, CH1 to 10ms, CH2 to 15ms\n"
         "and so on.\n"
         "In the output you should see that every channel fired, after an\n"
         "evenly distributed amount of time -> the shown diff values should\n"
         "be pretty much equal (to some jitter...)\n"
         "This test does however NOT show, if the timeouts were correct in\n"
         "relation to the expected real-time ~ use e.g. tests/xtimer_msg for\n"
         "this.\n\n");

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
