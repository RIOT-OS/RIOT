/*
 * SPDX-FileCopyrightText: 2020 TUBA Freiberg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 * @file
 * @brief       Test sys/sched_round_robin
 * @author      Karl Fessel <karl.fessel@ovgu.de>
 * @}
 */

#include <stdio.h>
#include <stdint.h>

#include "thread.h"

static kernel_pid_t main_pid;

void * thread_wakeup_main(void *d)
{
    (void) d;
    puts("wakup_thread yield");
    thread_yield();
    while (puts("wakeup main"), thread_wakeup(main_pid) == (int)STATUS_NOT_FOUND) {
        thread_yield();
    };
    return NULL;
}

void * thread_bad(void *d)
{
    (void) d;
    puts("bad thread looping");
    for (;;) {
        /* I'm a bad thread I do nothing and I do that all the time */
    }
}

/* each thread gets a stack */
static char stack[2][THREAD_STACKSIZE_DEFAULT];

/* shared priority of the threads - lower than main waiting for it to sleep */
static const uint8_t shared_prio = THREAD_PRIORITY_MAIN + 1;

int main(void)
{
    puts("starting threads");
    main_pid = thread_getpid();
    thread_create(stack[0], sizeof(stack[0]), shared_prio, 0,
                  thread_wakeup_main, NULL, "TWakeup");
    thread_create(stack[1], sizeof(stack[1]), shared_prio, 0,
                  thread_bad, NULL, "TBad");
    puts("main is going to sleep");
    thread_sleep();

    /* success: main got woken up again which means "TWakup" got cpu time
     * even though "TBad" was trying to hog the whole CPU */
    puts("[SUCCESS]");
}
