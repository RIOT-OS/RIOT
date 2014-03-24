/*
 * Copyright (C) 2013 Freie Universität Berlin
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
 * @brief       IPC pingpong application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>

#include "thread.h"
#include "msg.h"
#include "kernel.h"

void second_thread(void)
{
    printf("second_thread starting.\n");
    msg_t m;

    while (1) {
        msg_receive(&m);
        printf("2nd: got msg from %i\n", m.sender_pid);
        m.content.value++;
        msg_reply(&m, &m);
    }
}

char second_thread_stack[KERNEL_CONF_STACKSIZE_MAIN];

int main(void)
{
    printf("Hello world!\n");

    msg_t m;

    int pid = thread_create(second_thread_stack, sizeof(second_thread_stack),
                            PRIORITY_MAIN - 1, CREATE_STACKTEST,
                            second_thread, "pong");

    m.content.value = 1;

    while (1) {
        msg_send_receive(&m, &m, pid);
        printf("Got msg with content %u\n", m.content.value);
    }
}
