/*
 * Copyright (C) 2013 Christian Mehlis <mehlis@inf.fu-berlin.de>
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
 * @brief Thread test application
 *
 * @author Christian Mehlis <mehlis@inf.fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "thread.h"
#include "msg.h"

char t1_stack[THREAD_STACKSIZE_MAIN];
char t2_stack[THREAD_STACKSIZE_MAIN];
char t3_stack[THREAD_STACKSIZE_MAIN];

kernel_pid_t p1, p2, p3;

void *thread1(void *arg)
{
    (void) arg;
    puts("THREAD 1 start\n");

    for (int i = 0; i < 3; ++i) {
        msg_t msg, reply;

        msg_receive(&msg);
        printf("T1 recv: %" PRIu32 "(i=%d)\n", msg.content.value, i);

        msg.content.value = i;
        msg_send_receive(&msg, &reply, p2);
        printf("T1 got reply: %" PRIu32 " (i=%d)\n", reply.content.value, i);
    }

    puts("THREAD 1 end\n");
    return NULL;
}

void *thread2(void *arg)
{
    (void) arg;
    puts("THREAD 2\n");

    for (int i = 0;; ++i) {
        msg_t msg, reply;

        msg_receive(&msg);
        printf("T2 got %" PRIu32 " (i=%d)\n", msg.content.value, i);
        reply.content.value = msg.content.value;
        msg_reply(&msg, &reply);
    }

    return NULL;
}

void *thread3(void *arg)
{
    (void) arg;
    puts("THREAD 3\n");

    for (int i = 0;; ++i) {
        msg_t msg;
        msg.content.value = i;
        printf("T3 i=%d\n", i);
        msg_send(&msg, p1);
    }
    return NULL;
}

int main(void)
{
    p1 = thread_create(t1_stack, sizeof(t1_stack), THREAD_PRIORITY_MAIN - 1,
                       CREATE_WOUT_YIELD | CREATE_STACKTEST,
                       thread1, NULL, "nr1");
    p2 = thread_create(t2_stack, sizeof(t2_stack), THREAD_PRIORITY_MAIN - 1,
                       CREATE_WOUT_YIELD | CREATE_STACKTEST,
                       thread2, NULL, "nr2");
    p3 = thread_create(t3_stack, sizeof(t3_stack), THREAD_PRIORITY_MAIN - 1,
                       CREATE_WOUT_YIELD | CREATE_STACKTEST,
                       thread3, NULL, "nr3");
    puts("THREADS CREATED\n");
    return 0;
}
