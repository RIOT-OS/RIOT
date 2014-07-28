/*
* Copyright (C) 2014 Hamburg University of Applied Sciences
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

mutex_t mtx;

volatile int storage = 1;
int main_id;
thread_t threads[PROBLEM];
char stacks[PROBLEM][KERNEL_CONF_STACKSIZE_MAIN];

void *run(void *arg)
{
    (void) arg;

    int me = thread_getpid();
    printf("I am alive (%d)\n", me);
    msg_t m;
    msg_receive(&m);
    printf("Thread %d has arg %" PRIu32 "\n", me, m.content.value);

    mutex_lock(&mtx);

    storage *= m.content.value;
    mutex_unlock(&mtx);

    msg_t final;
    final.content.value = me;
    int err = msg_send(&final, main_id, 1);

    if (err < 0) {
        printf("[!!!] Failed to send message from %d to main\n", me);
    }

    return NULL;
}

int main(void)
{
    main_id = thread_getpid();

    mutex_init(&mtx);

    printf("Problem: %d\n", PROBLEM);

    msg_t args[PROBLEM];

    for (int i = 0; i < PROBLEM; ++i) {
        printf("Creating thread with arg %d\n", (i + 1));
        int err = thread_create(&threads[i], stacks[i], sizeof(stacks[i]),
                                PRIORITY_MAIN - 1, CREATE_WOUT_YIELD | CREATE_STACKTEST,
                                run, NULL, "thread");

        if (err < 0)  {
            printf("[!!!] Creating thread failed with %d\n", err);
        }
        else {
            args[i].content.value = i + 1;
            err = msg_send(&args[i], threads[i].pid, 1);

            if (err < 0) {
                printf("[!!!] Sending message to thread %d failed\n", threads[i].pid);
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

