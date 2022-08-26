/*
 * Copyright (C) 2017 Inria
 *               2017 Freie Universität Berlin
 *               2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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

#ifndef EVENT_TIMEOUT_H
#define EVENT_TIMEOUT_H

#include "event.h"
#include "ztimer.h"

#ifdef __cplusplus
extern "C" {
#endif

#if IS_USED(MODULE_EVENT_TIMEOUT_ZTIMER) || defined(DOXYGEN)
typedef ztimer_clock_t *event_timeout_scale_t;  /**< Timeout scale */

#define EVENT_TIMEOUT_SCALE_USEC    ZTIMER_USEC /**< interpret timeout delay in microseconds */
#define EVENT_TIMEOUT_SCALE_MSEC    ZTIMER_MSEC /**< interpret timeout delay in milliseconds */
#define EVENT_TIMEOUT_SCALE_SEC     ZTIMER_SEC  /**< interpret timeout delay in seconds */
#else
#error "No timer backend provided"
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
 * @param[in]   scale           scale, i.e. time-unit to use with the timeout.
 *                              See @ref event_timeout_scale_t.
 * @param[in]   queue           queue that the timed-out event will be added to
 * @param[in]   event           event to add to queue after timeout
 */
void event_timeout_scale_init(event_timeout_t *event_timeout, event_timeout_scale_t scale,
                              event_queue_t *queue, event_t *event);

/**
 * @brief   Initialize timeout event object
 *
 * @deprecated  This function will be deprecated after the 2023.01 release. Use
 *              @ref event_timeout_scale_init() instead.
 *
 * @param[in]   event_timeout   event_timeout object to initialize
 * @param[in]   clock           the clock backend, eg: ZTIMER_USEC, ZTIMER_MSEC
 * @param[in]   queue           queue that the timed-out event will be added to
 * @param[in]   event           event to add to queue after timeout
 */
static inline void event_timeout_ztimer_init(event_timeout_t *event_timeout, ztimer_clock_t *clock,
                                             event_queue_t *queue, event_t *event)
{
    event_timeout_scale_init(event_timeout, (event_timeout_scale_t)clock, queue, event);
}

#if IS_USED(MODULE_EVENT_TIMEOUT) || DOXYGEN
/**
 * @brief   Initialize timeout event object
 *
 * @deprecated  This function will be deprecated after the 2023.01 release. Use
 *              @ref event_timeout_scale_init() instead. After the 2023.01 event_timeout_init()
 *              will become an alias for event_timeout_scale_init().
 *
 * @note    EVENT_TIMEOUT_SCALE_USEC is used as timeout scale.
 *
 * @param[in]   event_timeout   event_timeout object to initialize
 * @param[in]   queue           queue that the timed-out event will be added to
 * @param[in]   event           event to add to queue after timeout
 */
static inline void event_timeout_init(event_timeout_t *event_timeout, event_queue_t *queue,
                                      event_t *event)
{
    event_timeout_scale_init(event_timeout, EVENT_TIMEOUT_SCALE_USEC, queue, event);
}
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

#ifdef __cplusplus
}
#endif
#endif /* EVENT_TIMEOUT_H */
/** @} */
