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

#include <stdbool.h>
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

#define MAX_LOOPS   (10U)
#define TIMER_SPEED (1000000ul) /* try to run with 1MHz */
#define TIMEOUT     (20000U)

static volatile unsigned fired;
static volatile uint32_t sw_count;
static volatile unsigned timevals[MAX_LOOPS];

static void cb(void *arg, int chan)
{
    (void) chan;
    timevals[fired] = timer_read(TIMER_DEV((unsigned)arg));
    fired++;
    if (fired < MAX_LOOPS) {
        timer_set(TIMER_DEV((unsigned)arg), 0, TIMEOUT);
    }
}

static int test_timer(unsigned num)
{
    printf("\n*** Testing TIMER_%u ***\n", num);
    /* reset state */
    sw_count = 0;
    fired = 0;
    for (unsigned i = 0; i < MAX_LOOPS; i++) {
        timevals[i] = 0;
    }
    printf("  timer_init(%u, %lu, cb, args) ", num, TIMER_SPEED);
    /* initialize and halt timer */
    if (timer_init(TIMER_DEV(num), TIMER_SPEED, cb, (void *)(num)) < 0) {
        puts("FAIL");
        return 0;
    }
    puts("DONE");
    /* stop timer */
    timer_stop(TIMER_DEV(num));
    /* set first timeout */
    printf("  timer_set(%u, 0, %u) ", num, TIMEOUT);
    if (timer_set(TIMER_DEV(num), 0, TIMEOUT) < 0) {
        puts("FAIL");
        return 0;
    }
    puts("DONE");
    /* (re)start timer */
    timer_start(TIMER_DEV(num));
    /* wait for all channels to fire */
    do {
        ++sw_count;
    } while (fired < MAX_LOOPS);
    bool match = true;
    /* collect results */
    printf("  fired %u times, offset %u [us], result table\n\n", fired, TIMEOUT);
    printf("; num, begin, until, timer_diff, simple_diff\n");
    for (unsigned i = 1; i < fired; i++) {
        unsigned tdiff = timer_diff(TIMER_DEV(num), timevals[i-1], timevals[i]);
        unsigned sdiff = (timevals[i] - timevals[i-1]);
        if (tdiff != sdiff) {
            match = false;
        }
        printf("  %u, %u, %u, %u, %u\n", i, timevals[i-1], timevals[i], tdiff, sdiff);
    }
    printf("\n  Note: timers max value is %s platform max value.\n",
           ((match) ? "equal to" : "less than"));
    return 1;
}

int main(void)
{
    int res = 0;

    printf("TIMER_NUMOF=%i\n", TIMER_NUMOF);
    printf("TIMER_SPEED=%lu\n", TIMER_SPEED);

    /* test all configured timers */
    for (unsigned i = 0; i < TIMER_NUMOF; i++) {
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
