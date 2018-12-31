/*
 * Copyright (C) 2018 University of California, Berkeley
 * Copyright (C) 2016 Freie Universität Berlin
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
 * @brief       Test application for testing mutexes
 *
 * @author      Sam Kumar <samkumar@cs.berkeley.edu>,
 *              Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include <stdio.h>

#include "cond.h"
#include "mutex.h"
#include "thread.h"

#define THREAD_NUMOF            (5U)
#define THREAD_FIRSTGROUP_NUMOF (3U)

extern volatile thread_t *sched_active_thread;

static char stacks[THREAD_NUMOF][THREAD_STACKSIZE_MAIN];

static const char prios[THREAD_NUMOF] = {THREAD_PRIORITY_MAIN - 1, 4, 0, 2, 1};

static mutex_t testlock;
static cond_t testcond;

static void *lockme(void *arg)
{
    (void)arg;
    volatile thread_t *t = sched_active_thread;

    mutex_lock(&testlock);
    printf("T%i (prio %i): waiting on condition variable now\n",
           (int)t->pid, (int)t->priority);
    cond_wait(&testcond, &testlock);
    printf("T%i (prio %i): condition variable was signaled now\n",
           (int)t->pid, (int)t->priority);
    mutex_unlock(&testlock);

    thread_yield();

    mutex_unlock(&testlock);

    return NULL;
}

int main(void)
{
    puts("Condition variable order test");
    puts("Please refer to the README.md for more information\n");

    mutex_init(&testlock);
    cond_init(&testcond);

    /* create threads */
    for (unsigned i = 0; i < THREAD_NUMOF; i++) {
        thread_create(stacks[i], sizeof(stacks[i]), prios[i], 0,
                      lockme, NULL, "t");
    }
    /* allow threads to lock the mutex and wait on the condition variable */

    /* signal the first few threads, in a group */
    mutex_lock(&testlock);
    for (unsigned i = 0; i < THREAD_FIRSTGROUP_NUMOF; i++) {
        cond_signal(&testcond);
    }
    printf("First batch was signaled\n");
    mutex_unlock(&testlock);
    /* allow the first THREAD_FIRSTGROUP_NUMOF threads to wake up */

    printf("First batch has woken up\n");

    mutex_lock(&testlock);
    cond_broadcast(&testcond);
    printf("Second batch was signaled\n");
    mutex_unlock(&testlock);
    /* allow the remaining threads to wake up */
    printf("Second batch has woken up\n");

    mutex_lock(&testlock);
    puts("\nTest END, check the order of priorities above.");

    return 0;
}
