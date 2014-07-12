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
#include <thread.h>
#include <flags.h>
#include <kernel.h>
#include <mutex.h>

#define PROBLEM 12

mutex_t mtx;

volatile int storage = 1;
int main_id;
int ths[PROBLEM];
char stacks[PROBLEM][KERNEL_CONF_STACKSIZE_MAIN];

void *run(void *arg)
{
    (void) arg;

    int err;
    int me = thread_getpid();
    printf("I am alive (%d)\n", me);
    msg_t m;
    err = msg_receive(&m);
    printf("Thread %d has arg %" PRIu32 "\n", me, m.content.value);

    err = mutex_lock(&mtx);

    if (err < 1) {
        printf("[!!!] mutex_lock failed with %d\n", err);
    }

    storage *= m.content.value;
    mutex_unlock(&mtx);

    msg_t final;
    final.content.value = me;
    err = msg_send(&final, main_id, 1);

    if (err < 0) {
        printf("[!!!] Failed to send message from %d to main\n", me);
    }

    return NULL;
}

int main(void)
{
    int err;
    main_id = thread_getpid();

    err = mutex_init(&mtx);

    if (err < 1) {
        printf("[!!!] mutex_init failed with %d\n", err);
    }

    printf("Problem: %d\n", PROBLEM);

    msg_t args[PROBLEM];

    for (int i = 0; i < PROBLEM; ++i) {
        printf("Creating thread with arg %d\n", (i + 1));
        ths[i] = thread_create(stacks[i], sizeof(stacks[i]),
                               PRIORITY_MAIN - 1, CREATE_WOUT_YIELD | CREATE_STACKTEST,
                               run, NULL, "thread");

        if (ths[i] < 0)  {
            printf("[!!!] Creating thread failed with %d\n", err);
        }
        else {
            args[i].content.value = i + 1;
            err = msg_send(&args[i], ths[i], 1);

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

