/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     pkg_uwb_core
 * @{
 *
 * @file
 * @brief       uwb-core DPL (Decawave Porting Layer) callout abstraction
 *
 * Callout sets a timer that on expiration will post an event to an
 * event queue. This mimics the same as MyNewt callout api.
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#include "os/os_callout.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief dpl callout wrapper
 */
struct dpl_callout {
    struct os_callout co;   /**< the callout */
};

/**
 * @brief   Initialize a callout.
 *
 * Callouts are used to schedule events in the future onto an event
 * queue. Callout timers are scheduled using the dpl_callout_reset()
 * function.  When the timer expires, an event is posted to the event
 * queue specified in dpl_callout_init(). The event argument given here
 * is posted in the ev_arg field of that event.
 *
 * @param[out]  c       callout to initialize
 * @param[in]   q       event queue to queue event in
 * @param[in]   e_cb    callback function
 * @param[in]   e_arg   callback function argument
 */
static inline void dpl_callout_init(struct dpl_callout *c, struct dpl_eventq *q,
                      dpl_event_fn *e_cb, void *e_arg)
{
    os_callout_init(&c->co, &q->evq, (os_event_fn *) e_cb, e_arg);
}

/**
 * @brief   Reset the callout to fire off in 'ticks' ticks.
 *
 * @param[in]   c       callout to reset
 * @param[in]   ticks   number of ticks to wait before posting an event
 *
 * @return 0 on success, non-zero on failure
 */
static inline dpl_error_t dpl_callout_reset(struct dpl_callout *c, dpl_time_t ticks)
{
    return (dpl_error_t) os_callout_reset(&c->co, ticks);
}

/**
 * @brief   Stops the callout from firing.
 *
 * @param[in]   c   the callout to stop
 */
static inline void dpl_callout_stop(struct dpl_callout *c)
{
    os_callout_stop(&c->co);
}

#ifdef __cplusplus
}
#endif
