/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <assert.h>

#include <string.h>

#include "event.h"
#include "clist.h"
#include "thread.h"

void event_queue_init(event_queue_t *queue)
{
    assert(queue);
    memset(queue, '\0', sizeof(*queue));
    queue->waiter = (thread_t *)sched_active_thread;
}

void event_post(event_queue_t *queue, event_t *event)
{
    assert(queue && queue->waiter && event);

    unsigned state = irq_disable();
    if (!event->list_node.next) {
        clist_rpush(&queue->event_list, &event->list_node);
    }
    irq_restore(state);

    thread_flags_set(queue->waiter, THREAD_FLAG_EVENT);
}

void event_cancel(event_queue_t *queue, event_t *event)
{
    assert(queue);
    assert(event);

    unsigned state = irq_disable();
    clist_remove(&queue->event_list, &event->list_node);
    event->list_node.next = NULL;
    irq_restore(state);
}

event_t *event_get(event_queue_t *queue)
{
    unsigned state = irq_disable();
    event_t *result = (event_t *) clist_lpop(&queue->event_list);

    irq_restore(state);
    if (result) {
        result->list_node.next = NULL;
    }
    return result;
}

event_t *event_wait(event_queue_t *queue)
{
    event_t *result;
    do {
        thread_flags_wait_any(THREAD_FLAG_EVENT);
        unsigned state = irq_disable();
        result = (event_t *) clist_lpop(&queue->event_list);
        if (clist_rpeek(&queue->event_list)) {
            queue->waiter->flags |= THREAD_FLAG_EVENT;
        }
        irq_restore(state);
        /* `result` will be NULL if all queued events are canceled after the
         * thread flag has been set but before clist_lpop has been called. One
         * example where this may occur is when using event_cancel from inside
         * the handler of an earlier event in the same queue, or when using
         * event_cancel from inside an interrupt handler. */
    } while (!result);
    result->list_node.next = NULL;
    return result;
}

void event_loop(event_queue_t *queue)
{
    event_t *event;

    while ((event = event_wait(queue))) {
        event->handler(event);
    }
}
