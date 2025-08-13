/*
 * SPDX-FileCopyrightText: 2014 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       IPC pingpong application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "thread.h"
#include "msg.h"

void *second_thread(void *arg)
{
    (void) arg;

    printf("2nd thread started, pid: %" PRIkernel_pid "\n", thread_getpid());
    msg_t m;

    while (1) {
        msg_receive(&m);
        printf("2nd: Got msg from %" PRIkernel_pid "\n", m.sender_pid);
        m.content.value++;
        msg_reply(&m, &m);
    }

    return NULL;
}

char second_thread_stack[THREAD_STACKSIZE_MAIN];

int main(void)
{
    printf("Starting IPC Ping-pong example...\n");
    printf("1st thread started, pid: %" PRIkernel_pid "\n", thread_getpid());

    msg_t m;

    kernel_pid_t pid = thread_create(second_thread_stack, sizeof(second_thread_stack),
                            THREAD_PRIORITY_MAIN - 1, 0,
                            second_thread, NULL, "pong");

    m.content.value = 1;

    while (1) {
        msg_send_receive(&m, &m, pid);
        printf("1st: Got msg with content %u\n", (unsigned int)m.content.value);
    }
}
