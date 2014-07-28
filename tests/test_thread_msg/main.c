/*
 * Copyright (C) 2013 Christian Mehlis <mehlis@inf.fu-berlin.de>
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
 * @brief Thread test application
 *
 * @author Christian Mehlis <mehlis@inf.fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "thread.h"
#include "msg.h"

thread_t t1, t2, t3;
char t1_stack[KERNEL_CONF_STACKSIZE_MAIN];
char t2_stack[KERNEL_CONF_STACKSIZE_MAIN];
char t3_stack[KERNEL_CONF_STACKSIZE_MAIN];

void *thread1(void *arg)
{
    (void) arg;
    puts("THREAD 1 start\n");

    for (int i = 0; i < 3; ++i) {
        msg_t msg, reply;

        msg_receive(&msg);
        printf("T1 recv: %" PRIu32 "(i=%d)\n", msg.content.value, i);

        msg.content.value = i;
        msg_send_receive(&msg, &reply, t2.pid);
        printf("T1 got reply: %" PRIu32 " (i=%d)\n", reply.content.value, i);
    }

    puts("THREAD 1 end\n");
    return NULL;
}

void *thread2(void *arg)
{
    (void) arg;
    puts("THREAD 2\n");

    for (int i = 0; /* for ever */; ++i) {
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

    for (int i = 0; /* for ever */; ++i) {
        msg_t msg;
        msg.content.value = i;
        printf("T3 i=%d\n", i);
        msg_send(&msg, t1.pid, 1);
    }
    return NULL;
}

int main(void)
{
    thread_create(&t1, t1_stack, sizeof(t1_stack), PRIORITY_MAIN - 1,
                  CREATE_WOUT_YIELD | CREATE_STACKTEST,
                  thread1, NULL, "nr1");
    thread_create(&t2, t2_stack, sizeof(t2_stack), PRIORITY_MAIN - 1,
                  CREATE_WOUT_YIELD | CREATE_STACKTEST,
                  thread2, NULL, "nr2");
    thread_create(&t3, t3_stack, sizeof(t3_stack), PRIORITY_MAIN - 1,
                  CREATE_WOUT_YIELD | CREATE_STACKTEST,
                  thread3, NULL, "nr3");
    puts("THREADS CREATED\n");
    return 0;
}
