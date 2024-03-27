/*
 * Copyright (C) 2017 Freie Universität Berlin
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
 * @brief    evtimer_msg test application
 *
 * @author   Martine Lenders <m.lenders@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "evtimer_msg.h"
#include "thread.h"
#include "msg.h"
#include "ztimer.h"

#define WORKER_MSG_QUEUE_SIZE   (8)

msg_t worker_msg_queue[WORKER_MSG_QUEUE_SIZE];
static char worker_stack[THREAD_STACKSIZE_MAIN];
static evtimer_t evtimer;
static evtimer_msg_event_t events[] = {
    { .event = { .offset = 0 }, .msg = { .content = { .ptr = "1" } } },
    { .event = { .offset = 0 }, .msg = { .content = { .ptr = "2" } } },
    { .event = { .offset = 0 }, .msg = { .content = { .ptr = "3" } } },
    { .event = { .offset = 0 }, .msg = { .content = { .ptr = "4" } } },
    { .event = { .offset = 0 }, .msg = { .content = { .ptr = "5" } } },
    { .event = { .offset = 0 }, .msg = { .content = { .ptr = "6" } } },
    { .event = { .offset = 0 }, .msg = { .content = { .ptr = "7" } } },
    { .event = { .offset = 0 }, .msg = { .content = { .ptr = "8" } } },
};

#define NEVENTS ARRAY_SIZE(events)

/* This thread will print the drift to stdout once per second */
void *worker_thread(void *arg)
{
    (void)arg;

    msg_init_queue(worker_msg_queue, WORKER_MSG_QUEUE_SIZE);
    while (1) {
        char *ctx;
        msg_t m;

        msg_receive(&m);
        ctx = m.content.ptr;
        printf("received msg \"%s\"\n", ctx);
    }
}

void sleep_msec(uint16_t t)
{
    ztimer_sleep(ZTIMER_MSEC, t);
}

int main(void)
{
    evtimer_init_msg(&evtimer);

    /* create worker thread */
    kernel_pid_t pid = thread_create(worker_stack, sizeof(worker_stack),
                                     THREAD_PRIORITY_MAIN - 1,
                                     0,
                                     worker_thread, NULL, "worker");

    while (1) {
        for (unsigned i = 0; i < NEVENTS; i++) {
            evtimer_add_msg(&evtimer, &events[i], pid);
            sleep_msec(10);
        }
    }
}
