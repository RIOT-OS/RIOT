/*
 * Copyright (C) 2020 TUBA Freiberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
#include "mutex.h"

static mutex_t _shared_mutex;

void * thread_mutex_unlock(void *d)
{
    (void) d;
    puts("mutex_thread yield");
    thread_yield();
    puts("unlock mutex");
    mutex_unlock(&_shared_mutex);
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

/* with priority inversion this should be set to THREAD_PRIORITY_MAIN
 * until then a lower priority (higher number) is the better choice */
const uint8_t shared_prio = THREAD_PRIORITY_MAIN + 1;

int main(void)
{
    puts("starting threads");

    mutex_init(&_shared_mutex);
    thread_create(stack[0], sizeof(stack[0]), shared_prio, THREAD_CREATE_STACKTEST,
                  thread_mutex_unlock, NULL, "TMutex");
    thread_create(stack[1], sizeof(stack[1]), shared_prio, THREAD_CREATE_STACKTEST,
                  thread_bad, NULL, "TBad");

    puts("double locking mutex");

    mutex_lock(&_shared_mutex);
    mutex_lock(&_shared_mutex);

    /* success: mutex got unlocked, which means thread "TMutex" got cpu time
     * even though "TBad" was trying to hog the whole CPU */
    puts("[SUCCESS]");
}
