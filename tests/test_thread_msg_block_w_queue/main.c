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

char t1_stack[KERNEL_CONF_STACKSIZE_PRINTF];

uint16_t p1, p_main;

void thread1(void)
{
    printf("THREAD %u start\n", p1);

    msg_t msg, reply;
    memset(&msg, 1, sizeof(msg_t));

    /* step 1: send asynchonously */
    msg_send(&msg, p_main, 0);

    /* step 2: send message, turning its status into STATUS_REPLY_BLOCKED */
    msg_send_receive(&msg, &reply, p_main);
    printf("received: %u, %u \n", reply.sender_pid, reply.type);
    printf("pointer: %s\n", reply.content.ptr);

    printf("THREAD %u SHOULD BE BLOCKING :(\n", p1);
}

int main(void)
{
    msg_t msg;
    p_main = thread_pid;

    msg_t msg_q[1];
    msg_init_queue(msg_q, 1);

    p1 = thread_create(t1_stack, KERNEL_CONF_STACKSIZE_PRINTF, PRIORITY_MAIN - 1,
                       CREATE_WOUT_YIELD | CREATE_STACKTEST, thread1, "nr1");

    /* step 3: receive a msg */
    msg_receive(&msg);

    printf("MAIN THREAD %u ALIVE!\n", p_main);
}
