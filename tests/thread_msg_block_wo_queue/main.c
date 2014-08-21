/*
 * Copyright (C) 2014 Freie Universität Berlin
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
 * @author Lotte Steenbrink <lotte.steenbrink@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "thread.h"
#include "msg.h"
#include "msg_queue.h"

char t1_stack[KERNEL_CONF_STACKSIZE_MAIN];

kernel_pid_t p1 = KERNEL_PID_UNDEF, p_main = KERNEL_PID_UNDEF;

void *thread1(void *arg)
{
    (void) arg;

    printf("THREAD %u start\n", p1);

    msg_pulse_t msg, reply;
    memset(&msg, 1, sizeof(msg_pulse_t));

    /* step 1: send asynchonously */
    msg_try_send_pulse(&msg, p_main);

    /* step 2: send message, turning its status into STATUS_REPLY_BLOCKED */
    msg_send_receive_pulse(&msg, &reply, p_main);
    printf("received: %u, %u \n", reply.sender_pid, reply.type);
    printf("pointer: %s\n", reply.content.ptr);

    printf("THREAD %" PRIkernel_pid " SHOULD BE BLOCKING :(\n", p1);

    return NULL;
}

char queue_buf[4096];

int main(void)
{
    msg_pulse_t msg;
    p_main = sched_active_pid;

    msg_queue_t msg_queue;

    msg_queue_init(&msg_queue, queue_buf, sizeof(queue_buf), 0);
    thread_set_msg_queue(sched_active_pid, &msg_queue);

    p1 = thread_create(t1_stack, KERNEL_CONF_STACKSIZE_PRINTF, PRIORITY_MAIN - 1,
                       CREATE_WOUT_YIELD | CREATE_STACKTEST, thread1, "nr1");

    /* step 3: receive a msg */
    msg_receive_pulse(&msg);

    printf("MAIN THREAD %" PRIkernel_pid " ALIVE!\n", p_main);
    return 0;
}
