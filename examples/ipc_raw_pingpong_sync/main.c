/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014 Kaspar Schleiser
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       IPC pingpong application using synchronous raw messaging
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "thread.h"
#include "msg.h"
#include "kernel.h"

#define NUM_MSGS 3

static void *second_thread(void *arg)
{
    (void)arg;

    printf("2nd thread started, pid: %i\n", thread_getpid());
    msg_hdr_t hdr;

    unsigned int value = 0;

    while (value <= NUM_MSGS) {
        msg_receive((char*) &value, sizeof(value), &hdr);
        printf("2nd: Got msg from node %p with data=%u\n", (void*)hdr.node, value);
        value += 1;
        msg_reply(hdr.node, (char*)&value, sizeof(value));
    }

    printf("2nd: Exiting.\n");

    return NULL;
}

char second_thread_stack[KERNEL_CONF_STACKSIZE_MAIN];

int main(void)
{
    printf("Starting IPC Ping-pong example...\n");
    printf("1st thread started, pid: %i\n", thread_getpid());

    unsigned int value = 0;

    kernel_pid_t pid = thread_create(second_thread_stack, sizeof(second_thread_stack),
                            PRIORITY_MAIN - 1, CREATE_STACKTEST,
                            second_thread, NULL, "pong");

    printf("1st: Sending %i messages.\n", NUM_MSGS+1);

    msg_node_t *target = thread_get_msg_node(pid);
    while (value <= NUM_MSGS) {
        msg_send_receive(target, (char*)&value, sizeof(value), (char*)&value, sizeof(value));
    }

    printf("1st: Final value: %i (Expected: %i)\n", value, NUM_MSGS+1);
    printf("1st: Exiting.\n");

    return 0;
}
