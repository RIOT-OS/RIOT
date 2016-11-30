/*
 * Copyright (C) 2014 Hamburg University of Applied Siences (HAW)
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
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
#include "condition_variable.h"
#include "thread.h"
#include "mutex.h"

static mutex_t mutex;
static struct pthread_cond_t cv;
static volatile int count;
static char stack[KERNEL_CONF_STACKSIZE_MAIN];

/**
 * @brief   This thread tries to lock the mutex to enter the critical section.
 *          Then it sets cv true (==1), signals one waiting thread to check cv state and sleep
 */
static void second_thread(void)
{
    while (1) {
        mutex_lock(&mutex);
        cv.val = 1;
        pthread_cond_signal(&cv);
        thread_sleep();
        mutex_unlock(&mutex);
    }
}

int main(void)
{
    count = 0;
    mutex_init(&mutex);
    pthread_cond_init(&cv, NULL);

    int pid = thread_create(stack,
                            KERNEL_CONF_STACKSIZE_MAIN,
                            PRIORITY_MAIN - 1,
                            CREATE_WOUT_YIELD | CREATE_STACKTEST,
                            second_thread,
                            "second_thread");

    while (1) {
        mutex_lock(&mutex);
        thread_wakeup(pid);
        count++;

        if ((count % 100000) == 0) {
            printf("Still alive alternated [count: %dk] times.\n", count / 1000);
        }

        pthread_cond_wait(&cv, &mutex);
        cv.val = 0;
        mutex_unlock(&mutex);
    }
}
