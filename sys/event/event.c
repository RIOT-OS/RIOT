/*
 * Copyright (C) 2017 Inria
 *               2017 Kaspar Schleiser <kaspar@schleiser.de>
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

#if IS_USED(MODULE_XTIMER)
#include "xtimer.h"
#endif

static inline bool _post_event(void *ctx)
{
    event_t *event = ctx;

    return !event->list_node.next;
}

static inline bool _post_event_counted(void *ctx)
{
    event_counted_t *event = ctx;

    ++event->num_posted;
    return event->num_posted == 1;
}

static inline void _post(event_queue_t *queue, void *event, bool counted)
{
    assert(queue && event);

    unsigned state = irq_disable();

    bool post = counted ? _post_event_counted(event) : _post_event(event);
    if (post) {
        clist_rpush(&queue->event_list, &((event_t *)event)->list_node);
    }

    thread_t *waiter = queue->waiter;
    irq_restore(state);

    if (waiter) {
        thread_flags_set(waiter, THREAD_FLAG_EVENT);
    }
}

void event_post(event_queue_t *queue, event_t *event)
{
    _post(queue, event, false);
}

void event_counted_post(event_queue_t *queue, event_counted_t *event)
{
    _post(queue, event, true);
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

void event_counted_cancel(event_queue_t *queue, event_counted_t *event,
                          bool cancel_all)
{
    assert(queue);
    assert(event);

    unsigned state = irq_disable();
    if (cancel_all) {
        event->num_posted = 1;
    }

    if (event->num_posted && --event->num_posted == 0) {
        clist_remove(&queue->event_list, &event->list_node);
    }
    irq_restore(state);
}

static event_t *_get(event_queue_t *queue)
{
    event_t *result = (event_t *)clist_lpop(&queue->event_list);

    if (result) {
        result->list_node.next = NULL;
    }

    return result;
}

static event_counted_t *_get_counted(event_queue_t *queue)
{
    clist_node_t *node = clist_lpeek(&queue->event_list);
    event_counted_t *result = container_of(node, event_counted_t, list_node);

    if (result) {
        assert(result->num_posted);
        if (--result->num_posted == 0) {
            clist_lpop(&queue->event_list);
        }
    }

    return result;
}

event_t *event_get(event_queue_t *queue)
{
    unsigned state = irq_disable();
    event_t *result = _get(queue);
    irq_restore(state);

    return result;
}

event_counted_t *event_counted_get(event_queue_t *queue)
{
    unsigned state = irq_disable();
    event_counted_t *result = _get_counted(queue);
    irq_restore(state);

    return result;
}

static void *_wait_multi(event_queue_t *queues, size_t n_queues, bool counted)
{
    assert(queues && n_queues);
    void *result = NULL;

    do {
        unsigned state = irq_disable();
        for (size_t i = 0; i < n_queues; i++) {
            assert(queues[i].waiter);
            result = counted
                   ? (void *)_get_counted(&queues[i])
                   : (void *)_get(&queues[i]);
            if (result) {
                break;
            }
        }
        irq_restore(state);
        if (result == NULL) {
            thread_flags_wait_any(THREAD_FLAG_EVENT);
        }
    } while (result == NULL);

    return result;
}

event_t *event_wait_multi(event_queue_t *queues, size_t n_queues)
{
    return _wait_multi(queues, n_queues, false);
}

event_counted_t *event_counted_wait_multi(event_queue_t *queues, size_t n_queues)
{
    return _wait_multi(queues, n_queues, true);
}

#if IS_USED(MODULE_XTIMER) || IS_USED(MODULE_ZTIMER)
static event_t *_wait_timeout(event_queue_t *queue)
{
    assert(queue);
    assert(queue->waiter);
    event_t *result;
    thread_flags_t flags = 0;

    do {
        result = event_get(queue);
        if (result == NULL) {
            flags = thread_flags_wait_any(THREAD_FLAG_EVENT |
                                          THREAD_FLAG_TIMEOUT);
        }
    } while ((result == NULL) && (flags & THREAD_FLAG_EVENT));

    return result;
}
#endif

#if IS_USED(MODULE_XTIMER)
static event_t *_wait_timeout_xtimer(event_queue_t *queue, xtimer_t *timer)
{
    event_t *result = _wait_timeout(queue);
    if (result) {
        xtimer_remove(timer);
    }

    return result;
}

event_t *event_wait_timeout(event_queue_t *queue, uint32_t timeout)
{
    xtimer_t timer;

    thread_flags_clear(THREAD_FLAG_TIMEOUT);
    xtimer_set_timeout_flag(&timer, timeout);
    return _wait_timeout_xtimer(queue, &timer);
}

event_t *event_wait_timeout64(event_queue_t *queue, uint64_t timeout)
{
    xtimer_t timer;

    thread_flags_clear(THREAD_FLAG_TIMEOUT);
    xtimer_set_timeout_flag64(&timer, timeout);
    return _wait_timeout_xtimer(queue, &timer);
}
#endif

#if IS_USED(MODULE_ZTIMER)
event_t *event_wait_timeout_ztimer(event_queue_t *queue,
                                   ztimer_clock_t *clock, uint32_t timeout)
{
    ztimer_t timer;
    event_t *result;

    thread_flags_clear(THREAD_FLAG_TIMEOUT);
    ztimer_set_timeout_flag(clock, &timer, timeout);
    result = _wait_timeout(queue);
    if (result) {
        ztimer_remove(clock, &timer);
    }
    return result;
}
#endif
