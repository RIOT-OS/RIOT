/*
 * Copyright (C) 2022 ML!PA Consulting GmbH
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
 * @brief       Thread sleep test application
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <stdio.h>
#include "thread.h"
#include "ztimer.h"

static char t1_stack[THREAD_STACKSIZE_SMALL];
static char t2_stack[THREAD_STACKSIZE_SMALL];

static void *first_thread(void *arg)
{
    (void) arg;

    while (1) {
        puts("first thread");
        ztimer_sleep(ZTIMER_MSEC, 10);
    }

    return NULL;
}

static void *second_thread(void *arg)
{
    (void) arg;

    while (1) {
        puts("second thread");
        ztimer_sleep(ZTIMER_MSEC, 10);
    }
    return NULL;
}

int main(void)
{
    kernel_pid_t pid1 = thread_create(
            t1_stack, sizeof(t1_stack),
            THREAD_PRIORITY_MAIN - 1,
            THREAD_CREATE_STACKTEST,
            first_thread, NULL, "nr1");
    kernel_pid_t pid2 = thread_create(
            t2_stack, sizeof(t2_stack),
            THREAD_PRIORITY_MAIN - 1,
            THREAD_CREATE_STACKTEST,
            second_thread, NULL, "nr2");

    sched_set_status(thread_get(pid1), STATUS_SLEEPING);
    sched_set_status(thread_get(pid2), STATUS_SLEEPING);

    ztimer_sleep(ZTIMER_MSEC, 100);
    puts("> wake sleeping threads");

    sched_set_status(thread_get(pid1), STATUS_PENDING);
    sched_set_status(thread_get(pid2), STATUS_PENDING);

    thread_yield();
    puts("> put threads to sleep again");

    sched_set_status(thread_get(pid1), STATUS_SLEEPING);
    sched_set_status(thread_get(pid2), STATUS_SLEEPING);

    return 0;
}
