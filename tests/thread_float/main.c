/*
 * Copyright (C) 2017 OTA keys S.A.
 *               2021 Otto-von-Guericke-Universität Magdeburg
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
 * @author Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

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

static void *thread_1_2_3(void *_arg)
{
    const char *arg = _arg;
    float f, init;

    mutex_lock(&lock);
    printf("THREAD %s start\n", arg);
    mutex_unlock(&lock);

    /* Use number at end of thread name, e.g. 3 for "t3", to seed the calculation */
    init = 1.0 * (arg[strlen(arg) - 1] - '0');
    f = init;

    while (1) {
        for (unsigned long i = 0; i < 10000ul; i++) {
            f = f + 1.0 / f;
        }
        /* only t1 and t3 should print */
        if (strcmp("t2", arg) != 0) {
            mutex_lock(&lock);
            printf("%s: %f\n", arg, (double)f);
            mutex_unlock(&lock);
        }
        f = init;
    }
    return NULL;
}

int main(void)
{
    const char *t1_name = "t1";
    const char *t2_name = "t2";
    const char *t3_name = "t3";
    p1 = thread_create(t1_stack, sizeof(t1_stack), THREAD_PRIORITY_MAIN + 1,
                       THREAD_CREATE_WOUT_YIELD | THREAD_CREATE_STACKTEST,
                       thread_1_2_3, (void *)t1_name, t1_name);
    p2 = thread_create(t2_stack, sizeof(t2_stack), THREAD_PRIORITY_MAIN + 1,
                       THREAD_CREATE_WOUT_YIELD | THREAD_CREATE_STACKTEST,
                       thread_1_2_3, (void *)t2_name, t2_name);
    p3 = thread_create(t3_stack, sizeof(t3_stack), THREAD_PRIORITY_MAIN + 1,
                       THREAD_CREATE_WOUT_YIELD | THREAD_CREATE_STACKTEST,
                       thread_1_2_3, (void *)t3_name, t3_name);
    puts("THREADS CREATED\n");

    timer.callback = timer_cb;
    xtimer_set(&timer, OFFSET);

    return 0;
}
