/*
 * Copyright (C) 2014 Hamburg University of Applied Sciences
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
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "thread.h"
#include "mutex.h"

#define PROBLEM 12

mutex_t mtx = MUTEX_INIT;

volatile int storage = 1;
kernel_pid_t main_id = KERNEL_PID_UNDEF;
kernel_pid_t ths[PROBLEM];
char stacks[PROBLEM][THREAD_STACKSIZE_MAIN];

void *run(void *arg)
{
    (void) arg;

    kernel_pid_t me = thread_getpid();
    printf("I am alive (%d)\n", me);
    msg_t m;
    msg_receive(&m);
    printf("Thread %d has arg %" PRIu32 "\n", me, m.content.value);

    mutex_lock(&mtx);

    storage *= m.content.value;
    mutex_unlock(&mtx);

    msg_t final;
    final.content.value = me;
    int err = msg_send(&final, main_id);

    if (err < 0) {
        printf("[!!!] Failed to send message from %d to main\n", me);
    }

    return NULL;
}

int main(void)
{
    main_id = thread_getpid();

    printf("Problem: %d\n", PROBLEM);

    msg_t args[PROBLEM];

    for (int i = 0; i < PROBLEM; ++i) {
        printf("Creating thread with arg %d\n", (i + 1));
        ths[i] = thread_create(stacks[i], sizeof(stacks[i]),
                               THREAD_PRIORITY_MAIN - 1,
                               THREAD_CREATE_WOUT_YIELD | THREAD_CREATE_STACKTEST,
                               run, NULL, "thread");

        if (ths[i] < 0)  {
            printf("[!!!] Creating thread failed.\n");
        }
        else {
            args[i].content.value = i + 1;

            int err = msg_send(&args[i], ths[i]);
            if (err < 0) {
                printf("[!!!] Sending message to thread %d failed\n", ths[i]);
            }
        }
    }

    for (int i = 0; i < PROBLEM; ++i) {
        msg_t msg;
        msg_receive(&msg);
        printf("Reveiced message %d from thread %" PRIu32 "\n", i, msg.content.value);
    }

    printf("Factorial: %d\n", storage);

    if (storage != 479001600) {
        puts("[!!!] Error, expected: 12!= 479001600.");
    }

    puts("finished");
    return 0;
}
