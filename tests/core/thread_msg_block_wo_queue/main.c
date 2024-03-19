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

char t1_stack[THREAD_STACKSIZE_MAIN];

kernel_pid_t p_send = KERNEL_PID_UNDEF, p_recv = KERNEL_PID_UNDEF;

void *thread1(void *arg)
{
    (void) arg;

    printf("sender_thread start\n");

    msg_t msg, reply;
    memset(&msg, 1, sizeof(msg_t));

    /* step 1: send non-blocking to fill up the msg_queue of p_recv */
    msg_try_send(&msg, p_recv);

    /* step 2: send message. This puts sender_thread into msg_waiters and turns its
       status into STATUS_REPLY_BLOCKED. It should block forever, since the
       second message is never read by p_recv. */
    msg_send_receive(&msg, &reply, p_recv);

    /* If this is printed, sender_thread did *not* block as expected. */
    printf("ERROR: sender_thread should be blocking\n");

    return NULL;
}

int main(void)
{
    msg_t msg;
    p_recv = thread_getpid();

    p_send = thread_create(t1_stack, sizeof(t1_stack), THREAD_PRIORITY_MAIN - 1,
                       THREAD_CREATE_WOUT_YIELD | THREAD_CREATE_STACKTEST,
                       thread1, NULL, "nr1");

    /* step 3: receive first msg from sender_thread*/
    msg_receive(&msg);

    printf("main thread alive\n");
    return 0;
}
