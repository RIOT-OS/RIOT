/*
 * SPDX-FileCopyrightText: 2017 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Scheduler benchmark test application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>
#include "macros/units.h"
#include "clk.h"
#include "thread.h"

#include "xtimer.h"

#ifndef TEST_DURATION
#define TEST_DURATION       (1000000U)
#endif

volatile unsigned _flag = 0;

static void _timer_callback(void*arg)
{
    (void)arg;

    _flag = 1;
}

int main(void)
{
    printf("main starting\n");

    xtimer_t timer;
    timer.callback = _timer_callback;

    uint32_t n = 0;

    xtimer_set(&timer, TEST_DURATION);
    while(!_flag) {
        thread_yield();
        n++;
    }

    printf("{ \"result\" : %"PRIu32, n);
    printf(", \"ticks\" : %"PRIu32,
           (uint32_t)((TEST_DURATION/US_PER_MS) * (coreclk()/KHZ(1)))/n);
    puts(" }");

    return 0;
}
