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

#include "thread.h"
#include "mutex.h"

static mutex_t _shared_mutex;

void * thread_mutex_unlock(void * d)
{
    (void) d;
    thread_yield();
    puts("unlock mutex");
    mutex_unlock(&_shared_mutex);
    return NULL;
}

void * thread_bad(void * d)
{
    (void) d;
    for (;;) {
        /* i am a bad thread i do nothing and i do that all the time */
    }
}

/* each thread gets a stack */
static char stack[2][THREAD_STACKSIZE_DEFAULT];

int main(void)
{
    puts("starting threads");

    mutex_init(&_shared_mutex);
    thread_create(stack[0], sizeof(stack[0]), 10, THREAD_CREATE_STACKTEST,
                  thread_mutex_unlock, NULL, "TMutex");
    thread_create(stack[1], sizeof(stack[1]), 10, THREAD_CREATE_STACKTEST,
                  thread_bad, NULL, "TBad");

    puts("double locking mutex");

    mutex_lock(&_shared_mutex);
    mutex_lock(&_shared_mutex);

    /*success: mutex unlock thread got cpu time*/
    puts("[SUCCESS]");
}
