/*
 * SPDX-FileCopyrightText: 2014 Hamburg University of Applied Sciences (HAW)
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief   simple condition variable test application
 *
 * @author  Martin Landsmann <martin.landsmann@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>

#include "buildinfo/cpu.h"
#include "pthread.h"
#include "thread.h"

static mutex_t mutex = MUTEX_INIT;
static pthread_cond_t cv;
static volatile int is_finished;
static volatile long count;
static char stack[THREAD_STACKSIZE_MAIN];

/**
 * @brief   This thread tries to lock the mutex to enter the critical section.
 *          Then it signals one waiting thread to check the condition and it goes to sleep again
 *          If is_finished is set to 1 second_thread ends
 */
static void *second_thread(void *arg)
{
    (void) arg;
    while (1) {
        mutex_lock(&mutex);

        if (is_finished == 1) {
            break;
        }

        pthread_cond_signal(&cv);
        mutex_unlock_and_sleep(&mutex);
    }
    return NULL;
}

#ifdef CPU_NATIVE
#define ITERATION_STEPS 100000
#else
#define ITERATION_STEPS 10000
#endif

int main(void)
{
    puts("START");
    count = 0;
    is_finished = 0;
    pthread_cond_init(&cv, NULL);

    kernel_pid_t pid = thread_create(stack,sizeof(stack), THREAD_PRIORITY_MAIN - 1,
                                     THREAD_CREATE_WOUT_YIELD,
                                     second_thread, NULL, "second_thread");

    while (1) {
        mutex_lock(&mutex);
        thread_wakeup(pid);
        count++;

        if ((count % ITERATION_STEPS) == 0) {
            printf("Still alive alternated [count: %ldk] times.\n", count / 1000);
        }

        if (count == (10ul * ITERATION_STEPS)) {
            puts("condition fulfilled.");
            is_finished = 1;
            mutex_unlock(&mutex);
            break;
        }

        pthread_cond_wait(&cv, &mutex);
        mutex_unlock(&mutex);
    }

    puts("SUCCESS");

    return 0;
}
