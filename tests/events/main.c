/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *               2018 Freie Universität Berlin
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
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "thread.h"
#include "event.h"
#include "event/timeout.h"
#include "event/callback.h"

#define STACKSIZE       THREAD_STACKSIZE_DEFAULT
#define PRIO            (THREAD_PRIORITY_MAIN - 1)

static char stack[STACKSIZE];

static unsigned order = 0;
static uint32_t before;

static void callback(event_t *arg);
static void custom_callback(event_t *event);
static void timed_callback(void *arg);
static void forbidden_callback(void *arg);
static void delayed_callback1(event_t *arg);
static void delayed_callback2(event_t *arg);

static event_t event = { .handler = callback };
static event_t event2 = { .handler = callback };
static event_t delayed_event1 = { .handler = delayed_callback1 };
static event_t delayed_event2 = { .handler = delayed_callback2 };

static void callback(event_t *arg)
{
    order++;
    assert(order == 3);
    assert(arg == &event);
    printf("triggered 0x%08x\n", (unsigned)arg);
}

typedef struct {
    event_t super;
    const char *text;
} custom_event_t;

static custom_event_t custom_event = { .super.handler = custom_callback, .text = "CUSTOM CALLBACK" };
static event_callback_t event_callback = EVENT_CALLBACK_INIT(timed_callback, 0x12345678);
static event_callback_t noevent_callback = EVENT_CALLBACK_INIT(forbidden_callback, 0);

static void custom_callback(event_t *event)
{
    order++;
    assert(order == 4);
    assert(event == (event_t *)&custom_event);
    custom_event_t *custom_event = (custom_event_t *)event;
    printf("triggered custom event with text: \"%s\"\n", custom_event->text);
}

static void timed_callback(void *arg)
{
    order++;
    assert(order == 5);
    assert(arg == event_callback.arg);
    uint32_t now = xtimer_now_usec();
    assert((now - before >= 100000LU));
    printf("triggered timed callback with arg 0x%08x after %" PRIu32 "us\n", (unsigned)arg, now - before);
    puts("[SUCCESS]");
}

static void forbidden_callback(void *arg)
{
    (void)arg;
    /* this callback should never be triggered! */
    puts("call to forbidden callback");
    puts("[FAILED]");
    while (1) {
        assert(false);
    }
}

static void delayed_callback1(event_t *arg)
{
    order++;
    assert(order == 1);
    assert(arg == &delayed_event1);
    printf("triggered delayed event %p\n", (void *)arg);
}

static void delayed_callback2(event_t *arg)
{
    order++;
    assert(order == 2);
    assert(arg == &delayed_event2);
    printf("triggered delayed event %p\n", (void *)arg);
}

static void *claiming_thread(void *arg)
{
    event_queue_t *dq = (event_queue_t *)arg;

    printf("claiming event queue %p\n", (void *)dq);
    event_queue_claim(dq);
    printf("launching event queue %p\n", (void *)dq);
    event_loop(dq);

    return NULL;
}

int main(void)
{
    puts("[START] event test application.\n");

    /* test creation of delayed claiming of a detached event queue */
    event_queue_t dq = EVENT_QUEUE_INIT_DETACHED;
    printf("initializing detached event queue %p\n", (void *)&dq);
    event_queue_init_detached(&dq);

    printf("posting %p\n", (void *)&delayed_event1);
    event_post(&dq, &delayed_event1);
    printf("posting %p\n", (void *)&delayed_event2);
    event_post(&dq, &delayed_event2);

    printf("running thread that will claim event queue %p\n", (void *)&dq);
    thread_create(stack, sizeof(stack), PRIO, 0, claiming_thread, &dq, "ct");

    /* test posting different kind of events in order to a statically
     * initialized queue */
    event_queue_t queue = EVENT_QUEUE_INIT;
    printf("posting 0x%08x\n", (unsigned)&event);
    event_post(&queue, &event);

    printf("posting 0x%08x\n", (unsigned)&event2);
    event_post(&queue, &event2);
    printf("canceling 0x%08x\n", (unsigned)&event2);
    event_cancel(&queue, &event2);

    puts("posting custom event");
    event_post(&queue, (event_t *)&custom_event);

    event_timeout_t event_timeout;

    puts("posting timed callback with timeout 1sec");
    event_timeout_init(&event_timeout, &queue, (event_t *)&event_callback);
    before = xtimer_now_usec();
    event_timeout_set(&event_timeout, (1 * US_PER_SEC));

    event_timeout_t event_timeout_canceled;

    puts("posting timed callback with timeout 0.5sec and canceling it again");
    event_timeout_init(&event_timeout_canceled, &queue,
                       (event_t *)&noevent_callback);
    event_timeout_set(&event_timeout_canceled, 500 * US_PER_MS);
    event_timeout_clear(&event_timeout_canceled);

    puts("launching event queue");
    event_loop(&queue);

    return 0;
}
