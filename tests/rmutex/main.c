/*
 * Copyright (C) 2016 Theobroma Systems Design & Consulting GmbH
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
 * @brief       Test application for testing recursive mutexes
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Martin Elshuber <martin.elshuber@theobroma-systems.com>
 * @}
 */

#include <stdio.h>

#include "rmutex.h"
#include "thread.h"
#include "xtimer.h"

#define THREAD_NUMOF            (5U)

extern volatile thread_t *sched_active_thread;

static char stacks[THREAD_NUMOF][THREAD_STACKSIZE_MAIN];

static const char prios[THREAD_NUMOF] = {THREAD_PRIORITY_MAIN - 1, 4, 5, 2, 4};
static const char depth[THREAD_NUMOF] = {5, 3, 3, 4, 5};
static const unsigned timeouts[THREAD_NUMOF] = { 100 * US_PER_MS, 200 * US_PER_MS, 300 * US_PER_MS,
                                                 400 * US_PER_MS, 500 * US_PER_MS};

static rmutex_t testlock;

static void lock_recursive(char n, char depth)
{
    volatile thread_t *t = sched_active_thread;

    printf("T%i (prio %i, depth %i): trying to lock rmutex now\n",
        (int)t->pid, (int)t->priority, (int)n);
    rmutex_lock(&testlock);

    printf("T%i (prio %i, depth %i): locked rmutex now\n",
           (int)t->pid, (int)t->priority, (int)n);

    if (n + 1 < depth) {
        lock_recursive(n + 1, depth);
    }

    thread_yield();

    rmutex_unlock(&testlock);

    printf("T%i (prio %i, depth %i): unlocked rmutex\n",
           (int)t->pid, (int)t->priority, (int)n);
}

static void *lockme(void *arg)
{
    intptr_t depth = (intptr_t)arg;

    lock_recursive(0, depth);

    return NULL;
}

static void lock_recursive_timeout(char n, uint32_t us)
{
    volatile thread_t *t = sched_active_thread;

    printf("T%i (prio %i, depth %i, timeout %" PRIu32 "): trying to lock rmutex now\n",
        (int)t->pid, (int)t->priority, (int)n, us);
    if (xtimer_rmutex_lock_timeout(&testlock, us) < 0) {
        printf("T%i (prio %i, depth %i): timeout\n",
               (int)t->pid, (int)t->priority, (int)n);
        return;
    }

    printf("T%i (prio %i, depth %i): locked rmutex now\n",
           (int)t->pid, (int)t->priority, (int)n);

    if (n + 1 < 3) {
        lock_recursive_timeout(n + 1, us);
    }

    thread_yield();

    rmutex_unlock(&testlock);

    printf("T%i (prio %i, depth %i): unlocked rmutex\n",
           (int)t->pid, (int)t->priority, (int)n);
}

static void *lockme_timeout(void *arg)
{
    intptr_t timeout = (intptr_t)arg;

    lock_recursive_timeout(0, timeout);

    return NULL;
}

int main(void)
{
    puts("Recursive Mutex test");
    puts("Please refer to the README.md for more information\n");

    rmutex_init(&testlock);

    /* lock mutex, so that spawned threads have to wait */
    rmutex_lock(&testlock);
    /* create threads */
    for (unsigned i = 0; i < THREAD_NUMOF; i++) {
        thread_create(stacks[i], sizeof(stacks[i]), prios[i], 0,
                      lockme, (void*)(intptr_t)depth[i], "t");
    }
    /* allow threads to lock the mutex */
    printf("main: unlocking recursive mutex\n");

    rmutex_unlock(&testlock);
    /* test 1 END */

    /* xtimer_rmutex_lock_timeout() test */
    /* lock mutex, so that spawned threads have to wait */
    rmutex_lock(&testlock);
    /* create threads */
    for (unsigned i = 0; i < THREAD_NUMOF; i++) {
        thread_create(stacks[i], sizeof(stacks[i]), prios[i], 0,
                      lockme_timeout, (void*)(intptr_t)timeouts[i], "t");
    }
    /* wait to have some thread timeouts */
    puts("main: waiting 250ms");
    xtimer_usleep(250 * US_PER_MS);
    /* allow threads to lock the mutex */
    printf("main: unlocking recursive mutex\n");

    rmutex_unlock(&testlock);

    puts("\nTest END, check the order of priorities above.");

    return 0;
}
