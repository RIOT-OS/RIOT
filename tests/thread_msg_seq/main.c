/*
 * Copyright (C) 2013 Christian Mehlis <mehlis@inf.fu-berlin.de>
 *           (C) 2014 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @author Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>

#include "thread.h"
#include "msg.h"

char t1_stack[KERNEL_CONF_STACKSIZE_MAIN];
char t2_stack[KERNEL_CONF_STACKSIZE_MAIN];
char t3_stack[KERNEL_CONF_STACKSIZE_MAIN];

kernel_pid_t p_main = KERNEL_PID_UNDEF, p1 = KERNEL_PID_UNDEF,
             p2 = KERNEL_PID_UNDEF, p3 = KERNEL_PID_UNDEF;

void *sub_thread(void *arg)
{
    kernel_pid_t pid = thread_getpid();
    printf("THREAD %s (pid:%" PRIkernel_pid ") start\n", (char*) arg, pid);

    msg_t msg;

    msg.content.ptr = (char*) arg;

    msg_send(&msg, p_main);

    printf("THREAD %s (pid:%" PRIkernel_pid ") end.\n", (char*) arg, pid);

    return NULL;
}


int main(void)
{
    msg_t msg;

    p_main = sched_active_pid;

    p1 = thread_create(t1_stack, sizeof(t1_stack), PRIORITY_MAIN - 1,
                       CREATE_WOUT_YIELD | CREATE_STACKTEST,
                       sub_thread, "nr1", "nr1");
    p2 = thread_create(t2_stack, sizeof(t2_stack), PRIORITY_MAIN - 1,
                       CREATE_WOUT_YIELD | CREATE_STACKTEST,
                       sub_thread, "nr2", "nr2");
    p3 = thread_create(t3_stack, sizeof(t3_stack), PRIORITY_MAIN - 1,
                       CREATE_WOUT_YIELD | CREATE_STACKTEST,
                       sub_thread, "nr3", "nr3");

    puts("THREADS CREATED\n");
    for(int i = 0; i < 3; i++) {
        msg_receive(&msg);
        printf("Got msg from pid %" PRIkernel_pid ": \"%s\"\n", msg.sender_pid, msg.content.ptr);
    }

    return 0;
}
