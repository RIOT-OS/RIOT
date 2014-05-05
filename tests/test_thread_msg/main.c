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

char t1_stack[KERNEL_CONF_STACKSIZE_PRINTF];
char t2_stack[KERNEL_CONF_STACKSIZE_PRINTF];
char t3_stack[KERNEL_CONF_STACKSIZE_PRINTF];

uint16_t p1, p2, p3;

void thread1(void)
{
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
}

void thread2(void)
{
    puts("THREAD 2\n");

    for (int i = 0;; ++i) {
        msg_t msg, reply;

        msg_receive(&msg);
        printf("T2 got %" PRIu32 " (i=%d)\n", msg.content.value, i);
        reply.content.value = msg.content.value;
        msg_reply(&msg, &reply);
    }
}

void thread3(void)
{
    puts("THREAD 3\n");

    for (int i = 0;; ++i) {
        msg_t msg;
        msg.content.value = i;
        printf("T3 i=%d\n", i);
        msg_send(&msg, p1, 1);
    }
}

int main(void)
{
    p1 = thread_create(t1_stack, KERNEL_CONF_STACKSIZE_PRINTF, PRIORITY_MAIN - 1,
                       CREATE_WOUT_YIELD | CREATE_STACKTEST, thread1, "nr1");
    p2 = thread_create(t2_stack, KERNEL_CONF_STACKSIZE_PRINTF, PRIORITY_MAIN - 1,
                       CREATE_WOUT_YIELD | CREATE_STACKTEST, thread2, "nr2");
    p3 = thread_create(t3_stack, KERNEL_CONF_STACKSIZE_PRINTF, PRIORITY_MAIN - 1,
                       CREATE_WOUT_YIELD | CREATE_STACKTEST, thread3, "nr3");
    puts("THREADS CREATED\n");
}
