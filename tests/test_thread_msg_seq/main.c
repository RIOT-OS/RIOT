/*
 * Copyright (C) 2013 Christian Mehlis <mehlis@inf.fu-berlin.de>
 *           (C) 2014 Kaspar Schleiser <kaspar@schleiser.de>
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

uint16_t p1, p2, p3;

void *sub_thread(void *arg)
{
    (void) arg;

    int pid = thread_getpid();
    printf("THREAD %s (pid:%i) start\n", thread_getname(pid), pid);

    msg_t msg;

    msg.content.ptr = (char*)thread_getname(pid);

    msg_send(&msg, 1, 1);

    printf("THREAD %s (pid:%i) end.\n", thread_getname(pid), pid);

    return NULL;
}


int main(void)
{
    msg_t msg;

    p1 = thread_create(t1_stack, sizeof(t1_stack), PRIORITY_MAIN - 1,
                       CREATE_WOUT_YIELD | CREATE_STACKTEST,
                       sub_thread, NULL, "nr1");
    p2 = thread_create(t2_stack, sizeof(t2_stack), PRIORITY_MAIN - 1,
                       CREATE_WOUT_YIELD | CREATE_STACKTEST,
                       sub_thread, NULL, "nr2");
    p3 = thread_create(t3_stack, sizeof(t3_stack), PRIORITY_MAIN - 1,
                       CREATE_WOUT_YIELD | CREATE_STACKTEST,
                       sub_thread, NULL, "nr3");

    puts("THREADS CREATED\n");
    for(int i = 0; i < 3; i++) {
        msg_receive(&msg);
        printf("Got msg from pid %i: \"%s\"\n", msg.sender_pid, msg.content.ptr);
    }

    return 0;
}
