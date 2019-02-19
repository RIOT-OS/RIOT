/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *               2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_event
 * @{
 *
 * @file
 * @brief       Event loop implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <assert.h>

#include <string.h>

#include "event.h"
#include "clist.h"
#include "thread.h"

void event_queue_init_detached(event_queue_t *queue)
{
    assert(queue);
    memset(queue, '\0', sizeof(*queue));
}

void event_queue_init(event_queue_t *queue)
{
    assert(queue);
    memset(queue, '\0', sizeof(*queue));
    queue->waiter = (thread_t *)sched_active_thread;
}

void event_queue_claim(event_queue_t *queue)
{
    assert(queue && (queue->waiter == NULL));
    queue->waiter = (thread_t *)sched_active_thread;
}

void event_post(event_queue_t *queue, event_t *event)
{
    assert(queue && event);

    unsigned state = irq_disable();
    if (!event->list_node.next) {
        clist_rpush(&queue->event_list, &event->list_node);
    }
    thread_t *waiter = queue->waiter;
    irq_restore(state);

    /* WARNING: there is a minimal chance, that a waiter claims a formerly
     *          detached queue between the end of the critical section above and
     *          the block below. In that case, the new waiter will not be woken
     *          up. This should be fixed at some point once it is safe to call
     *          thread_flags_set() inside a critical section on all platforms. */
    if (waiter) {
        thread_flags_set(waiter, THREAD_FLAG_EVENT);
    }
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
    assert(queue);
    event_t *result;

    do {
        unsigned state = irq_disable();
        result = (event_t *)clist_lpop(&queue->event_list);
        irq_restore(state);
        if (result == NULL) {
            thread_flags_wait_any(THREAD_FLAG_EVENT);
        }
    } while (result == NULL);

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
