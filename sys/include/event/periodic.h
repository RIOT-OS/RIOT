/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

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


#include "event.h"
#include "ztimer/periodic.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Run the periodic event forever
 */
#define EVENT_PERIODIC_FOREVER      0

/**
 * @brief   Timeout Event structure
 */
typedef struct {
    ztimer_periodic_t timer;    /**< ztimer object used for timeout */
    event_queue_t *queue;       /**< event queue to post event to */
    event_t *event;             /**< event to post after timeout */
    uint32_t count;             /**< times the event should repeat itself */
} event_periodic_t;

/**
 * @brief   Initialize a periodic event timeout
 *
 * @note: On init the periodic event is to to run forever.
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
 * @brief   Starts a periodic timeout without delay for the first occurrence
 *
 * This will make the event as configured in @p event_periodic be triggered
 * at every interval ticks (based on event_periodic->clock).
 *
 * @note: the used event_periodic struct must stay valid until after the timeout
 *        event has been processed!
 *
 * @note: this function does not touch the current count value.
 *
 * @note: the periodic event will start without delay.
 *
 * @param[in]   event_periodic   event_timout context object to use
 * @param[in]   interval         period length for the event
 */
static inline void event_periodic_start_now(event_periodic_t *event_periodic, uint32_t interval)
{
    event_periodic->timer.interval = interval;
    ztimer_periodic_start_now(&event_periodic->timer);
}

/**
 * @brief   Starts a periodic timeout
 *
 * This will make the event as configured in @p event_periodic be triggered
 * at every interval ticks (based on event_periodic->clock).
 *
 * @note: the used event_periodic struct must stay valid until after the timeout
 *        event has been processed!
 *
 * @note: this function does not touch the current count value.
 *
 * @param[in]   event_periodic   event_timout context object to use
 * @param[in]   interval         period length for the event
 */
static inline void event_periodic_start(event_periodic_t *event_periodic, uint32_t interval)
{
    event_periodic->timer.interval = interval;
    ztimer_periodic_start(&event_periodic->timer);
}

/**
 * @brief   Set the amount of times the periodic event should repeat itself.
 *
 * @param[in]   event_periodic   event_timout context object to use
 * @param[in]   count            times the event should repeat itself,
 *                               EVENT_PERIODIC_FOREVER to run for ever.
 */
static inline void event_periodic_set_count(event_periodic_t *event_periodic, uint32_t count)
{
    unsigned state = irq_disable();

    event_periodic->count = count;
    irq_restore(state);
}

/**
 * @brief   Stop a periodic timeout event
 *
 * Calling this function will cancel the timeout by removing its underlying
 * timer. If the timer has already fired before calling this function, the
 * connected event will be put already into the given event queue and this
 * function does not have any effect.
 *
 * @note Calling this function does not touch event_periodic->count, if the
 * periodic event was not set to run for ever and did run until expiration,
 * then count will be != 0 after this function is called.
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
/** @} */
