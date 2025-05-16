/*
 * Copyright (C) 2017 Inria
 *               2017 Kaspar Schleiser <kaspar@schleiser.de>
 *               2018-2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

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
 *
 * An event queue is bound to a thread, but any thread or ISR can put events
 * into a queue. In most cases, the owning thread of a queue is set during the
 * queue's initialization. But it is also possible to initialize a queue in a
 * detached state from a different context and to set the owning thread
 * at a later point of time using the event_queue_claim() function.
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
 *    this implementation.
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
 *     custom_event_t *custom_event = container_of(event, custom_event_t, super);
 *     printf("triggered custom event with text: \"%s\"\n", custom_event->text);
 * }
 *
 * static custom_event_t custom_event = { .super.handler = custom_handler, .text = "CUSTOM EVENT" };
 *
 * [...] event_post(&queue, &custom_event.super)
 * ~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @{
 *
 * @file
 * @brief       Event API
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "assert.h"
#include "clist.h"
#include "irq.h"
#include "thread.h"
#include "thread_flags.h"
#include "ptrtag.h"
#include "ztimer.h"

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
#define EVENT_QUEUE_INIT    { .waiter = thread_get_active() }

/**
 * @brief   static initializer for detached event queues
 */
#define EVENT_QUEUE_INIT_DETACHED   { .waiter = NULL }

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
typedef struct PTRTAG {
    clist_node_t event_list;    /**< list of queued events              */
    thread_t *waiter;           /**< thread owning event queue          */
} event_queue_t;

/**
 * @brief   Initialize an array of event queues
 *
 * This will set the calling thread as owner of each queue in @p queues.
 *
 * @param[out]  queues      event queue objects to initialize
 * @param[in]   n_queues    number of queues in @p queues
 */
static inline void event_queues_init(event_queue_t *queues,
                                          size_t n_queues)
{
    assert(queues && n_queues);
    thread_t *me = thread_get_active();
    for (size_t i = 0; i < n_queues; i++) {
        memset(&queues[i], '\0', sizeof(queues[0]));
        queues[i].waiter = me;
    }
}

/**
 * @brief   Initialize an event queue
 *
 * This will set the calling thread as owner of @p queue.
 *
 * @param[out]  queue   event queue object to initialize
 */
static inline void event_queue_init(event_queue_t *queue)
{
    event_queues_init(queue, 1);
}

/**
 * @brief   Initialize an array of event queues not binding it to a thread
 *
 * @param[out]  queues      event queue objects to initialize
 * @param[in]   n_queues    number of queues in @p queues
 */
static inline void event_queues_init_detached(event_queue_t *queues,
                                             size_t n_queues)
{
    assert(queues);
    for (size_t i = 0; i < n_queues; i++) {
        memset(&queues[i], '\0', sizeof(queues[0]));
    }
}

/**
 * @brief   Initialize an event queue not binding it to a thread
 *
 * @param[out]  queue   event queue object to initialize
 */
static inline void event_queue_init_detached(event_queue_t *queue)
{
    event_queues_init_detached(queue, 1);
}

/**
 * @brief   Bind an array of event queues to the calling thread
 *
 * This function must only be called once and only if the given queue is not
 * yet bound to a thread.
 *
 * @pre     (queues[i].waiter == NULL for i in {0, ..., n_queues - 1})
 *
 * @param[out]  queues      event queue objects to bind to a thread
 * @param[in]   n_queues    number of queues in @p queues
 */
static inline void event_queues_claim(event_queue_t *queues, size_t n_queues)
{
    assert(queues);
    thread_t *me = thread_get_active();
    for (size_t i = 0; i < n_queues; i++) {
        assert(queues[i].waiter == NULL);
        queues[i].waiter = me;
    }
}

/**
 * @brief   Bind an event queue to the calling thread
 *
 * This function must only be called once and only if the given queue is not
 * yet bound to a thread.
 *
 * @pre     (queue->waiter == NULL)
 *
 * @param[out]  queue   event queue object to bind to a thread
 */
static inline void event_queue_claim(event_queue_t *queue)
{
    event_queues_claim(queue, 1);
}

/**
 * @brief   Queue an event
 *
 * The given event will be posted on the given @p queue. If the event is already
 * queued when calling this function, the event will not be touched and remain
 * in the previous position on the queue. So reposting an event while it is
 * already on the queue will have no effect.
 *
 * @pre     queue should be initialized
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
 * @brief   Check if an event is already queued
 *
 * @param[in]   queue   event queue to check
 * @param[in]   event   event to check
 *
 * @returns true if @p event is in @p queue
 * @returns false otherwise
 */
bool event_is_queued(const event_queue_t *queue, const event_t *event);

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
 * @brief   Get next event from the given event queues, blocking
 *
 * This function will block until an event becomes available. If more than one
 * queue contains an event, the queue with the lowest index is chosen. Thus,
 * a lower index in the @p queues array translates into a higher priority of
 * the queue.
 *
 * In order to handle an event retrieved using this function,
 * call event->handler(event).
 *
 * @warning There can only be a single waiter on a queue!
 *
 * @note    This function *can* be suitable for having a single thread
 *          handling both real-time and non-real-time events. However, a real
 *          time event can be delayed for the whole duration a single
 *          non-real-time event takes (in addition to all other sources of
 *          latency). Thus, the slowest to handle non-real-time event must still
 *          execute fast enough to add an amount of latency (on top of other
 *          sources of latency) that is acceptable to the real-time event with
 *          the strictest requirements.
 *
 * @pre     0 < @p n_queues (expect blowing `assert()` otherwise)
 * @pre     The queue must have a waiter (i.e. it should have been claimed, or
 *          initialized using @ref event_queue_init, @ref event_queues_init)
 *
 * @param[in]   queues      Array of event queues to get event from
 * @param[in]   n_queues    Number of event queues passed in @p queues
 *
 * @returns     pointer to next event
 */
event_t *event_wait_multi(event_queue_t *queues, size_t n_queues);

/**
 * @brief   Get next event from event queue, blocking
 *
 * This function will block until an event becomes available.
 *
 * In order to handle an event retrieved using this function,
 * call event->handler(event).
 *
 * @warning     There can only be a single waiter on a queue!
 *
 * @pre     The queue must have a waiter (i.e. it should have been claimed, or
 *          initialized using @ref event_queue_init, @ref event_queues_init)
 *
 * @param[in]   queue   event queue to get event from
 *
 * @returns     pointer to next event
 */
static inline event_t *event_wait(event_queue_t *queue)
{
    return event_wait_multi(queue, 1);
}

#if IS_USED(MODULE_XTIMER) || defined(DOXYGEN)
/**
 * @brief   Get next event from event queue, blocking until timeout expires
 *
 * @pre     The queue must have a waiter (i.e. it should have been claimed, or
 *          initialized using @ref event_queue_init, @ref event_queues_init)
 *
 * @param[in]   queue    queue to query for an event
 * @param[in]   timeout  maximum time to wait for an event to be posted in us
 *
 * @return      pointer to next event if event was taken from the queue
 * @return      NULL if timeout expired before an event was posted
 */
event_t *event_wait_timeout(event_queue_t *queue, uint32_t timeout);

/**
 * @brief   Get next event from event queue, blocking until timeout expires
 *
 * @pre     The queue must have a waiter (i.e. it should have been claimed, or
 *          initialized using @ref event_queue_init, @ref event_queues_init)
 *
 * @param[in]   queue    queue to query for an event
 * @param[in]   timeout  maximum time to wait for an event to be posted in us
 *
 * @return      pointer to next event if event was taken from the queue
 * @return      NULL if timeout expired before an event was posted
 */
event_t *event_wait_timeout64(event_queue_t *queue, uint64_t timeout);
#endif

#if IS_USED(MODULE_ZTIMER) || defined(DOXYGEN)
/**
 * @brief   Get next event from event queue, blocking until timeout expires
 *
 * This function is the same as event_wait_timeout() with the difference that it
 * uses ztimer instead of xtimer as timer backend.
 *
 * @pre     The queue must have a waiter (i.e. it should have been claimed, or
 *          initialized using @ref event_queue_init, @ref event_queues_init)
 *
 * @param[in]   queue    queue to query for an event
 * @param[in]   clock    ztimer clock to use
 * @param[in]   timeout  maximum time to wait for an event, time unit depends
 *                       on the used ztimer clock
 *
 * @return      pointer to next event if event was taken from the queue
 * @return      NULL if timeout expired before an event was posted
 */
event_t *event_wait_timeout_ztimer(event_queue_t *queue,
                                   ztimer_clock_t *clock, uint32_t timeout);
#endif

/**
 * @brief   Simple event loop with multiple queues
 *
 * This function will forever sit in a loop, waiting for events to be queued
 * and executing their handlers. If more than one queue contains an event, the
 * queue with the lowest index is chosen. Thus, a lower index in the @p queues
 * array translates into a higher priority of the queue.
 *
 * It is pretty much defined as:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 *     while ((event = event_wait_multi(queues, n_queues))) {
 *         event->handler(event);
 *     }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @see event_wait_multi
 *
 * @pre     The queue must have a waiter (i.e. it should have been claimed, or
 *          initialized using @ref event_queue_init, @ref event_queues_init)
 *
 * @note    Enable the `event_loop_debug` module to print the execution times of
 *          the event handler functions.
 *
 * @param[in]   queues      Event queues to process
 * @param[in]   n_queues    Number of queues passed with @p queues
 */
static inline void event_loop_multi(event_queue_t *queues, size_t n_queues)
{
    event_t *event;

    while ((event = event_wait_multi(queues, n_queues))) {
        if (IS_USED(MODULE_EVENT_LOOP_DEBUG)) {
            uint32_t now;
            ztimer_acquire(ZTIMER_USEC);

            void _event_callback_handler(event_t *event);
            if (!IS_USED(MODULE_EVENT_CALLBACK) ||
                event->handler != _event_callback_handler) {
                printf("event: executing %p->%p\n",
                       (void *)event, (void *)(uintptr_t)event->handler);
            }
            now = ztimer_now(ZTIMER_USEC);

            event->handler(event);

            printf("event: %p took %" PRIu32 " µs\n",
                   (void *)event, ztimer_now(ZTIMER_USEC) - now);
            ztimer_release(ZTIMER_USEC);
        }
        else {
            event->handler(event);
        }
    }
}

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
 * @pre     The queue must have a waiter (i.e. it should have been claimed, or
 *          initialized using @ref event_queue_init, @ref event_queues_init)
 *
 * @note    Enable the `event_loop_debug` module to print the execution times of
 *          the event handler functions.
 *
 * @param[in]   queue   event queue to process
 */
static inline void event_loop(event_queue_t *queue)
{
    event_loop_multi(queue, 1);
}

/**
 * @brief Synchronize with the last event on the queue
 *
 * Blocks until the last event on the queue at the moment of calling this is
 * processed.
 *
 * @warning May not be called from the event queue, as it would block forever.
 * @warning If the queue has no waiter, this will block until the queue is
 *          claimed. See @ref event_queue_claim()
 *
 * @param[in] queue event queue to sync with
 *
 * Usage example:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * event_post(queue, my_event);
 * // When event_sync() returns, my_event will have been processed.
 * event_sync(queue);
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void event_sync(event_queue_t *queue);

#ifdef __cplusplus
}
#endif
/** @} */
