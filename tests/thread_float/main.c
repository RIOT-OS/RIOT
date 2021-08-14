/*
 * Copyright (C) 2017 OTA keys S.A.
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
 * @brief Thread test application
 *
 * @author Vincent Dupont <vincent@otakeys.com>
 *
 * @}
 */

#include <stdio.h>

#include "thread.h"
#include "msg.h"
#include "xtimer.h"
#include "timex.h"

static char t1_stack[THREAD_STACKSIZE_MAIN];
static char t2_stack[THREAD_STACKSIZE_MAIN];
static char t3_stack[THREAD_STACKSIZE_MAIN];

static kernel_pid_t p1, p2, p3;

static xtimer_t timer;
#define OFFSET (10 * XTIMER_BACKOFF)

static mutex_t lock = MUTEX_INIT;

static void timer_cb(void *arg)
{
    (void) arg;

    thread_yield();
    xtimer_set(&timer, OFFSET);
}

static void *thread1(void *arg)
{
    (void) arg;

    float f, init;

    printf("THREAD %" PRIkernel_pid " start\n", thread_getpid());

    init = 1.0 * thread_getpid();
    f = init;

    while (1) {
        for (unsigned long i = 0; i < 10000ul; i++) {
            f = f + 1.0 / f;
        }
        mutex_lock(&lock);
        printf("T(%" PRIkernel_pid "): %f\n", thread_getpid(), (double)f);
        mutex_unlock(&lock);
        init += 1.0;
        f = init;
    }
    return NULL;
}

static void *thread2(void *arg)
{
    (void) arg;

    float f, init;

    printf("THREAD %" PRIkernel_pid " start\n", thread_getpid());

    init = 1.0 * thread_getpid();
    f = init;

    while (1) {
        for (unsigned long i = 0; i < 100000ul; i++) {
            f = f + 1.0 / f;
        }
        init += 1.0;
        f = init;
    }
    return NULL;
}

int main(void)
{
    p1 = thread_create(t1_stack, sizeof(t1_stack), THREAD_PRIORITY_MAIN + 1,
                       THREAD_CREATE_WOUT_YIELD | THREAD_CREATE_STACKTEST,
                       thread1, NULL, "nr1");
    p2 = thread_create(t2_stack, sizeof(t2_stack), THREAD_PRIORITY_MAIN + 1,
                       THREAD_CREATE_WOUT_YIELD | THREAD_CREATE_STACKTEST,
                       thread2, NULL, "nr2");
    p3 = thread_create(t3_stack, sizeof(t3_stack), THREAD_PRIORITY_MAIN + 1,
                       THREAD_CREATE_WOUT_YIELD | THREAD_CREATE_STACKTEST,
                       thread1, NULL, "nr3");
    puts("THREADS CREATED\n");

    timer.callback = timer_cb;
    xtimer_set(&timer, OFFSET);

    return 0;
}
