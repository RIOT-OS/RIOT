/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @brief   Application for testing cooperative scheduling of same-priority
 *          threads
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>

#include "thread.h"
#include "xtimer.h"

#define TEST_TIME (200000U)

static char t2_stack[THREAD_STACKSIZE_MAIN];

static void *second_thread(void *arg)
{
    (void) arg;
    if (xtimer_now_usec() < TEST_TIME) {
        puts("TEST FAILED");
    }
    else {
        puts("TEST SUCCESSFUL");
    }
    return NULL;
}

static void _cb(void *arg)
{
    (void)arg;
    puts("timer triggered");
    sched_context_switch_request = 1;
}

int main(void)
{
    (void) thread_create(
            t2_stack, sizeof(t2_stack),
            THREAD_PRIORITY_MAIN,
            THREAD_CREATE_WOUT_YIELD | THREAD_CREATE_STACKTEST,
            second_thread, NULL, "nr2");

    puts("first thread started");

    xtimer_t timer;
    timer.callback = _cb;
    xtimer_set(&timer, TEST_TIME/2);

    while(xtimer_now_usec() < TEST_TIME) {}

    puts("first thread done");

    return 0;
}
