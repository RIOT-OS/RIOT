/*
 * Copyright (C) 2014 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @brief msg_node test application
 *
 * @author Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include "thread.h"

char t2_stack[KERNEL_CONF_STACKSIZE_MAIN];

typedef struct action {
     msg_node_t node;
     char *name;
} action_t;

action_t action1;
action_t action2;

void *second_thread(void *arg)
{
    (void) arg;
    puts("second thread\n");

    msg_node_init(&action1.node);
    action1.name = "ACTION 1";
    msg_node_init(&action2.node);
    action2.name = "ACTION 2";

    char msg_buf[256];
    msg_hdr_t hdr;

    int res = bmsg_receive(msg_buf, sizeof(msg_buf), &hdr);
    printf("t2 a: res=%i target_node=%x\n", res, (unsigned)hdr.target_node);

    res = bmsg_receive(msg_buf, sizeof(msg_buf), &hdr);
    printf("t2 b: res=%i sender_node=%x target_node=%x action=%s\n", res, (unsigned)hdr.sender_node, (unsigned)hdr.target_node,
            ((action_t*)hdr.target_node)->name);
    msg_buf[hdr.size] = '\0';
    printf("t2 b: received \"%s\".\n", hdr.payload);
    
    res = bmsg_receive(msg_buf, sizeof(msg_buf), &hdr);
    printf("t2 c: res=%i sender_node=%x target_node=%x action=%s\n", res, (unsigned)hdr.sender_node, (unsigned)hdr.target_node,
            ((action_t*)hdr.target_node)->name);
    msg_buf[hdr.size] = '\0';
    printf("t2 b: received \"%s\".\n", hdr.payload);

    return NULL;
}

int main(void)
{
    kernel_pid_t t2 = thread_create(
            t2_stack, sizeof(t2_stack),
            PRIORITY_MAIN - 1, CREATE_STACKTEST,
            second_thread, NULL, "nr2");

    puts("first thread.\n");

    char *buf = "TEST MESSAGE";

    int res = bmsg_send(thread_get(t2)->msg_node, NULL, buf, 3);
    printf("main: 1. res=%i\n", res);

    res = bmsg_send(&action1.node, NULL, buf, 4);
    printf("main: 2. res=%i\n", res);
    
    res = bmsg_send(&action2.node, NULL, buf, 7);
    printf("main: 3. res=%i\n", res);

    return 0;
}
