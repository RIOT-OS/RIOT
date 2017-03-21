/*
 * Copyright (C) 2017 HAW Hamburg
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2015 Eistec AB
 *               2013 INRIA
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
 * @brief    xtimer_drift test application
 *
 * @author   Kaspar Schleiser <kaspar@schleiser.de>
 * @author   Oliver Hahm <oliver.hahm@inria.fr>
 * @author   Christian Mehlis <mehlis@inf.fu-berlin.de>
 * @author   Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author   Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

#include <stdio.h>

#include "evtimer.h"
#include "thread.h"
#include "msg.h"
#include "xtimer.h"

static char worker_stack[THREAD_STACKSIZE_MAIN];
static evtimer_t evtimer;
static evtimer_msg_event_t events[] = {
    { .event = { .offset = 1000 }, .msg = { .content = { .ptr = "supposed to be 1000" } } },
    { .event = { .offset = 1500 }, .msg = { .content = { .ptr = "supposed to be 1500" } } },
    { .event = { .offset = 659 }, .msg = { .content = { .ptr = "supposed to be 659" } } },
    { .event = { .offset = 3954 }, .msg = { .content = { .ptr = "supposed to be 3954" } } },
    { .msg = { .content = { .ptr = NULL } }  },     /* sentinel */
};

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
    int count = 0;

    evtimer_init_msg(&evtimer);

    /* create worker thread */
    kernel_pid_t pid = thread_create(worker_stack, sizeof(worker_stack),
                                     THREAD_PRIORITY_MAIN - 1,
                                     THREAD_CREATE_STACKTEST,
                                     worker_thread, NULL, "worker");
    printf("Testing generic evtimer (start time = %" PRIu32 " ms)\n", now);
    for (evtimer_msg_event_t *event = &events[0];
         event->msg.content.ptr != NULL;
         event++) {
        evtimer_add_msg(&evtimer, event, pid);
        count++;
    }
    printf("Are the reception times of all %i msgs near to the supposed values?\n",
           count);
    puts("If yes, the tests were successful");
}
