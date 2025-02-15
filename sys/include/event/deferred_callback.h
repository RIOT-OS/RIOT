/*
 * Copyright (C) 2025 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_event
 * @brief       Trigger an event callback after a timeout
 *
 * This provides convenience functions to trigger a callback event after
 * some time has passed.
 *
 * @{
 *
 * @file
 * @brief       Event Deferred Callback API
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 */

#ifndef EVENT_DEFERRED_CALLBACK_H
#define EVENT_DEFERRED_CALLBACK_H

#include <assert.h>
#include "event/callback.h"
#include "ztimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Deferred Callback Event structure
 */
typedef struct {
    event_callback_t event;     /**< callback event portion */
    event_queue_t *queue;       /**< event queue to post event to */
    ztimer_t timer;             /**< ztimer object used for timeout */
} event_deferred_callback_t;

/**
 * @brief     Internal helper function for ztimer callback
 * @param[in] arg   event structure
 */
static inline void _event_deferred_post(void *arg)
{
    event_deferred_callback_t *event_timeout = arg;

    event_post(event_timeout->queue, arg);
}

/**
 * @brief   Execute a callback function in the event thread after a timeout
 *
 * @warning @p event must be kept allocated until the callback was executed
 *
 * @param[in]   event           event_deferred_callback object to initialize
 * @param[in]   queue           queue that the timed-out event will be added to
 * @param[in]   clock           the clock to configure this timer on
 * @param[in]   timeout         timer target (relative ticks from now)
 * @param[in]   callback        callback to set up
 * @param[in]   arg             callback argument to set up
 */
static inline void event_deferred_callback_post(event_deferred_callback_t *event,
                                                event_queue_t *queue,
                                                ztimer_clock_t *clock, uint32_t timeout,
                                                void (*callback)(void *), void *arg)
{
    event->event = (event_callback_t) {
        .super.handler = _event_callback_handler,
        .callback = callback,
        .arg = arg,
    };
    event->timer = (ztimer_t) {
        .callback = _event_deferred_post,
        .arg = event,
    };
    event->queue = queue;

    ztimer_set(clock, &event->timer, timeout);
}

/**
 * @brief   Cancel a callback function if it has not been executed yet
 *
 * @param[in]   event           event_deferred_callback object to cancel
 * @param[in]   clock           the clock this timer runs on
 */
static inline void event_deferred_callback_cancel(event_deferred_callback_t *event,
                                                  ztimer_clock_t *clock)
{
    ztimer_remove(clock, &event->timer);
    event_cancel(event->queue, (event_t *)event);
}

#ifdef __cplusplus
}
#endif
#endif /* EVENT_DEFERRED_CALLBACK_H */
/** @} */
