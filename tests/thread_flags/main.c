/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @brief   simple thread flags test application
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>
#include "thread.h"

static char stack[THREAD_STACKSIZE_MAIN];

static void *_thread(void *arg)
{
    (void) arg;

    thread_flags_t flags;

    printf("thread(): waiting for 0x1...\n");
    flags = thread_flags_wait_any(0x1);
    printf("thread(): received flags: 0x%04x\n", (unsigned)flags & 0xFFFF);

    printf("thread(): waiting for 0x1 || 0x64...\n");
    flags = thread_flags_wait_any(0x1 | 0x64);
    printf("thread(): received flags: 0x%04x\n", (unsigned)flags & 0xFFFF);

    printf("thread(): waiting for 0x2 && 0x4...\n");
    flags = thread_flags_wait_all(0x2 | 0x4);
    printf("thread(): received flags: 0x%04x\n", (unsigned)flags & 0xFFFF);


    printf("thread(): waiting for any flag, one by one\n");
    flags = thread_flags_wait_one(0xFFFF);
    printf("thread(): received flags: 0x%04x\n", (unsigned)flags & 0xFFFF);

    printf("thread(): waiting for any flag, one by one\n");
    flags = thread_flags_wait_one(0xFFFF);
    printf("thread(): received flags: 0x%04x\n", (unsigned)flags & 0xFFFF);

    return NULL;
}

static void _set(thread_t *thread, thread_flags_t flags)
{
    printf("main(): setting flag 0x%04x\n", (unsigned)flags & 0xFFFF);
    thread_flags_set(thread, flags);
}

int main(void)
{
    printf("main starting\n");

    kernel_pid_t pid = thread_create(stack,
                  sizeof(stack),
                  THREAD_PRIORITY_MAIN - 1,
                  THREAD_CREATE_STACKTEST,
                  _thread,
                  NULL,
                  "second_thread");

    thread_t *thread = (thread_t*) thread_get(pid);
    _set(thread, 0x1);
    _set(thread, 0x64);
    _set(thread, 0x1);
    _set(thread, 0x8);
    _set(thread, 0x2);
    _set(thread, 0x4);

    while(1) {};
    return 0;
}
