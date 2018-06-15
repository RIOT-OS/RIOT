/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @brief       Context switch benchmark test application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>

#include "thread.h"
#include "xtimer.h"

#ifndef TEST_DURATION
#define TEST_DURATION       (1000000U)
#endif

volatile unsigned _flag = 0;
static char _stack[THREAD_STACKSIZE_MAIN];

static void _timer_callback(void*arg)
{
    (void)arg;

    _flag = 1;
}

static void *_second_thread(void *arg)
{
    (void)arg;

    while(1) {
        thread_yield();
    }

    return NULL;
}

int main(void)
{
    printf("main starting\n");

    thread_create(_stack,
                  sizeof(_stack),
                  THREAD_PRIORITY_MAIN,
                  THREAD_CREATE_STACKTEST,
                  _second_thread,
                  NULL,
                  "second_thread");

    xtimer_t timer;
    timer.callback = _timer_callback;

    uint32_t n = 0;

    xtimer_set(&timer, TEST_DURATION);
    while(!_flag) {
        thread_yield();
        n++;
    }

    printf("{ \"result\" : %"PRIu32" }\n", n);

    return 0;
}
