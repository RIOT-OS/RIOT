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
#include "xtimer.h"

static char worker_stack[THREAD_STACKSIZE_MAIN];
static evtimer_t evtimer;
static evtimer_msg_event_t events[] = {
    { .event = { .offset_ms = 1000 }, .msg = { .content = { .ptr = "supposed to be 1000" } } },
    { .event = { .offset_ms = 1500 }, .msg = { .content = { .ptr = "supposed to be 1500" } } },
    { .event = { .offset_ms = 659 }, .msg = { .content = { .ptr = "supposed to be 659" } } },
    { .event = { .offset_ms = 3954 }, .msg = { .content = { .ptr = "supposed to be 3954" } } },
};

#define NEVENTS ((unsigned)(sizeof(events) / sizeof(evtimer_msg_event_t)))

/* This thread will print the drift to stdout once per second */
void *worker_thread(void *arg)
{
    int count = 0;
    (void) arg;

    while (1) {
        char *ctx;
        msg_t m;
        uint32_t now;

        msg_receive(&m);
        now = xtimer_now_usec() / US_PER_MS;
        ctx = m.content.ptr;
        printf("At %6" PRIu32 " ms received msg %i: \"%s\"\n", now, count++, ctx);
    }
}

int main(void)
{
    uint32_t now = xtimer_now_usec() / US_PER_MS;

    evtimer_init_msg(&evtimer);

    /* create worker thread */
    kernel_pid_t pid = thread_create(worker_stack, sizeof(worker_stack),
                                     THREAD_PRIORITY_MAIN - 1,
                                     THREAD_CREATE_STACKTEST,
                                     worker_thread, NULL, "worker");
    printf("Testing generic evtimer (start time = %" PRIu32 " ms)\n", now);
    for (unsigned i = 0; i < NEVENTS; i++) {
        evtimer_add_msg(&evtimer, &events[i], pid);
    }
    printf("Are the reception times of all %u msgs close to the supposed values?\n",
           NEVENTS);
    puts("If yes, the tests were successful");
}
