/*
 * Copyright (C) 2014 Hamburg University of Applied Siences (HAW)
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
 * @brief   simple test application for atomic mutex unlocking and sleeping
 *
 * @author  Martin Landsmann <martin.landsmann@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include "thread.h"
#include "mutex.h"

static mutex_t mutex = MUTEX_INIT;
static volatile int indicator, count;
static kernel_pid_t main_pid;

static char stack[THREAD_STACKSIZE_MAIN];
static void *second_thread(void *arg)
{
    (void) arg;
    while (1) {
        mutex_lock(&mutex);
        thread_wakeup(main_pid);
        indicator--;
        mutex_unlock_and_sleep(&mutex);
    }
    return NULL;
}

int main(void)
{
    indicator = 0;
    count = 0;

    main_pid = thread_getpid();

    kernel_pid_t second_pid = thread_create(stack,
                  sizeof(stack),
                  THREAD_PRIORITY_MAIN - 1,
                  THREAD_CREATE_WOUT_YIELD | THREAD_CREATE_STACKTEST,
                  second_thread,
                  NULL,
                  "second_thread");

    while (1) {
        mutex_lock(&mutex);
        thread_wakeup(second_pid);
        indicator++;
        count++;

        if (indicator > 1 || indicator < -1) {
            printf("Error, threads did not sleep properly. [indicator: %d]\n", indicator);
            return -1;
        }

        if ((count % 100000) == 0) {
            printf("Still alive alternated [count: %dk] times.\n", count / 1000);
        }

        mutex_unlock_and_sleep(&mutex);
    }
}
