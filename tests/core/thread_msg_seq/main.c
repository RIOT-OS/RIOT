/*
 * SPDX-FileCopyrightText: 2013 Christian Mehlis <mehlis@inf.fu-berlin.de>
 * SPDX-FileCopyrightText: 2014 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief Thread test application
 *
 * @author Christian Mehlis <mehlis@inf.fu-berlin.de>
 * @author Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>

#include "thread.h"
#include "msg.h"

char t1_stack[THREAD_STACKSIZE_MAIN];
char t2_stack[THREAD_STACKSIZE_MAIN];
char t3_stack[THREAD_STACKSIZE_MAIN];

kernel_pid_t p_main = KERNEL_PID_UNDEF, p1 = KERNEL_PID_UNDEF,
             p2 = KERNEL_PID_UNDEF, p3 = KERNEL_PID_UNDEF;

void *sub_thread(void *arg)
{
    kernel_pid_t pid = thread_getpid();
    printf("THREAD %s (pid:%" PRIkernel_pid ") start\n", (char*) arg, pid);

    msg_t msg;

    msg.content.ptr = arg;

    msg_send(&msg, p_main);

    printf("THREAD %s (pid:%" PRIkernel_pid ") end.\n", (char*) arg, pid);

    return NULL;
}

int main(void)
{
    puts("START");
    msg_t msg;

    p_main = thread_getpid();

    p1 = thread_create(t1_stack, sizeof(t1_stack), THREAD_PRIORITY_MAIN - 1,
                       0, sub_thread, "nr1", "nr1");

    p2 = thread_create(t2_stack, sizeof(t2_stack), THREAD_PRIORITY_MAIN - 1,
                       0, sub_thread, "nr2", "nr2");

    p3 = thread_create(t3_stack, sizeof(t3_stack), THREAD_PRIORITY_MAIN - 1,
                       0, sub_thread, "nr3", "nr3");
    puts("THREADS CREATED\n");

    for (int i = 0; i < 3; i++) {
        msg_receive(&msg);
        printf("Got msg from pid %" PRIkernel_pid ": \"%s\"\n", msg.sender_pid, (char *)msg.content.ptr);
    }

    puts("SUCCESS");

    return 0;
}
