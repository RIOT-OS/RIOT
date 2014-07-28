/*
 * Copyright (C) 2014 Freie Universität Berlin
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
 * @author Lotte Steenbrink <lotte.steenbrink@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "thread.h"
#include "msg.h"

thread_t t1;
char t1_stack[KERNEL_CONF_STACKSIZE_MAIN];

void *thread1(void *arg)
{
    (void) arg;

    printf("THREAD %u start\n", t1.pid);

    msg_t msg, reply;
    memset(&msg, 1, sizeof(msg_t));

    /* step 1: send asynchonously */
    msg_send(&msg, kernel_main_thread.pid, 0);

    /* step 2: send message, turning its status into STATUS_REPLY_BLOCKED */
    msg_send_receive(&msg, &reply, kernel_main_thread.pid);
    printf("received: %u, %u \n", reply.sender_pid, reply.type);
    printf("pointer: %s\n", reply.content.ptr);

    printf("THREAD %u SHOULD BE BLOCKING :(\n", t1.pid);

    return NULL;
}

int main(void)
{
    thread_create(&t1, t1_stack, sizeof(t1_stack), PRIORITY_MAIN - 1,
                  CREATE_WOUT_YIELD | CREATE_STACKTEST,
                  thread1, NULL, "nr1");

    /* step 3: receive a msg */
    msg_t msg;
    msg_receive(&msg);

    puts("MAIN THREAD ALIVE!");
    return 0;
}
