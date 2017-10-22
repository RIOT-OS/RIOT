/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "event/timeout.h"

static void _event_timeout_callback(void *arg)
{
    event_timeout_t *event_timeout = (event_timeout_t *)arg;
    event_post(event_timeout->queue, event_timeout->event);
}

void event_timeout_init(event_timeout_t *event_timeout, event_queue_t *queue, event_t *event)
{
    event_timeout->timer.callback = _event_timeout_callback;
    event_timeout->timer.arg = event_timeout;
    event_timeout->queue = queue;
    event_timeout->event = event;
}

void event_timeout_set(event_timeout_t *event_timeout, uint32_t timeout)
{
    xtimer_set(&event_timeout->timer, timeout);
}
