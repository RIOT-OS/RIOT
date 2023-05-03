/*
 * Copyright (C) 2023 ML!PA Consulting GmbH
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
 * @brief       Message bus test application for system power bus and pm_layered integration
 *
 * @author      Fabian Hüßler <fabian.huessler@ml-pa.com>
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>

#include "thread.h"
#include "msg.h"
#include "msg_bus.h"
#include "sys/bus.h"
#include "shell.h"
#include "ztimer.h"
#include "ztimer/periodic.h"

char t1_stack[THREAD_STACKSIZE_SMALL];
char t2_stack[THREAD_STACKSIZE_SMALL];
char t3_stack[THREAD_STACKSIZE_SMALL];

kernel_pid_t p1, p2, p3;

static void _recv_msg(const char *name, const msg_bus_t *bus)
{
    msg_t msg;

    msg_receive(&msg);
    printf("%s recv: %p (type=%d)%s\n", name,
           msg.content.ptr, msg_bus_get_type(&msg),
           msg_is_from_bus(bus, &msg) ? " from bus" : "");
    if (msg_is_from_bus(sys_bus_get(SYS_BUS_POWER), &msg)) {
        msg.type = msg_bus_get_type(&msg);
        msg.sender_pid = msg_bus_get_sender_pid(&msg);
        if (msg.type == SYS_BUS_POWER_EVENT_PM_CHANGED) {
            printf("received pm mode %u event\n", (uintptr_t)msg.content.ptr);
            msg_t reply;
            msg_reply(&msg, &reply);
        }
    }
}

void *thread1(void *arg)
{
    (void)arg;
    msg_bus_entry_t sub;

    puts("THREAD 1 start");

    msg_bus_attach(sys_bus_get(SYS_BUS_POWER), &sub);
    msg_bus_subscribe(&sub, SYS_BUS_POWER_EVENT_PM_CHANGED);

    while (1) {
        _recv_msg("T1", sys_bus_get(SYS_BUS_POWER));
    }

    return NULL;
}

void *thread2(void *arg)
{
    (void)arg;
    msg_bus_entry_t sub;

    puts("THREAD 2 start");

    msg_bus_attach(sys_bus_get(SYS_BUS_POWER), &sub);
    msg_bus_subscribe(&sub, SYS_BUS_POWER_EVENT_PM_CHANGED);

    while (1) {
        _recv_msg("T2", sys_bus_get(SYS_BUS_POWER));
    }

    return NULL;
}

void *thread3(void *arg)
{
    (void)arg;
    msg_bus_entry_t sub;

    puts("THREAD 3 start");

    msg_bus_attach(sys_bus_get(SYS_BUS_POWER), &sub);
    msg_bus_subscribe(&sub, SYS_BUS_POWER_EVENT_PM_CHANGED);

    while (1) {
        _recv_msg("T3", sys_bus_get(SYS_BUS_POWER));
    }

    return NULL;
}

static bool _ztimer_periodic_isr(void *arg)
{
    (void)arg;
    return true;
}

int main(void)
{
    p1 = thread_create(t1_stack, sizeof(t1_stack), THREAD_PRIORITY_MAIN - 3,
                       THREAD_CREATE_STACKTEST, thread1, NULL, "nr1");
    p2 = thread_create(t2_stack, sizeof(t2_stack), THREAD_PRIORITY_MAIN - 2,
                       THREAD_CREATE_STACKTEST, thread2, NULL, "nr2");
    p3 = thread_create(t3_stack, sizeof(t3_stack), THREAD_PRIORITY_MAIN - 1,
                       THREAD_CREATE_STACKTEST, thread3, NULL, "nr3");
    puts("THREADS CREATED");

    ztimer_periodic_t interrupt_timer;
    ztimer_periodic_init(ZTIMER_SEC, &interrupt_timer, _ztimer_periodic_isr, NULL, 30);
    ztimer_periodic_start(&interrupt_timer);

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
