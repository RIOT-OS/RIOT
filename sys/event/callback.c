/*
 * Copyright (C) 2017 Inria
 *               2017 Freie Universität Berlin
 *               2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "event/callback.h"

void _event_callback_handler(event_t *event)
{
    event_callback_t *event_callback = (event_callback_t *) event;
    event_callback->callback(event_callback->arg);
}

void event_callback_init(event_callback_t *event_callback, void (callback)(void *), void *arg)
{
    event_callback->super.handler = _event_callback_handler;
    event_callback->callback = callback;
    event_callback->arg = arg;
}
