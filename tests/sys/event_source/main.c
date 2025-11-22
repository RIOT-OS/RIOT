/*
 * SPDX-FileCopyrightText: 2022 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Event Source test application
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <stdio.h>

#include "event/callback.h"
#include "event/source.h"
#include "event/thread.h"

static void _event_cb(void *ctx)
{
    puts(ctx);
}

int main(void)
{
    event_source_t source = EVENT_SOURCE_INIT;
    event_callback_t a, b, c;

    event_source_subscriber_t ba = EVENT_SOURCE_SUBSCRIBER_INIT(&a, EVENT_PRIO_MEDIUM);
    event_source_subscriber_t bb = EVENT_SOURCE_SUBSCRIBER_INIT(&b, EVENT_PRIO_MEDIUM);
    event_source_subscriber_t bc = EVENT_SOURCE_SUBSCRIBER_INIT(&c, EVENT_PRIO_MEDIUM);

    event_callback_init(&a, _event_cb, "event A");
    event_callback_init(&b, _event_cb, "event B");
    event_callback_init(&c, _event_cb, "event C");

    puts("empty event source");
    event_source_trigger(&source);

    event_source_attach(&source, &ba);
    event_source_attach(&source, &bb);
    event_source_attach(&source, &bc);

    puts("occupied event source");
    event_source_trigger(&source);

    return 0;
}
