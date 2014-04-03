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

#define STACK_SIZE (KERNEL_CONF_STACKSIZE_MAIN)
#define PROBLEM 12

mutex_t mtx;

volatile int storage = 1;
int main_id;
int ths[PROBLEM];
char stacks[PROBLEM][STACK_SIZE];

void run(void)
{
    int err;
    int me = thread_getpid();
    printf("I am alive (%d)\n", me);
    msg_t arg;
    err = msg_receive(&arg);
    printf("Thread %d has arg %d\n", me, arg.content.value);

    err = mutex_lock(&mtx);

    if (err < 1) {
        printf("[!!!] mutex_lock failed with %d\n", err);
    }

    storage *= arg.content.value;
    mutex_unlock(&mtx);

    msg_t final;
    final.content.value = me;
    err = msg_send(&final, main_id, 1);

    if (err < 0) {
        printf("[!!!] Failed to send message from %d to main\n", me);
    }
}

int main(void)
{
    int err;
    main_id = thread_getpid();

    err = mutex_init(&mtx);

    if (err < 1) {
        printf("[!!!] mutex_init failed with %d\n", err);
    }

    printf("Problem: %" PRIu32 "\n", PROBLEM);

    msg_t args[PROBLEM];

    for (int i = 0; i < PROBLEM; ++i) {
        printf("Creating thread with arg %d\n", (i + 1));
        ths[i] = thread_create(stacks[i], STACK_SIZE, PRIORITY_MAIN - 1, CREATE_WOUT_YIELD | CREATE_STACKTEST, run, "thread");

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
        printf("Reveiced message %d from thread %d\n", i, msg.content.value);
    }

    printf("Factorial: %" PRIu32 "\n", storage);

    if (storage != 479001600) {
        puts("[!!!] Error, expected: 12!= 479001600.");
    }

    puts("finished");
    return 0;
}

