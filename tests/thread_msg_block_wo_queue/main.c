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

char t1_stack[KERNEL_CONF_STACKSIZE_MAIN];

kernel_pid_t p1 = KERNEL_PID_UNDEF, p_main = KERNEL_PID_UNDEF;

void *thread1(void *arg)
{
    (void) arg;

    printf("THREAD %u start\n", p1);

    blip_t msg, reply;
    memset(&msg, 1, sizeof(blip_t));

    /* step 1: send asynchonously */
    blip_try_send(&msg, p_main);

    /* step 2: send message, turning its status into STATUS_REPLY_BLOCKED */
    blip_send_receive(&msg, &reply, p_main);
    printf("received: %" PRIkernel_pid ", %u \n", reply.sender_pid, reply.type);
    printf("pointer: %s\n", reply.content.ptr);

    printf("THREAD %" PRIkernel_pid " SHOULD BE BLOCKING :(\n", p1);

    return NULL;
}

int main(void)
{
    blip_t msg;
    p_main = sched_active_pid;

    p1 = thread_create(t1_stack, sizeof(t1_stack), PRIORITY_MAIN - 1,
                       CREATE_WOUT_YIELD | CREATE_STACKTEST,
                       thread1, NULL, "nr1");

    /* step 3: receive a msg */
    blip_receive(&msg);

    printf("MAIN THREAD %" PRIkernel_pid " ALIVE!\n", p_main);
    return 0;
}
