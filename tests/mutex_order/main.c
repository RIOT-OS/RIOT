/*
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
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include <stdio.h>

#include "mutex.h"
#include "thread.h"

#define THREAD_NUMOF            (5U)

extern volatile thread_t *sched_active_thread;

static char stacks[THREAD_NUMOF][THREAD_STACKSIZE_MAIN];

static const char prios[THREAD_NUMOF] = {THREAD_PRIORITY_MAIN - 1, 4, 0, 2, 1};

static mutex_t testlock;

static void *lockme(void *arg)
{
    (void)arg;
    volatile thread_t *t = sched_active_thread;

    printf("T%i (prio %i): trying to lock mutex now\n",
           (int)t->pid, (int)t->priority);
    mutex_lock(&testlock);
    printf("T%i (prio %i): locked mutex now\n",
           (int)t->pid, (int)t->priority);

    thread_yield();

    mutex_unlock(&testlock);

    return NULL;
}

int main(void)
{
    puts("Mutex order test");
    puts("Please refer to the README.md for more information\n");

    mutex_init(&testlock);

    /* lock mutex, so that spawned threads have to wait */
    mutex_lock(&testlock);
    /* create threads */
    for (unsigned i = 0; i < THREAD_NUMOF; i++) {
        thread_create(stacks[i], sizeof(stacks[i]), prios[i], 0,
                      lockme, NULL, "t");
    }
    /* allow threads to lock the mutex */
    mutex_unlock(&testlock);

    mutex_lock(&testlock);
    puts("\nTest END, check the order of priorities above.");

    return 0;
}
