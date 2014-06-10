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
 * @brief       Test sched_set_priority()
 *
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "kernel.h"
#include "pthread.h"
#include "thread.h"

#define NUM_CHILDREN 4
#define NUM_ITERATIONS 5

static pthread_barrier_t barrier;

static char stacks[NUM_CHILDREN][KERNEL_CONF_STACKSIZE_MAIN];
static int pids[NUM_CHILDREN];
static volatile int last_prio;

static void *run(void *arg)
{
    (void) arg;

    for (unsigned n = 0; n < NUM_ITERATIONS; ++n) {
        pthread_barrier_wait(&barrier);
        printf(" * %2u. id: %2d, priority: %2d, last_prio: %d\n",
               n, sched_active_pid, sched_active_thread->priority, last_prio);

        if (last_prio >= sched_active_thread->priority) {
            printf("id: %2d !!! TEST FAILED !!!\n", sched_active_pid);
        }

        last_prio = sched_active_thread->priority;
    }

    return 0;
}

int main(void)
{
    puts("Start.\n");
    pthread_barrier_init(&barrier, NULL, NUM_CHILDREN + 1);

    for (unsigned i = 0; i < NUM_CHILDREN; ++i) {
        pids[i] = thread_create(stacks[i], sizeof(stacks[i]),
                                PRIORITY_MAIN - 1 - i, CREATE_WOUT_YIELD | CREATE_STACKTEST,
                                run, NULL, "child");
    }
    thread_yield();

    for (unsigned n = 0; n < NUM_ITERATIONS - 1; ++n) {
        last_prio = 0;
        pthread_barrier_wait(&barrier);
        puts("");
        for (unsigned i = 0; i < NUM_CHILDREN; ++i) {
            unsigned new_priority = PRIORITY_MAIN - 1 - ((n + i + 1) % NUM_CHILDREN);
            printf("New priority for %2d: %2d\n", pids[i], new_priority);
            sched_set_priority((tcb_t *) sched_threads[pids[i]], new_priority);
        }
    }
    last_prio = 0;
    pthread_barrier_wait(&barrier);

    pthread_barrier_destroy(&barrier);
    puts("\nDone.");
    return 0;
}
