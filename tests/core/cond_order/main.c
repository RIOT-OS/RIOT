/*
 * SPDX-FileCopyrightText: 2018 University of California, Berkeley
 * SPDX-FileCopyrightText: 2016 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
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

static char stacks[THREAD_NUMOF][THREAD_STACKSIZE_MAIN];

static const char prios[THREAD_NUMOF] = {THREAD_PRIORITY_MAIN - 1, 4, 0, 2, 1};

static mutex_t testlock;
static cond_t testcond;

static void *lockme(void *arg)
{
    (void)arg;
    thread_t *t = thread_get_active();

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

    /* Test if condition signal and broadcast works when no thread is waiting */
    cond_signal(&testcond);
    cond_broadcast(&testcond);

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
