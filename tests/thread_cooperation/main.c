/*
 * Copyright (C) 2014-2017 Hamburg University of Applied Sciences
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
 * @brief riot thread test application
 *
 * @author Raphael Hiesgen <raphael.hiesgen@haw-hamburg.de>
 * @author Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "thread.h"
#include "mutex.h"

#ifndef PROBLEM
#define PROBLEM 12
#endif

mutex_t mtx = MUTEX_INIT;
volatile uint32_t storage = 1;
char stacks[PROBLEM][THREAD_STACKSIZE_DEFAULT];

void *run(void *arg)
{
    (void) arg;

    msg_t m, final;
    kernel_pid_t me = thread_getpid();

    printf("T-%02d: alive\n", me);

    msg_receive(&m);
    printf("T-%02d: got arg %" PRIu32 "\n", me, m.content.value);

    mutex_lock(&mtx);
    storage *= m.content.value;
    mutex_unlock(&mtx);

    final.content.value = me;

    if (msg_send(&final, m.sender_pid) < 0) {
        printf("T-%02d: send reply to main failed!\n", me);
    }

    return NULL;
}

int main(void)
{
    msg_t args[PROBLEM];
    kernel_pid_t ths;
    uint32_t factorial = 1;

    printf("[START] compute %d! (factorial).\n", PROBLEM);

    for (int i = 0; i < PROBLEM; ++i) {
        int arg = i + 1;
        factorial *= arg;
        printf("MAIN: create thread, arg: %d\n", arg);
        ths = thread_create(stacks[i], sizeof(stacks[i]),
                               THREAD_PRIORITY_MAIN - 1,
                               THREAD_CREATE_WOUT_YIELD | THREAD_CREATE_STACKTEST,
                               run, NULL, "thread");

        if (ths < 0)  {
            puts("[ERROR]");
            return 2;
        }

        printf("MAIN: msg to T-%d\n", ths);
        args[i].content.value = arg;
        if (msg_send(&args[i], ths) < 0) {
            puts("[ERROR]");
            return 3;
        }
    }

    for (int i = 0; i < PROBLEM; ++i) {
        msg_t msg;
        msg_receive(&msg);
        printf("MAIN: reply from T-%" PRIu32 "\n", msg.content.value);
    }

    printf("MAIN: %d! = %" PRIu32 "\n", PROBLEM, storage);

    if (storage != factorial) {
        printf("[ERROR] expected %" PRIu32 "\n", factorial);
        return 1;
    }
    puts("[SUCCESS]");

    return 0;
}
