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
 * @brief       Measure messages send per second
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdint.h>
#include <stdatomic.h>
#include <stdio.h>
#include "macros/units.h"
#include "thread.h"
#include "clk.h"

#include "msg.h"
#include "xtimer.h"

#ifndef TEST_DURATION_US
#define TEST_DURATION_US    (1000000U)
#endif

static char _stack[THREAD_STACKSIZE_MAIN];

static void _timer_callback(void *_flag)
{
    atomic_flag *flag = _flag;
    atomic_flag_clear(flag);
}

static void *_second_thread(void *arg)
{
    (void)arg;

    while (1) {
        msg_t test;
        msg_receive(&test);
    }

    return NULL;
}

int main(void)
{
    puts("main starting");

    kernel_pid_t other = thread_create(_stack,
                                       sizeof(_stack),
                                       (THREAD_PRIORITY_MAIN - 1),
                                       0,
                                       _second_thread,
                                       NULL,
                                       "second_thread");

    atomic_flag flag = ATOMIC_FLAG_INIT;
    uint32_t n = 0;

    xtimer_t timer = {
        .callback = _timer_callback,
        .arg = &flag,
    };

    atomic_flag_test_and_set(&flag);
    xtimer_set(&timer, TEST_DURATION_US);

    while (atomic_flag_test_and_set(&flag)) {
        msg_t test;
        msg_send(&test, other);
        n++;
    }

    printf("{ \"result\" : %"PRIu32, n);
    printf(", \"ticks\" : %"PRIu32,
           (uint32_t)((TEST_DURATION_US/US_PER_MS) * (coreclk()/KHZ(1)))/n);
    puts(" }");

    return 0;
}
