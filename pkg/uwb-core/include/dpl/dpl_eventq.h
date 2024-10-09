/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_uwb_core
 * @{
 *
 * @file
 * @brief       uwb-core DPL (Decawave Porting Layer) event queue wrappers
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#ifndef DPL_DPL_EVENTQ_H
#define DPL_DPL_EVENTQ_H

#include <dpl/dpl_types.h>

#include "os/os_eventq.h"
#include "uwb_core.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief dpl event wrapper
 */
struct dpl_event {
    struct os_event ev;     /**< the event */
};

/**
 * @brief dpl event queue wrapper
 */
struct dpl_eventq {
    struct os_eventq evq;   /**< the event queue */
};

/**
 * @brief dpl event callback function
 */
typedef void dpl_event_fn(struct dpl_event *ev);

/**
 * @brief   Init a event
 *
 * @param[in]   ev      pointer to event to set
 * @param[in]   fn      event callback function
 * @param[in]   arg     event argument
 */
static inline void dpl_event_init(struct dpl_event *ev, dpl_event_fn * fn,
                                 void *arg)
{
    os_event_init(&ev->ev, (os_event_fn*) fn, arg);
}

/**
 * @brief   Check if event is in queue
 *
 * @param[in]   ev      event to check
 *
 * @return  true if event is queues, false otherwise
 */
static inline bool dpl_event_is_queued(struct dpl_event *ev)
{
    return os_event_is_queued(&ev->ev);
}

/**
 * @brief   Runs an event
 *
 * @param[in]   ev      event to run
 */
static inline void *dpl_event_get_arg(struct dpl_event *ev)
{
    return os_event_get_arg(&ev->ev);
}

/**
 * @brief   Set the vent arg
 *
 * @param[in]   ev      event
 * @param[in]   arg     arg to set event
 */
static inline void dpl_event_set_arg(struct dpl_event *ev, void *arg)
{
    os_event_set_arg(&ev->ev, arg);
}

/**
 * @brief   Runs an event
 *
 * @param[in]   ev      event to run
 */
static inline void dpl_event_run(struct dpl_event *ev)
{
   os_event_run(&ev->ev);
}

/**
 * @brief   Initialize the event queue
 *
 * @param[in]   evq     The event queue to initialize
 */
static inline void dpl_eventq_init(struct dpl_eventq *evq)
{
    os_eventq_init(&evq->evq);
}

/**
 * @brief   Check whether the event queue is initialized.
 *
 * @param[in]   evq     the event queue to check
 */
static inline int dpl_eventq_inited(struct dpl_eventq *evq)
{
    return os_eventq_inited(&evq->evq);
}

/**
 * @brief   Deinitialize an event queue
 *
 * @note    Not supported in RIOT
 *
 * @param[in]   evq     the event queue to deinit
 */
static inline void dpl_eventq_deinit(struct dpl_eventq *evq)
{
    (void) evq;
    /* Can't deinit an eventq in RIOT */
}

/**
 * @brief   Get next event from event queue, blocking.
 *
 * @param[in]   evq     the event queue to pull an event from
 *
 * @return  the event from the queue
 */
static inline struct dpl_event * dpl_eventq_get(struct dpl_eventq *evq)
{
    return (struct dpl_event *) os_eventq_get(&evq->evq, DPL_WAIT_FOREVER);
}

/**
 * @brief   Get next event from event queue, non-blocking
 *
 * @return  event from the queue, or NULL if none available.
 */
static inline struct dpl_event * dpl_eventq_get_no_wait(struct dpl_eventq *evq)
{
    return (struct dpl_event *) os_eventq_get_no_wait(&evq->evq);
}

/**
 * @brief   Put an event on the event queue.
 *
 * @param[in]   evq     event queue
 * @param[in]   ev      event to put in queue
 */
static inline void dpl_eventq_put(struct dpl_eventq *evq, struct dpl_event *ev)
{
    os_eventq_put(&evq->evq, &ev->ev);
}

/**
 * @brief   Remove an event from the queue.
 *
 * @param[in]   evq     event queue to remove the event from
 * @param[in]   ev      event to remove from the queue
 */
static inline void dpl_eventq_remove(struct dpl_eventq *evq, struct dpl_event *ev)
{
    os_eventq_remove(&evq->evq, &ev->ev);
}

/**
 * @brief   Gets and runs an event from the queue callback.
 *
 * @param[in]   evq     The event queue to pull the item off.
 */
static inline void dpl_eventq_run(struct dpl_eventq *evq)
{
    os_eventq_run(&evq->evq);
}

/**
 * @brief   Check if queue is empty
 *
 * @param[in]   evq     the event queue to check
 *
 * @return      true    if empty, false otherwise
 */
static inline bool dpl_eventq_is_empty(struct dpl_eventq *evq)
{
    return os_eventq_is_empty(&evq->evq);
}

/**
 * @brief   Retrieves the default event queue.
 *
 * As there is no default event queue in RIOT, uwb-core will start and
 * handle a default event queue.
 *
 * @return  the default event queue.
 */
static inline struct dpl_eventq * dpl_eventq_dflt_get(void)
{
    return (struct dpl_eventq *) uwb_core_get_eventq();
}

#ifdef __cplusplus
}
#endif

#endif /* DPL_DPL_EVENTQ_H */
