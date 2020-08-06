/*
 * Copyright (C) 2019 Freie Universität Berlin,
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief   Testing behaviour of thread_yield_higher in interrupt
 *
 * Calling 'thread_yield_higher' from an interrupt should cause the
 * thread to yield after the interrupt. This test flags cpus that
 * do not behave accordingly.
 *
 * @author  Gaëtan Harter <gaetan.harter@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "thread.h"
#include "xtimer.h"

#define TEST_TIME (200000U)

struct cb_arg {
    thread_t *thread;
    uint8_t value;
};

static void _timer_cb(void *arg)
{
    struct cb_arg *cb_arg = arg;

    /* simulate what is done in `xtimer_mutex_lock_timeout` */
    sched_set_status(cb_arg->thread, STATUS_RUNNING);
    thread_yield_higher();

    /* set the value after thread_yield_higher */
    cb_arg->value = 1;
}

int main(void)
{
    struct cb_arg arg = {.thread=(thread_t *)sched_active_thread, .value=0};
    xtimer_t timer = {.callback=_timer_cb, .arg=&arg};

    printf("Send character to start\n");
    getchar();

    printf("Go to sleep\n");
    xtimer_set(&timer, 1000000);
    thread_sleep();
    printf("Value is %u\n", arg.value);

    return 0;
}
