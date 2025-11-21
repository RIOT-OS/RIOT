/*
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
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

static char worker_stack[THREAD_STACKSIZE_MAIN];
static evtimer_t evtimer;
#define NEVENTS (unsigned)(4)
/*
 * The events (.offset) are modified by evtimer_add.
 * This list of offsets is used to populate the events
 * before adding (or re-adding).
 */
static uint32_t offsets[NEVENTS] = {
        1000,
        1500,
        659,
        3954,
};
static evtimer_msg_event_t events[NEVENTS];
static char texts[NEVENTS][40];

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
        now = evtimer_now_msec();
        ctx = m.content.ptr;
        printf("At %6" PRIu32 " ms received msg %i: \"%s\"\n", now, count++, ctx);
    }
}

int main(void)
{
    uint32_t now;

    evtimer_init_msg(&evtimer);

    /* create worker thread */
    kernel_pid_t pid = thread_create(worker_stack, sizeof(worker_stack),
                                     THREAD_PRIORITY_MAIN - 1,
                                     0,
                                     worker_thread, NULL, "worker");

    printf("Testing generic evtimer\n");

    /* Add all the events */
    for (unsigned i = 0; i < NEVENTS; i++) {
        events[i].event.offset = offsets[i];
        now = evtimer_now_msec();
        snprintf(texts[i], sizeof(texts[i]) - 1, "#%u supposed to be %" PRIu32, i, now + events[i].event.offset);
        events[i].msg.content.ptr = texts[i];
        evtimer_add_msg(&evtimer, &events[i], pid);
    }

    /* Delete all the events */
    /* First we delete the last, to test deleting the last */
    /* Then we delete the first, to test deleting the first */
    evtimer_del(&evtimer, &events[3].event);
    evtimer_del(&evtimer, &events[2].event);
    printf("This should list %u items\n", NEVENTS - 2);
    evtimer_print(&evtimer);

    /* Delete the remaining entries */
    for (unsigned i = 0; i < NEVENTS; i++) {
        evtimer_del(&evtimer, &events[i].event);
    }

    /* Add all the events, again */
    for (unsigned i = 0; i < NEVENTS; i++) {
        events[i].event.offset = offsets[i];
        now = evtimer_now_msec();
        snprintf(texts[i], sizeof(texts[i]) - 1, "#%u supposed to be %" PRIu32, i, now + events[i].event.offset);
        events[i].msg.content.ptr = texts[i];
        evtimer_add_msg(&evtimer, &events[i], pid);
    }
    printf("This should list %u items\n", NEVENTS);
    evtimer_print(&evtimer);

    printf("Are the reception times of all %u msgs close to the supposed values?\n",
           NEVENTS);

    /* The last offset is the largest, wait for it and a tiny bit more */
    ztimer_sleep(ZTIMER_MSEC, (offsets[3] + 10));
    puts("By now all msgs should have been received");
    puts("If yes, the tests were successful");
}
