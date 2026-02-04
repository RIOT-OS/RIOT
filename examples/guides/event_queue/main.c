/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <stdio.h>
#include "event.h"
#include "event/thread.h"
#include "event/periodic.h"
#include "ztimer.h"

/* A custom event structure that extends the base event structure
 * the custom event carries an additional text field.
 */
typedef struct {
    event_t super;
    const char *text;
} custom_event_t;

/* Function to handle regular events */
static void regular_handler(event_t *event)
{
    (void) event;
    printf("\tTriggered regular event.\n");
}

/* Function to handle custom events */
static void custom_handler(event_t *event)
{
    /* The handler receives a pointer to the base event structure.
     * We need to get the pointer to our custom event structure.
     */
    custom_event_t *custom_event = container_of(event, custom_event_t, super);
    printf("\tTriggered custom event with text: \"%s\".\n", custom_event->text);
}

/* Defining a regular event and setting its handler */
static event_t          event        = { .handler = regular_handler };
/* Defining a custom event and setting its handler and text */
static custom_event_t   custom_event = { .super.handler = custom_handler,
                                         .text = "CUSTOM EVENT" };
/* Declaring a periodic event */
static event_periodic_t periodic_event;
/* Declaring an event queue */
static event_queue_t    my_queue;

/* This thread will handle events from the event queue */
void *event_handler_thread(void *arg)
{
    assert(arg != NULL);
    event_queue_t *queue = (event_queue_t *)arg;

    /* A thread must own an event queue to process its events.
     * Ownership is initially assigned to the thread that initializes the queue.
     * It can later be transferred by calling `event_queue_claim`.
     */
    event_queue_init(queue);

    puts("Event handler thread listening for events...");
    event_loop(queue);
    return NULL;
}

/* Stack for the event handler thread */
char _event_handler_thread_stack[THREAD_STACKSIZE_MAIN];

int main(void)
{

    /* Starting the event handler thread */
    thread_create(_event_handler_thread_stack, sizeof(_event_handler_thread_stack),
                  THREAD_PRIORITY_MAIN - 1, 0, event_handler_thread, &my_queue,
                  "Event Handler Thread");

    puts("Posting regular event...");
    /* Posting a regular event to the queue handled by our event handler thread */
    event_post(&my_queue, &event);
    ztimer_sleep(ZTIMER_SEC, 1);

    puts("Posting custom event...");
    /* Posting a custom event to the queue handled by our event handler thread */
    event_post(&my_queue, &custom_event.super);
    ztimer_sleep(ZTIMER_SEC, 1);

    puts("Posting regular event to the medium priority event thread...");
    /* Posting a regular event to medium priority queue.
     * This queue will initialized and handled by the `event_thread` module.
     */
    event_post(EVENT_PRIO_MEDIUM, &event);
    ztimer_sleep(ZTIMER_SEC, 1);

    puts("Starting periodic event that posts regular events every second...");
    /* This initializes the periodic event. We set the timer it should use,
     * the queue it should post to, and the event it should post.
     */
    event_periodic_init(&periodic_event, ZTIMER_SEC, EVENT_PRIO_MEDIUM, &event);
    /* This starts the periodic event with a period of 1 second. */
    event_periodic_start(&periodic_event, 1);

    return 0;
}
