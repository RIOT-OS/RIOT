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

#define TEST_TIME (200 * US_PER_MS)

static char t2_stack[THREAD_STACKSIZE_MAIN];
static uint32_t start_time;

static void *second_thread(void *arg)
{
    (void)arg;
    if (xtimer_now_usec() < (TEST_TIME + start_time)) {
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

    start_time = xtimer_now_usec();

    xtimer_t timer;
    timer.callback = _cb;
    xtimer_set(&timer, TEST_TIME / 2);

    (void)thread_create(
        t2_stack, sizeof(t2_stack),
        THREAD_PRIORITY_MAIN,
        THREAD_CREATE_WOUT_YIELD,
        second_thread, NULL, "nr2");

    while (xtimer_now_usec() < (TEST_TIME + start_time)) {}

    puts("first thread done");

    return 0;
}
