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

#include <assert.h>
#include <stdio.h>
#include <inttypes.h>

#include "thread.h"
#include "msg.h"
#include "msg_bus.h"

char t1_stack[THREAD_STACKSIZE_MAIN];
char t2_stack[THREAD_STACKSIZE_MAIN];
char t3_stack[THREAD_STACKSIZE_MAIN];

kernel_pid_t p_main, p1, p2, p3;

void *thread1(void *arg)
{
    msg_t msg;
    msg_bus_entry_t sub;

    puts("THREAD 1 start");

    msg_bus_attach(arg, &sub);
    msg_bus_subscribe(&sub, 23);
    msg_bus_subscribe(&sub, 24);

    msg_receive(&msg);

    /* check if the message came from the right bus */
    assert(msg_is_from_bus(arg, &msg));
    assert(msg_bus_get_sender_pid(&msg) == p_main);

    printf("T1 recv: %s (type=%d)\n",
          (char*) msg.content.ptr, msg_bus_get_type(&msg));

    msg_bus_detach(arg, &sub);

    return NULL;
}

void *thread2(void *arg)
{
    msg_t msg;
    msg_bus_entry_t sub;

    puts("THREAD 2 start");

    msg_bus_attach(arg, &sub);
    msg_bus_subscribe(&sub, 24);

    msg_receive(&msg);

    /* check if the message came from the right bus */
    assert(msg_is_from_bus(arg, &msg));
    assert(msg_bus_get_sender_pid(&msg) == p_main);

    printf("T2 recv: %s (type=%d)\n",
          (char*) msg.content.ptr, msg_bus_get_type(&msg));

    msg_bus_detach(arg, &sub);

    return NULL;
}

void *thread3(void *arg)
{
    msg_t msg;
    msg_bus_entry_t sub;

    puts("THREAD 3 start");

    msg_bus_attach(arg, &sub);
    msg_bus_subscribe(&sub, 23);

    msg_receive(&msg);

    /* check if the message came from the right bus */
    assert(msg_is_from_bus(arg, &msg));
    assert(msg_bus_get_sender_pid(&msg) == p_main);

    printf("T3 recv: %s (type=%d)\n",
          (char*) msg.content.ptr, msg_bus_get_type(&msg));

    msg_bus_detach(arg, &sub);

    return NULL;
}

int main(void)
{
    msg_bus_t my_bus;

    msg_bus_init(&my_bus);

    p_main = thread_getpid();
    p1 = thread_create(t1_stack, sizeof(t1_stack), THREAD_PRIORITY_MAIN - 3,
                       THREAD_CREATE_STACKTEST, thread1, &my_bus, "nr1");
    p2 = thread_create(t2_stack, sizeof(t2_stack), THREAD_PRIORITY_MAIN - 2,
                       THREAD_CREATE_STACKTEST, thread2, &my_bus, "nr2");
    p3 = thread_create(t3_stack, sizeof(t3_stack), THREAD_PRIORITY_MAIN - 1,
                       THREAD_CREATE_STACKTEST, thread3, &my_bus, "nr3");
    puts("THREADS CREATED");

    const char hello[] = "Hello Threads!";

    for (int id = 22; id < 25; ++id) {
        int woken = msg_bus_post(&my_bus, id, (void*)hello);
        printf("Posted event %d to %d threads\n", id, woken);
    }

    /* make sure all threads have terminated */
    if (thread_getstatus(p1) != STATUS_NOT_FOUND ||
        thread_getstatus(p2) != STATUS_NOT_FOUND ||
        thread_getstatus(p3) != STATUS_NOT_FOUND ) {
        puts("FAILED");
        return 1;
    }

    puts("SUCCESS");
    return 0;
}
