/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_mynewt_core
 * @{
 *
 * @file
 * @brief       mynewt-core event and event queue abstraction
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#ifndef OS_OS_EVENTQ_H
#define OS_OS_EVENTQ_H

#include <os/os_types.h>

#include "event/callback.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Event wrapper
 */
struct os_event
{
    event_callback_t e; /**< the event callback */
    void *arg;          /**< the event argument */
};

/**
 * @brief Event queue wrapper
 */
struct os_eventq
{
    event_queue_t q;    /**< the event queue */
};

/**
 * @brief Event callback function
 */
typedef void os_event_fn(struct os_event *ev);

/**
 * @brief   Init a event
 *
 * @param[in]   ev      pointer to event to set
 * @param[in]   fn      event callback function
 * @param[in]   arg     event argument
 */
static inline void os_event_init(struct os_event *ev, os_event_fn * fn,
                                 void *arg)
{
    /*
     * Need to clear list_node manually since init function below does not do
     * this.
     */
    ev->e.super.list_node.next = NULL;
    event_callback_init(&ev->e, (void(*)(void *))fn, ev);
    ev->arg = arg;
}

/**
 * @brief   Check if event is in queue
 *
 * @param[in]   ev      event to check
 *
 * @return  true if event is queues, false otherwise
 */
static inline bool os_event_is_queued(struct os_event *ev)
{
    return (ev->e.super.list_node.next != NULL);
}

/**
 * @brief   Returns event argument
 *
 * @param[in]   ev      event
 */
static inline void *os_event_get_arg(struct os_event *ev)
{
    return ev->arg;
}

/**
 * @brief   Set the event argument
 *
 * @param[in]   ev      event
 * @param[in]   arg     arg to set event
 */
static inline void os_event_set_arg(struct os_event *ev, void *arg)
{
    ev->arg = arg;
}

/**
 * @brief   Runs an event
 *
 * @param[in]   ev      event to run
 */
static inline void os_event_run(struct os_event *ev)
{
    ev->e.super.handler(&ev->e.super);
}

/**
 * @brief   Initialize the event queue
 *
 * @param[in]   evq     The event queue to initialize
 */
static inline void os_eventq_init(struct os_eventq *evq)
{
    event_queue_init_detached(&evq->q);
}

/**
 * @brief   Check whether the event queue is initialized.
 *
 * @param[in]   evq     the event queue to check
 */
static inline int os_eventq_inited(struct os_eventq *evq)
{
    return evq->q.waiter != NULL;
}

/**
 * @brief   Deinitialize an event queue
 *
 * @note    Not supported in RIOT
 *
 * @param[in]   evq     the event queue to deinit
 */
static inline void os_eventq_deinit(struct os_eventq *evq)
{
    (void) evq;
    /* Can't deinit an eventq in RIOT */
}

/**
* @brief   Get next event from event queue
 *
 * @param[in]   evq     the event queue to pull an event from
 * @param[in]   tmo     timeout, OS_WAIT_FOREVER to block, 0 to return immediately
 *
 * @return  the event from the queue
 */
static inline struct os_event * os_eventq_get(struct os_eventq *evq, os_time_t tmo)
{
    if (evq->q.waiter == NULL) {
        event_queue_claim(&evq->q);
    }

    if (tmo == 0) {
        return (struct os_event *)event_get(&evq->q);
    } else if (tmo == OS_WAIT_FOREVER) {
        return (struct os_event *)event_wait(&evq->q);
    } else {
        return (struct os_event *)event_wait_timeout_ztimer(&evq->q,
                                                            ZTIMER_MSEC,
                                                            (uint32_t)tmo);
    }
}

/**
 * @brief   Get next event from event queue, non-blocking
 *
 * @return  event from the queue, or NULL if none available.
 */
static inline struct os_event * os_eventq_get_no_wait(struct os_eventq *evq)
{
    if (evq->q.waiter == NULL) {
        event_queue_claim(&evq->q);
    }

    return (struct os_event *) event_get(&evq->q);
}

/**
 * @brief   Put an event on the event queue.
 *
 * @param[in]   evq     event queue
 * @param[in]   ev      event to put in queue
 */
static inline void os_eventq_put(struct os_eventq *evq, struct os_event *ev)
{
    event_post(&evq->q, &ev->e.super);
}

/**
 * @brief   Remove an event from the queue.
 *
 * @param[in]   evq     event queue to remove the event from
 * @param[in]   ev      event to remove from the queue
 */
static inline void os_eventq_remove(struct os_eventq *evq, struct os_event *ev)
{
    event_cancel(&evq->q, &ev->e.super);
}

/**
 * @brief   Gets and runs an event from the queue callback.
 *
 * @param[in]   evq     The event queue to pull the item off.
 */
static inline void os_eventq_run(struct os_eventq *evq)
{
    struct os_event *ev = os_eventq_get(evq, OS_WAIT_FOREVER);
    os_event_run(ev);
}

/**
 * @brief   Check if queue is empty
 *
 * @param[in]   evq     the event queue to check
 *
 * @return      true    if empty, false otherwise
 */
static inline bool os_eventq_is_empty(struct os_eventq *evq)
{
    return clist_count(&(evq->q.event_list)) == 0;
}

#ifdef __cplusplus
}
#endif

#endif /* OS_OS_EVENTQ_H */
