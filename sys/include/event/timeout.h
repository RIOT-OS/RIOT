/*
 * Copyright (C) 2017 Inria
 *               2017 Freie Universität Berlin
 *               2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef EVENT_TIMEOUT_H
#define EVENT_TIMEOUT_H

/**
 * @ingroup     sys_event
 * @brief       Provides functionality to trigger events after timeout
 *
 * event_timeout intentionally doesn't extend event structures in order to
 * support events that are integrated in larger structs intrusively.
 *
 * Example:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * event_timeout_t event_timeout;
 *
 * printf("posting timed callback with timeout 1sec\n");
 * event_timeout_init(&event_timeout, &queue, (event_t*)&event);
 * event_timeout_set(&event_timeout, 1000000);
 * [...]
 * ~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @{
 *
 * @file
 * @brief       Event Timeout API
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#include "event.h"
#include "ztimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Timeout Event structure
 */
typedef struct {
    ztimer_clock_t *clock;  /**< ztimer clock to use */
    ztimer_t timer;         /**< ztimer object used for timeout */
    event_queue_t *queue;   /**< event queue to post event to   */
    event_t *event;         /**< event to post after timeout    */
} event_timeout_t;

/**
 * @brief   Initialize timeout event object
 *
 * @param[in]   event_timeout   event_timeout object to initialize
 * @param[in]   clock           the clock backend, eg: ZTIMER_USEC, ZTIMER_MSEC
 * @param[in]   queue           queue that the timed-out event will be added to
 * @param[in]   event           event to add to queue after timeout
 */
void event_timeout_ztimer_init(event_timeout_t *event_timeout, ztimer_clock_t *clock,
                               event_queue_t *queue, event_t *event);

#if IS_USED(MODULE_EVENT_TIMEOUT) || DOXYGEN
/**
 * @brief   Initialize timeout event object
 *
 * @note    If ztimer is used the default time clock backend is ZTIMER_USEC
 *
 * @param[in]   event_timeout   event_timeout object to initialize
 * @param[in]   queue           queue that the timed-out event will be added to
 * @param[in]   event           event to add to queue after timeout
 */
void event_timeout_init(event_timeout_t *event_timeout, event_queue_t *queue,
                        event_t *event);
#endif

/**
 * @brief   Set a timeout
 *
 * This will make the event as configured in @p event_timeout be triggered
 * after @p timeout microseconds (if using @ref xtimer) or the @ref
 * ztimer_clock_t ticks.
 *
 * @note: the used event_timeout struct must stay valid until after the timeout
 *        event has been processed!
 *
 * @param[in]   event_timeout   event_timout context object to use
 * @param[in]   timeout         timeout in microseconds or the ztimer_clock_t
 *                              ticks units
 */
void event_timeout_set(event_timeout_t *event_timeout, uint32_t timeout);

/**
 * @brief   Clear a timeout event
 *
 * Calling this function will cancel the timeout by removing its underlying
 * timer. If the timer has already fired before calling this function, the
 * connected event will be put already into the given event queue and this
 * function does not have any effect.
 *
 * @param[in]   event_timeout   event_timeout context object to use
 */
void event_timeout_clear(event_timeout_t *event_timeout);

/**
 * @brief   Check if a timeout event is scheduled to be executed in the future
 *
 * @param[in]   event_timeout   event_timout context object to use
 * @return      true if the event is scheduled, false otherwise
 */
static inline bool event_timeout_is_pending(const event_timeout_t *event_timeout)
{
    if (event_timeout->clock == NULL || event_timeout->queue == NULL ||
        event_timeout->event == NULL) {
        return false;
    }

    return ztimer_is_set(event_timeout->clock, &event_timeout->timer)
        || event_is_queued(event_timeout->queue, event_timeout->event);
}

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* EVENT_TIMEOUT_H */
