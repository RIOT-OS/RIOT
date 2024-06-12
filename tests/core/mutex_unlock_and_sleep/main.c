/*
 * Copyright (C) 2014-2017 HAW Hamburg
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
 * @author  Sebastian Meiling <s@mlng.net>
 * @}
 */

#include <stdio.h>
#include "thread.h"
#include "mutex.h"

static mutex_t mutex = MUTEX_INIT;
static volatile int indicator;
static kernel_pid_t main_pid;
static char stack[THREAD_STACKSIZE_DEFAULT];

#ifdef CPU_NATIVE
static const unsigned KITERATIONS = 100;
#else
static const unsigned KITERATIONS = 10;
#endif

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
    uint32_t count = 0;
    uint32_t kcount = 0;

    indicator = 0;
    main_pid = thread_getpid();

    kernel_pid_t second_pid = thread_create(stack,
                  sizeof(stack),
                  THREAD_PRIORITY_MAIN - 1,
                  THREAD_CREATE_WOUT_YIELD,
                  second_thread,
                  NULL,
                  "second_thread");

    while (1) {
        mutex_lock(&mutex);
        thread_wakeup(second_pid);
        indicator++;
        count++;

        if ((indicator > 1) || (indicator < -1)) {
            printf("[ERROR] threads did not sleep properly (%d).\n", indicator);
            return 1;
        }
        if (count == (KITERATIONS * 1000)) {
            count = 0;
            kcount += KITERATIONS;
            printf("[ALIVE] alternated %"PRIu32"k times.\n", kcount);
        }
        mutex_unlock_and_sleep(&mutex);
    }
}
