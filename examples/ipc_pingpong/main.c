/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
    blip_t m;

    while (1) {
        blip_receive(&m);
        printf("2nd: Got msg from %" PRIkernel_pid "\n", m.sender_pid);
        m.content.value++;
        blip_reply(&m, &m);
    }

    return NULL;
}

char second_thread_stack[KERNEL_CONF_STACKSIZE_MAIN];

int main(void)
{
    printf("Starting IPC Ping-pong example...\n");
    printf("1st thread started, pid: %" PRIkernel_pid "\n", thread_getpid());

    blip_t m;

    kernel_pid_t pid = thread_create(second_thread_stack, sizeof(second_thread_stack),
                            PRIORITY_MAIN - 1, CREATE_STACKTEST,
                            second_thread, NULL, "pong");

    m.content.value = 1;

    while (1) {
        blip_send_receive(&m, &m, pid);
        printf("1st: Got msg with content %u\n", (unsigned int)m.content.value);
    }
}
