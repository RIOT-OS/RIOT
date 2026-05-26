/*
 * SPDX-FileCopyrightText: 2017 Inria
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2017 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include "kernel_defines.h"
#include "event/timeout.h"
#include "ztimer.h"

static void _event_timeout_callback(void *arg)
{
    event_timeout_t *event_timeout = (event_timeout_t *)arg;

    event_post(event_timeout->queue, event_timeout->event);
}

#if IS_USED(MODULE_EVENT_TIMEOUT)
void event_timeout_init(event_timeout_t *event_timeout, event_queue_t *queue, event_t *event)
{
    event_timeout_ztimer_init(event_timeout, ZTIMER_USEC, queue, event);
}
#endif

void event_timeout_ztimer_init(event_timeout_t *event_timeout, ztimer_clock_t *clock,
                               event_queue_t *queue, event_t *event)
{
    event_timeout->clock = clock;
    event_timeout->timer.callback = _event_timeout_callback;
    event_timeout->timer.arg = event_timeout;
    event_timeout->queue = queue;
    event_timeout->event = event;
}

void event_timeout_set(event_timeout_t *event_timeout, uint32_t timeout)
{
    if (timeout == 0) {
        event_post(event_timeout->queue, event_timeout->event);
    } else {
        ztimer_set(event_timeout->clock, &event_timeout->timer, timeout);
    }
}

void event_timeout_clear(event_timeout_t *event_timeout)
{
    if (event_timeout->clock) {
        ztimer_remove(event_timeout->clock, &event_timeout->timer);
    }
}
