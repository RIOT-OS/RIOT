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
#include "ztimer.h"

#define TEST_TIME (200 * US_PER_MS)

static char t2_stack[THREAD_STACKSIZE_MAIN];
static uint32_t start_time;

static void *second_thread(void *arg)
{
    (void)arg;
    if (ztimer_now(ZTIMER_USEC) < (TEST_TIME + start_time)) {
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
    puts("first thread started");

    start_time = ztimer_now(ZTIMER_USEC);

    ztimer_t timer;
    timer.callback = _cb;
    ztimer_set(ZTIMER_USEC, &timer, TEST_TIME / 2);

    (void)thread_create(
        t2_stack, sizeof(t2_stack),
        THREAD_PRIORITY_MAIN,
        THREAD_CREATE_WOUT_YIELD | THREAD_CREATE_STACKTEST,
        second_thread, NULL, "nr2");

    while (ztimer_now(ZTIMER_USEC) < (TEST_TIME + start_time)) {}

    puts("first thread done");

    return 0;
}
