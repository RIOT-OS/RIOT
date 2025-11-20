/*
 * SPDX-FileCopyrightText: 2016 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief   msg_try_receive regression test application
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>
#include "thread.h"

#include "msg.h"

static kernel_pid_t _main_pid;

static char stack[THREAD_STACKSIZE_MAIN];
static void *second_thread(void *arg)
{
    (void) arg;
    msg_t test;
    puts("sending message...");
    msg_send(&test, _main_pid);

    return NULL;
}

int main(void)
{
    printf("main starting\n");

    _main_pid = thread_getpid();

    thread_create(stack,
                  sizeof(stack),
                  THREAD_PRIORITY_MAIN - 1,
                  0,
                  second_thread,
                  NULL,
                  "second_thread");

    msg_t m;
    printf("msg available: %i (should be 1!)\n", msg_try_receive(&m));

    return 0;
}
