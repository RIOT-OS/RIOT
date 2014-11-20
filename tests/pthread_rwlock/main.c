/*
 * Copyright (C) 2014  René Kijewski  <rene.kijewski@fu-berlin.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test rwlock implementation.
 *
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include <pthread.h>
#include <stdio.h>

#include "random.h"
#include "sched.h"
#include "thread.h"
#include "vtimer.h"

#define NUM_READERS_HIGH 2
#define NUM_READERS_LOW 3

#define NUM_WRITERS_HIGH 1
#define NUM_WRITERS_LOW 2

#define NUM_READERS (NUM_READERS_HIGH + NUM_READERS_LOW)
#define NUM_WRITERS (NUM_WRITERS_HIGH + NUM_WRITERS_LOW)
#define NUM_CHILDREN (NUM_READERS + NUM_WRITERS)

#define NUM_ITERATIONS 5

#define RAND_SEED 0xC0FFEE

static pthread_rwlock_t rwlock;
static volatile unsigned counter;

#define PRINTF(FMT, ...) \
    printf("%c%" PRIkernel_pid " (prio=%u): " FMT "\n", __func__[0], sched_active_pid, sched_active_thread->priority, __VA_ARGS__)

static void do_sleep(int factor)
{
    uint32_t timeout_us = (genrand_uint32() % 100000) * factor;
    /* PRINTF("sleep for % 8i µs.", timeout_us); */
    vtimer_usleep(timeout_us);
}

static void *writer(void *arg)
{
    (void) arg;
    /* PRINTF("%s", "start"); */
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        pthread_rwlock_wrlock(&rwlock);
        unsigned cur = ++counter;
        do_sleep(3); /* simulate time that it takes to write the value */
        PRINTF("%i: write -> %2u (correct = %u)", i, cur, cur == counter);
        pthread_rwlock_unlock(&rwlock);
        do_sleep(2);
    }
    /* PRINTF("%s", "done"); */
    return NULL;
}

static void *reader(void *arg)
{
    (void) arg;
    /* PRINTF("%s", "start"); */
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        pthread_rwlock_rdlock(&rwlock);
        unsigned cur = counter;
        do_sleep(1); /* simulate time that it takes to read the value */
        PRINTF("%i: read  <- %2u (correct = %u)", i, cur, cur == counter);
        pthread_rwlock_unlock(&rwlock);
        do_sleep(1);
    }
    /* PRINTF("%s", "done"); */
    return NULL;
}

int main(void)
{
    static char stacks[NUM_CHILDREN][KERNEL_CONF_STACKSIZE_MAIN];

    puts("Main start.");

    for (unsigned i = 0; i < NUM_CHILDREN; ++i) {
        int prio;
        void *(*fun)(void *);
        const char *name;

        if (i < NUM_READERS) {
            if (i < NUM_READERS_HIGH) {
                prio = PRIORITY_MAIN + 1;
            }
            else {
                prio = PRIORITY_MAIN + 2;
            }
            fun = reader;
            name = "reader";
        }
        else {
            if (i - NUM_READERS < NUM_WRITERS_HIGH) {
                prio = PRIORITY_MAIN + 1;
            }
            else {
                prio = PRIORITY_MAIN + 2;
            }
            fun = writer;
            name = "writer";
        }

        thread_create(stacks[i], sizeof(stacks[i]),
                      prio, CREATE_WOUT_YIELD | CREATE_STACKTEST,
                      fun, NULL, name);
    }

    puts("Main done.");
    return 0;
}
