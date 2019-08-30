/*
 * Copyright (C) 2019 Freie Universität Berlin,
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
 * @brief pthread test application
 *
 * Spawns pthreads till the scheduler's capacity is exhausted.
 *
 * @author      Julian Holzwarth <julian.holzwarth@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "kernel_types.h"
#include "thread.h"
#include "pthread.h"
#include "mutex.h"

static char dummy_stack[MAXTHREADS][THREAD_STACKSIZE_IDLE];
static mutex_t testing_mutex;

static void *thread_func(void *arg)
{
    (void)arg;
    mutex_lock(&testing_mutex);
    mutex_unlock(&testing_mutex);
    return NULL;
}

int main(void)
{
    mutex_init(&testing_mutex);
    mutex_lock(&testing_mutex);


    int pthread_cnt = 0;

    pthread_t pthread_ids[MAXTHREADS];

    pthread_attr_t th_attr;
    pthread_attr_init(&th_attr);

    if (-1 ==
        pthread_create(&(pthread_ids[pthread_cnt % MAXTHREADS]), &th_attr, thread_func,
                       NULL)) {
        puts("[ERROR] cannot create pthreads");
        return 0;
    }
    volatile int numthread_check = sched_num_threads;

    int exit_loop  = -1;

    puts("[START] Spawning threads");
    do {
        pthread_attr_setstackaddr(&th_attr,
                                  &(dummy_stack[pthread_cnt % MAXTHREADS]));
        pthread_attr_setstacksize(&th_attr, THREAD_STACKSIZE_IDLE);
        exit_loop = pthread_create(&(pthread_ids[pthread_cnt + 1 % MAXTHREADS]), &th_attr,
                                   thread_func, NULL);
        if (exit_loop == 0) {
            ++pthread_cnt;
            printf(".");
        }
    } while (-1 != exit_loop);
    volatile int numthread_check_after = sched_num_threads;

    puts("");
    if (numthread_check_after - numthread_check == pthread_cnt &&
        numthread_check_after == MAXTHREADS) {
        printf("[SUCCESS]\n");
    }
    else {
        printf("[ERROR] expected %d,", (MAXTHREADS - numthread_check));
    }
    printf("created %d pthreads\n", pthread_cnt);
    printf("created %d threads\n", numthread_check_after - numthread_check);
    mutex_unlock(&testing_mutex);

    for (int i = 0; i < pthread_cnt; i++) {
        if (pthread_ids[i] != 0) {
            pthread_join(pthread_ids[i], NULL);
        }
    }
    puts("test end");

    return 0;
}
