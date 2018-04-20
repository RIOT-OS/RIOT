/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_event Event Queue
 * @ingroup     sys
 * @brief       Provides an Event loop
 *
 * This module offers an event queue framework like libevent or libuev.
 *
 * An event queue is basically a FIFO queue of events, with some functions to
 * efficiently and safely handle adding and getting events to / from such a
 * queue.
 * An event queue is bound to a thread, but any thread or ISR can put events
 * into a queue.
 *
 * An event is a structure containing a pointer to an event handler. It can be
 * extended to provide context or arguments to the handler.  It can also be
 * embedded into existing structures (see examples).
 *
 * Compared to msg or mbox, this some fundamental differences:
 *
 * 1. events are "sender allocated". Unlike msg_send(), event_post() never
 *    blocks or fails.
 * 2. events contain everything necessary to handle them, thus a thread
 *    processing the events of an event queue doesn't need to be changed in
 *    order to support new event types.
 * 3. events can be safely used (and actually perform best) when used within
 *    one thread, e.g., in order to create a state-machine like process flow.
 *    This is not (easily) possible using msg queues, as they might fill up.
 * 4. an event can only be queued in one event queue at the same time.
 *    Notifying many queues using only one event object is not possible with
 *    this imlementation.
 *
 * At the core, event_wait() uses thread flags to implement waiting for events
 * to be queued. Thus event queues can be used safely and efficiently in combination
 * with thread flags and msg queues.
 *
 * Examples:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * // simple event handler
 * static void handler(event_t *event)
 * {
 *    printf("triggered 0x%08x\n", (unsigned)event);
 * }
 *
 * static event_t event = { .handler = handler };
 * static event_queue_t queue;
 *
 * int main(void)
 * {
 *     event_queue_init(&queue);
 *     event_loop(&queue);
 * }
 *
 * [...] event_post(&queue, &event);
 *
 * // example for event extended event struct
 * typedef struct {
 *     event_t super;
 *     const char *text;
 * } custom_event_t;
 *
 * static void custom_handler(event_t *event)
 * {
 *     custom_event_t *custom_event = (custom_event_t *)event;
 *     printf("triggered custom event with text: \"%s\"\n", custom_event->text);
 * }
 *
 * static custom_event_t custom_event = { .super.handler = custom_handler, .text = "CUSTOM EVENT" };
 *
 * [...] event_post(&queue, &custom_event)
 * ~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @{
 *
 * @file
 * @brief       Event API
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef EVENT_H
#define EVENT_H

#include <stdint.h>

#include "irq.h"
#include "thread_flags.h"
#include "clist.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef THREAD_FLAG_EVENT
/**
 * @brief   Thread flag use to notify available events in an event queue
 */
#define THREAD_FLAG_EVENT   (0x1)
#endif

/**
 * @brief   event_queue_t static initializer
 */
#define EVENT_QUEUE_INIT    { .waiter = (thread_t *)sched_active_thread }

/**
 * @brief   event structure forward declaration
 */
typedef struct event event_t;

/**
 * @brief   event handler type definition
 */
typedef void (*event_handler_t)(event_t *);

/**
 * @brief   event structure
 */
struct event {
    clist_node_t list_node;     /**< event queue list entry             */
    event_handler_t handler;    /**< pointer to event handler function  */
};

/**
 * @brief   event queue structure
 */
typedef struct {
    clist_node_t event_list;    /**< list of queued events              */
    thread_t *waiter;           /**< thread ownning event queue         */
} event_queue_t;

/**
 * @brief   Initialize an event queue
 *
 * This will set the calling thread as owner of @p queue.
 *
 * @param[out]  queue   event queue object to initialize
 */
void event_queue_init(event_queue_t *queue);

/**
 * @brief   Queue an event
 *
 * The given event will be posted on the given @p queue. If the event is already
 * queued when calling this function, the event will not be touched and remain
 * in the previous position on the queue. So reposting an event while it is
 * already on the queue will have no effect.
 *
 * @param[in]   queue   event queue to queue event in
 * @param[in]   event   event to queue in event queue
 */
void event_post(event_queue_t *queue, event_t *event);

/**
 * @brief   Cancel a queued event
 *
 * This will remove a queued event from an event queue.
 *
 * @note    Due to the underlying list implementation, this will run in O(n).
 *
 * @param[in]   queue   event queue to remove event from
 * @param[in]   event   event to remove from queue
 */
void event_cancel(event_queue_t *queue, event_t *event);

/**
 * @brief   Get next event from event queue, non-blocking
 *
 * In order to handle an event retrieved using this function,
 * call event->handler(event).
 *
 * @param[in]   queue   event queue to get event from
 *
 * @returns     pointer to next event
 * @returns     NULL if no event available
 */
event_t *event_get(event_queue_t *queue);

/**
 * @brief   Get next event from event queue, blocking
 *
 * This function will block until an event becomes available.
 *
 * In order to handle an event retrieved using this function,
 * call event->handler(event).
 *
 * @param[in]   queue   event queue to get event from
 *
 * @returns     pointer to next event
 */
event_t *event_wait(event_queue_t *queue);

/**
 * @brief   Simple event loop
 *
 * This function will forever sit in a loop, waiting for events to be queued
 * and executing their handlers.
 *
 * It is pretty much defined as:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 *     while ((event = event_wait(queue))) {
 *         event->handler(event);
 *     }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @param[in]   queue   event queue to process
 */
void event_loop(event_queue_t *queue);

#ifdef __cplusplus
}
#endif
#endif /* EVENT_H */
/** @} */
