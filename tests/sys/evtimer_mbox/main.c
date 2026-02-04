/*
 * SPDX-FileCopyrightText: 2020 Simon Brummer <simon.brummer@posteo.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief    evtimer_mbox test application
 *
 * @author   Simon Brummer <simon.brummer@posteo.de>
 *
 * @}
 */

#include <stdio.h>

#include "kernel_defines.h"
#include "msg.h"
#include "thread.h"
#include "evtimer_msg.h"
#include "evtimer_mbox.h"

#define MBOX_SIZE            (4)
#define TIMEOUT_MS           (100)
#define TRIGGER_TIMEOUT_MS   (TIMEOUT_MS + TIMEOUT_MS * MBOX_SIZE)

int main(void)
{
    /* Initialize mbox and evtimers */
    msg_t queue[MBOX_SIZE];
    mbox_t mbox;
    mbox_init(&mbox, queue, ARRAY_SIZE(queue));

    evtimer_t timer;
    evtimer_t trigger_timer;
    evtimer_init_mbox(&timer);
    evtimer_init_msg(&trigger_timer);

    /* Initialize mbox events */
    evtimer_mbox_event_t events[MBOX_SIZE];

    for (unsigned i = 0; i < ARRAY_SIZE(events); ++i) {
        evtimer_mbox_event_t *e = events + i;

        e->event.offset = TIMEOUT_MS + TIMEOUT_MS * i;
        e->msg.type = i;
    }

    /* Initialize trigger event */
    evtimer_msg_event_t trigger;
    trigger.event.offset = TRIGGER_TIMEOUT_MS;

    /* Add events in reverse order to their expected timeout expiration */
    for (int i = ARRAY_SIZE(events) - 1; i >= 0; --i) {
        evtimer_add_mbox(&timer, events + i, &mbox);
    }
    evtimer_add_msg(&trigger_timer, &trigger, thread_getpid());

    /* Wait until trigger event expired.*/
    msg_t msg;
    msg_receive(&msg);

    /* Verify mbox content, Expectations are:
     * 1) Given mbox is full (always use blocking get)
     * 2) All msg.type values are stored in ascending order
     */
    int failed = 0;

    for (unsigned i = 0; i < MBOX_SIZE; ++i) {
        mbox_get(&mbox, &msg);

        if (msg.type != i) {
            failed = 1;
            break;
        }
    }

    if (!failed) {
        puts("Test successful\n");
    }

    return 0;
}
