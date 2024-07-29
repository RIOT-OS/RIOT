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
#include "xtimer.h"

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

static kernel_pid_t main_thread_pid;

/* The test assumes that 'printf/puts' are non interruptible operations
 * use a mutex to guarantee it */
static mutex_t stdout_mutex = MUTEX_INIT;

#define PRINTF(FMT, ...)                                    \
    do {                                                    \
        mutex_lock(&stdout_mutex);                          \
        printf("%c%" PRIkernel_pid " (prio=%u): " FMT "\n", \
               __func__[0], thread_getpid(),                \
               thread_get_active()->priority,                \
               (int)__VA_ARGS__);                           \
        mutex_unlock(&stdout_mutex);                        \
    } while (0)

#define PUTS(s)                                             \
    do {                                                    \
        mutex_lock(&stdout_mutex);                          \
        puts(s);                                            \
        mutex_unlock(&stdout_mutex);                        \
    } while (0)

static void _notify_main_thread(void)
{
    msg_t msg;
    msg_send(&msg, main_thread_pid);
}

static void do_sleep(int factor)
{
    uint32_t timeout_us = (random_uint32() % 100000) * factor;
    PRINTF("sleep for % 8i µs.", timeout_us);
    xtimer_usleep(timeout_us);
}

static void *writer(void *arg)
{
    (void) arg;
    PUTS("start");
    for (unsigned i = 0; i < NUM_ITERATIONS; ++i) {
        pthread_rwlock_wrlock(&rwlock);
        unsigned cur = ++counter;
        do_sleep(3); /* simulate time that it takes to write the value */
        PRINTF("%i: write -> %2u (correct = %u)", i, cur, cur == counter);
        pthread_rwlock_unlock(&rwlock);
        do_sleep(2);
    }
    PUTS("done");
    _notify_main_thread();
    return NULL;
}

static void *reader(void *arg)
{
    (void) arg;
    PUTS("start");
    for (unsigned i = 0; i < NUM_ITERATIONS; ++i) {
        pthread_rwlock_rdlock(&rwlock);
        unsigned cur = counter;
        do_sleep(1); /* simulate time that it takes to read the value */
        PRINTF("%i: read  <- %2u (correct = %u)", i, cur, cur == counter);
        pthread_rwlock_unlock(&rwlock);
        do_sleep(1);
    }
    PUTS("done");
    _notify_main_thread();

    return NULL;
}

int main(void)
{
    static char stacks[NUM_CHILDREN][THREAD_STACKSIZE_MAIN];

    PUTS("START");
    /* Get main thread pid */
    main_thread_pid = thread_getpid();

    for (unsigned i = 0; i < NUM_CHILDREN; ++i) {
        int prio;
        void *(*fun)(void *);
        const char *name;

        if (i < NUM_READERS) {
            if (i < NUM_READERS_HIGH) {
                prio = THREAD_PRIORITY_MAIN + 1;
            }
            else {
                prio = THREAD_PRIORITY_MAIN + 2;
            }
            fun = reader;
            name = "reader";
        }
        else {
            if (i - NUM_READERS < NUM_WRITERS_HIGH) {
                prio = THREAD_PRIORITY_MAIN + 1;
            }
            else {
                prio = THREAD_PRIORITY_MAIN + 2;
            }
            fun = writer;
            name = "writer";
        }

        thread_create(stacks[i], sizeof(stacks[i]),
                      prio, THREAD_CREATE_WOUT_YIELD,
                      fun, NULL, name);
    }

    /* Block until all children threads are done */
    for (unsigned i = 0; i < NUM_CHILDREN; ++i) {
        msg_t msg;
        msg_receive(&msg);
    }

    PUTS("SUCCESS");

    return 0;
}
