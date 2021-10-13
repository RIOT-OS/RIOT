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

#include "thread.h"
#include "pthread.h"
#include "mutex.h"

static char dummy_stack[MAXTHREADS][THREAD_STACKSIZE_IDLE];
static pthread_t pthread_ids[MAXTHREADS];
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

    pthread_attr_t th_attr;
    pthread_attr_init(&th_attr);

    int numthread_check = sched_num_threads;

    puts("Spawning pthreads");
    int pthread_cnt = 0;
    for (uint8_t i = 0; i < MAXTHREADS; i++) {
        pthread_attr_setstackaddr(&th_attr, &(dummy_stack[i]));
        pthread_attr_setstacksize(&th_attr, THREAD_STACKSIZE_IDLE);
        if (pthread_create(&(pthread_ids[i]), &th_attr, thread_func, NULL)) {
            break;
        }
        printf(".");
        pthread_cnt++;
    }
    puts("");

    int numthread_check_after = sched_num_threads;

    if ((numthread_check_after - numthread_check) != pthread_cnt ||
        numthread_check_after != MAXTHREADS) {
        printf("[ERROR] expected %d, ", (MAXTHREADS - numthread_check));
    }

    printf("created %d pthreads\n", pthread_cnt);
    printf("created %d threads\n", numthread_check_after - numthread_check);

    mutex_unlock(&testing_mutex);

    puts("wait for created pthreads to exit...");
    for (uint8_t i = 0; i < pthread_cnt; i++) {
        if (pthread_ids[i] != 0) {
            pthread_join(pthread_ids[i], NULL);
        }
    }
    puts("SUCCESS");

    return 0;
}
