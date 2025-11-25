/*
 * Copyright (C) 2015-2017 Hamburg University of Applied Sciences
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
 * @brief Thread flooding test.
 *
 * Spawns sleeping threads till the scheduler's capacity is exhausted.
 *
 * @author  Andreas "Paul" Pauli <andreas.pauli@haw-hamburg.de>
 * @author  Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

#include <errno.h>
#include <stdio.h>

#include "thread.h"
#include "sched.h"

/* One stack for all threads. DON'T TRY THIS AT HOME!! */
static char dummy_stack[THREAD_STACKSIZE_IDLE];

static void *thread_func(void *arg)
{
    return arg;
}

int main(void)
{
    kernel_pid_t thr_id = KERNEL_PID_UNDEF;
    unsigned thr_cnt = 0;
    unsigned thr_in_use = sched_num_threads;

    puts("[START] Spawning threads");
    do {
        thr_id = thread_create(
            dummy_stack, sizeof(dummy_stack),
            THREAD_PRIORITY_MAIN - 1,
            THREAD_CREATE_SLEEPING,
            thread_func, NULL, "dummy");
            ++thr_cnt;
            printf(".");
    } while (-EOVERFLOW != thr_id);
    puts("");
    /* decrease by 1 because last thread_create failed */
    --thr_cnt;

    if (thr_cnt == (MAXTHREADS - thr_in_use)) {
        printf("[SUCCESS]");
    }
    else {
        printf("[ERROR] expected %u,", (MAXTHREADS - thr_in_use));
    }
    printf(" created %u\n", thr_cnt);

    return 0;
}
