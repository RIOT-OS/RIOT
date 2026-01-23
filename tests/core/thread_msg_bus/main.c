/*
 * SPDX-FileCopyrightText: 2020 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
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

#define MSG_TYPE_EXIT   0

char t1_stack[THREAD_STACKSIZE_MAIN];
char t2_stack[THREAD_STACKSIZE_MAIN];
char t3_stack[THREAD_STACKSIZE_MAIN];

kernel_pid_t p_main, p1, p2, p3;

static bool _recv_msg(const char *name, const msg_bus_t *bus)
{
    msg_t msg;

    msg_receive(&msg);
    assert(msg_bus_get_sender_pid(&msg) == p_main);

    printf("%s recv: %s (type=%d)%s\n", name,
           (char*) msg.content.ptr, msg_bus_get_type(&msg),
           msg_is_from_bus(bus, &msg) ? " from bus" : "");

    return msg_bus_get_type(&msg) != MSG_TYPE_EXIT;
}

void *thread1(void *arg)
{
    msg_bus_entry_t sub;

    puts("THREAD 1 start");

    msg_bus_attach(arg, &sub);
    msg_bus_subscribe(&sub, 23);
    msg_bus_subscribe(&sub, 24);
    msg_bus_subscribe(&sub, MSG_TYPE_EXIT);

    while (_recv_msg("T1", arg)) {}

    msg_bus_detach(arg, &sub);

    return NULL;
}

void *thread2(void *arg)
{
    msg_bus_entry_t sub;

    puts("THREAD 2 start");

    msg_bus_attach(arg, &sub);
    msg_bus_subscribe(&sub, 24);
    msg_bus_subscribe(&sub, MSG_TYPE_EXIT);

    while (_recv_msg("T2", arg)) {}

    msg_bus_detach(arg, &sub);

    return NULL;
}

void *thread3(void *arg)
{
    msg_bus_entry_t sub;

    puts("THREAD 3 start");

    msg_bus_attach(arg, &sub);
    msg_bus_subscribe(&sub, 23);
    msg_bus_subscribe(&sub, MSG_TYPE_EXIT);

    while (_recv_msg("T3", arg)) {}

    msg_bus_detach(arg, &sub);

    return NULL;
}

int main(void)
{
    msg_bus_t my_bus;

    msg_bus_init(&my_bus);

    p_main = thread_getpid();
    p1 = thread_create(t1_stack, sizeof(t1_stack), THREAD_PRIORITY_MAIN - 3,
                       0, thread1, &my_bus, "nr1");
    p2 = thread_create(t2_stack, sizeof(t2_stack), THREAD_PRIORITY_MAIN - 2,
                       0, thread2, &my_bus, "nr2");
    p3 = thread_create(t3_stack, sizeof(t3_stack), THREAD_PRIORITY_MAIN - 1,
                       0, thread3, &my_bus, "nr3");
    puts("THREADS CREATED");

    const char hello[] = "Hello Threads!";
    int woken;

    for (int id = 22; id < 25; ++id) {
        woken = msg_bus_post(&my_bus, id, (void*)hello);
        printf("Posted event %d to %d threads\n", id, woken);
    }

    /* mix non-bus message with bus messages */
    puts("Post message to thread 1");
    msg_t msg = {
        .type = 0x1337,
        .content.ptr = "Hello Thread 1",
    };
    msg_send(&msg, p1);

    puts("Post shutdown request to all threads");
    woken = msg_bus_post(&my_bus, MSG_TYPE_EXIT, "shutdown request");
    assert(woken == 3);

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
