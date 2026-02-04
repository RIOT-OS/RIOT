/*
 * SPDX-FileCopyrightText: 2017 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Mutex context switch benchmark
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>

#include "macros/units.h"
#include "clk.h"
#include "mutex.h"
#include "thread.h"
#include "xtimer.h"

#ifndef TEST_DURATION
#define TEST_DURATION       (1000000U)
#endif

volatile unsigned _flag = 0;
static char _stack[THREAD_STACKSIZE_MAIN];
static mutex_t _mutex = MUTEX_INIT;

static void _timer_callback(void*arg)
{
    (void)arg;

    _flag = 1;
}

static void *_second_thread(void *arg)
{
    (void)arg;

    while(1) {
        mutex_lock(&_mutex);
    }

    return NULL;
}

int main(void)
{
    printf("main starting\n");

    thread_create(_stack,
                  sizeof(_stack),
                  THREAD_PRIORITY_MAIN - 1,
                  THREAD_CREATE_WOUT_YIELD,
                  _second_thread,
                  NULL,
                  "second_thread");

    /* lock the mutex, then yield to second_thread */
    mutex_lock(&_mutex);
    thread_yield_higher();

    xtimer_t timer;
    timer.callback = _timer_callback;

    uint32_t n = 0;

    xtimer_set(&timer, TEST_DURATION);
    while(!_flag) {
        mutex_unlock(&_mutex);
        n++;
    }

    printf("{ \"result\" : %"PRIu32, n);
    printf(", \"ticks\" : %"PRIu32,
           (uint32_t)((TEST_DURATION/US_PER_MS) * (coreclk()/KHZ(1)))/n);
    puts(" }");

    return 0;
}
