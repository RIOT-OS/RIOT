/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
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
 * @brief       Message bus test application
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "thread.h"
#include "msg.h"

char t1_stack[THREAD_STACKSIZE_MAIN];
char t2_stack[THREAD_STACKSIZE_MAIN];
char t3_stack[THREAD_STACKSIZE_MAIN];

kernel_pid_t p_main, p1, p2, p3;

void *thread1(void *arg)
{
    (void) arg;
    puts("THREAD 1 start");

    msg_t msg;
    msg_bus_receive_single(&msg, 0x23);
    printf("T1 recv: %s (type=%x)\n", (char*) msg.content.ptr, msg.type);

    return NULL;
}

void *thread2(void *arg)
{
    (void) arg;
    puts("THREAD 2 start");

    msg_t msg;
    msg_bus_receive_single(&msg, 0x24);
    printf("T2 recv: %s (type=%x)\n", (char*) msg.content.ptr, msg.type);

    return NULL;
}

void *thread3(void *arg)
{
    (void) arg;
    puts("THREAD 3 start");

    msg_t msg;
    msg_bus_receive_single(&msg, 0x23);
    printf("T3 recv: %s (type=%x)\n", (char*) msg.content.ptr, msg.type);

    return NULL;
}

int main(void)
{
    p_main = sched_active_pid;
    p1 = thread_create(t1_stack, sizeof(t1_stack), THREAD_PRIORITY_MAIN - 1,
                       THREAD_CREATE_STACKTEST, thread1, NULL, "nr1");
    p2 = thread_create(t2_stack, sizeof(t2_stack), THREAD_PRIORITY_MAIN - 1,
                       THREAD_CREATE_STACKTEST, thread2, NULL, "nr2");
    p3 = thread_create(t3_stack, sizeof(t3_stack), THREAD_PRIORITY_MAIN - 1,
                       THREAD_CREATE_STACKTEST, thread3, NULL, "nr3");
    puts("THREADS CREATED");

    const char hello[] = "Hello Threads!";

    for (int id = 0x22; id < 0x25; ++id) {
        int woken = msg_bus_post(id, (void*) hello);
        printf("Posted event 0x%x to %d threads\n", id, woken);
    }

    puts("SUCCESS");

    return 0;
}
