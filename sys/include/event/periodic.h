/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_event
 * @brief       Provides functionality to trigger periodic events
 *
 * event_periodic intentionally doesn't extend event structures in order to
 * support events that are integrated in larger structs intrusively.
 *
 * Example:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * event_periodic_t event_periodic;
 *
 * printf("posting timed callback every 1sec\n");
 * event_periodic_init(&event_periodic, ZTIMER_USEC, &queue, (event_t*)&event);
 * event_periodic_start(&event_periodic, 1000000);
 * [...]
 * ~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @{
 *
 * @file
 * @brief       Event Periodic API
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 */

#ifndef EVENT_PERIODIC_H
#define EVENT_PERIODIC_H

#include "event.h"
#include "ztimer/periodic.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Timeout Event structure
 */
typedef struct {
    ztimer_periodic_t timer;    /**< ztimer object used for timeout */
    event_queue_t *queue;       /**< event queue to post event to */
    event_t *event;             /**< event to post after timeout */
} event_periodic_t;

/**
 * @brief    Initialize a periodic event timeout
 *
 * @param[in]       event_periodic  event_periodic object to initialize
 * @param[in]       clock           the clock to configure this timer on
 * @param[in]       queue           queue that the timed-out event will be
 *                                  added to
 * @param[in]       event           event to add to queue after timeout
 */
void event_periodic_init(event_periodic_t *event_periodic, ztimer_clock_t *clock,
                         event_queue_t *queue, event_t *event);

/**
 * @brief   Starts a periodic timeout
 *
 * This will make the event as configured in @p event_periodic be triggered
 * at every interval ticks (based on event_periodic->clock).
 *
 * @note: the used event_periodic struct must stay valid until after the timeout
 *        event has been processed!
 *
 * @param[in]   event_periodic   event_timout context object to use
 * @param[in]   interval         period length for the event
 */
static inline void event_periodic_start(event_periodic_t *event_periodic,
                                        uint32_t interval)
{
    event_periodic->timer.interval = interval;
    ztimer_periodic_start(&event_periodic->timer);
}

/**
 * @brief   Stop a periodic timeout event
 *
 * Calling this function will cancel the timeout by removing its underlying
 * timer. If the timer has already fired before calling this function, the
 * connected event will be put already into the given event queue and this
 * function does not have any effect.
 *
 * @param[in]   event_periodic  event_periodic_timeout context object to use
 */
static inline void event_periodic_stop(event_periodic_t *event_periodic)
{
    ztimer_periodic_stop(&event_periodic->timer);
}

#ifdef __cplusplus
}
#endif
#endif /* EVENT_PERIODIC_H */
/** @} */
