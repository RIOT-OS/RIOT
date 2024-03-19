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

#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>

#include "rmutex.h"
#include "thread.h"

#define THREAD_NUMOF            (5U)

static char stacks[THREAD_NUMOF][THREAD_STACKSIZE_SMALL + THREAD_EXTRA_STACKSIZE_PRINTF];

static const uint8_t prios[THREAD_NUMOF] = {THREAD_PRIORITY_MAIN - 1, 4, 5, 2, 4};
static const uint8_t depth[THREAD_NUMOF] = {5, 3, 3, 4, 5};

static rmutex_t testlock;

static void lock_recursive(uint8_t current_depth, uint8_t max_depth)
{
    thread_t *t = thread_get_active();

    printf("T%i (prio %i, depth %i): trying to lock rmutex now\n",
        (int)t->pid, (int)t->priority, (int)current_depth);
    rmutex_lock(&testlock);

    printf("T%i (prio %i, depth %i): locked rmutex now\n",
           (int)t->pid, (int)t->priority, (int)current_depth);

    if (current_depth + 1 < max_depth) {
        lock_recursive(current_depth + 1, max_depth);
    }

    thread_yield();

    rmutex_unlock(&testlock);

    printf("T%i (prio %i, depth %i): unlocked rmutex\n",
           (int)t->pid, (int)t->priority, (int)current_depth);
}

static void *lockme(void *arg)
{
    uintptr_t depth = (uintptr_t)arg;

    lock_recursive(0, depth);

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

    rmutex_lock(&testlock);
    puts("\nTest END, check the order of priorities above.");

    return 0;
}
