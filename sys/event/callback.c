/*
 * SPDX-FileCopyrightText: 2017 Inria
 * SPDX-FIleCopyrightText: 2017 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2017 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include "event/callback.h"

void _event_callback_handler(event_t *event)
{
    event_callback_t *event_callback = (event_callback_t *) event;
    if (IS_USED(MODULE_EVENT_LOOP_DEBUG)) {
        printf("event (pid %u): executing %p->%p(%p)\n",
               thread_getpid(),
               (void *)event,
               (void *)(uintptr_t)event_callback->callback,
               event_callback->arg);
    }
    event_callback->callback(event_callback->arg);
}

void event_callback_init(event_callback_t *event_callback, void (callback)(void *), void *arg)
{
    memset(event_callback, 0, sizeof(*event_callback));
    event_callback->super.handler = _event_callback_handler;
    event_callback->callback = callback;
    event_callback->arg = arg;
}
