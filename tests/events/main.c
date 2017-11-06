/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       event test application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>

#include "thread.h"
#include "event.h"
#include "event/timeout.h"
#include "event/callback.h"

static unsigned order;
static uint32_t before;

static void callback(event_t *arg);
static void custom_callback(event_t *event);
static void timed_callback(void *arg);


static event_t event = { .handler = callback };
static event_t event2 = { .handler = callback };

static void callback(event_t *arg)
{
    order++;
    assert(order == 1);
    assert(arg == &event);
    printf("triggered 0x%08x\n", (unsigned)arg);
}

typedef struct {
    event_t super;
    const char *text;
} custom_event_t;

static custom_event_t custom_event = { .super.handler = custom_callback, .text = "CUSTOM CALLBACK" };
static event_callback_t event_callback = EVENT_CALLBACK_INIT(timed_callback, 0x12345678);

static void custom_callback(event_t *event)
{
    order++;
    assert(order == 2);
    assert(event == (event_t *)&custom_event);
    custom_event_t *custom_event = (custom_event_t *)event;
    printf("triggered custom event with text: \"%s\"\n", custom_event->text);
}

static void timed_callback(void *arg)
{
    order++;
    assert(order == 3);
    assert(arg == event_callback.arg);
    uint32_t now = xtimer_now_usec();
    assert((now - before >= 100000LU));
    printf("triggered timed callback with arg 0x%08x after %" PRIu32 "us\n", (unsigned)arg, now - before);
    printf("[SUCCESS]\n");
}

int main(void)
{
    puts("[START] event test application.\n");

    event_queue_t queue = { .waiter = (thread_t *)sched_active_thread };
    printf("posting 0x%08x\n", (unsigned)&event);
    event_post(&queue, &event);

    printf("posting 0x%08x\n", (unsigned)&event2);
    event_post(&queue, &event2);
    printf("canceling 0x%08x\n", (unsigned)&event2);
    event_cancel(&queue, &event2);

    printf("posting custom event\n");
    event_post(&queue, (event_t *)&custom_event);

    event_timeout_t event_timeout;

    printf("posting timed callback with timeout 1sec\n");
    event_timeout_init(&event_timeout, &queue, (event_t *)&event_callback);
    before = xtimer_now_usec();
    event_timeout_set(&event_timeout, 100000LU);

    printf("launching event queue\n");
    event_loop(&queue);

    return 0;
}
