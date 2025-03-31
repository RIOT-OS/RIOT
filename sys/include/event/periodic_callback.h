/*
 * Copyright (C) 2022 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef EVENT_PERIODIC_CALLBACK_H
#define EVENT_PERIODIC_CALLBACK_H

/**
 * @ingroup     sys_event
 * @brief       Provides functionality to trigger periodic event callbacks
 *
 * This provides convenience functions to trigger periodic event callbacks
 * executed by the event thread.
 *
 * @{
 *
 * @file
 * @brief       Event Periodic Callback API
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 */

#include <assert.h>
#include "event/callback.h"
#include "event/periodic.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Periodic Callback Event structure
 */
typedef struct {
    event_periodic_t periodic;  /**< periodic event portion */
    event_callback_t event;     /**< callback event portion */
} event_periodic_callback_t;

/**
 * @brief   Get user context from Periodic Callback Event
 *
 * @param[in]   event           event_periodic_callback object to initialize
 * @return      User supplied argument to the event
 */
static inline void *event_periodic_callback_get_arg(event_periodic_callback_t *event)
{
    return event->event.arg;
}

/**
 * @brief   Initialize a periodic callback event
 *
 * @note: On init the periodic event is configured to run forever.
 *
 * @param[in]   event           event_periodic_callback object to initialize
 * @param[in]   clock           the clock to configure this timer on
 * @param[in]   queue           queue that the timed-out event will be added to
 * @param[in]   callback        callback to set up
 * @param[in]   arg             callback argument to set up
 */
static inline void event_periodic_callback_init(event_periodic_callback_t *event,
                                                ztimer_clock_t *clock, event_queue_t *queue,
                                                void (*callback)(void *), void *arg)
{
    memset(&event->event, 0, sizeof(event->event));
    event->event.super.handler = _event_callback_handler;
    event->event.callback = callback;
    event->event.arg = arg;

    event_periodic_init(&event->periodic, clock, queue, &event->event.super);
}

/**
 * @brief   Starts a periodic callback event
 *
 * If the event is already started, it's interval will be updated and it
 * will be scheduled with the new interval.
 *
 * This will make the event as configured in @p event be triggered
 * at every interval ticks (based on event->periodic.clock).
 *
 * @note: the used event_periodic struct must stay valid until after the timeout
 *        event has been processed!
 *
 * @note: this function does not touch the current count value.
 *
 * @param[in]   event           event_periodic_callback context object to use
 * @param[in]   interval        period length for the event
 */
static inline void event_periodic_callback_start(event_periodic_callback_t *event,
                                                 uint32_t interval)
{
    assert(event->event.callback);
    event_periodic_start(&event->periodic, interval);
}

/**
 * @brief   Initialize and start a periodic callback event
 *
 * This is a convenience function that combines @ref event_periodic_callback_init
 * and @ref event_periodic_callback_start
 *
 * @param[out]  event           event_periodic_callback object to initialize
 * @param[in]   clock           the clock to configure this timer on
 * @param[in]   interval        period length for the event
 * @param[in]   queue           queue that the timed-out event will be added to
 * @param[in]   callback        callback to set up
 * @param[in]   arg             callback argument to set up
 */
static inline void event_periodic_callback_create(event_periodic_callback_t *event,
                                                  ztimer_clock_t *clock, uint32_t interval,
                                                  event_queue_t *queue,
                                                  void (*callback)(void *), void *arg)
{
    event_periodic_callback_init(event, clock, queue, callback, arg);
    event_periodic_callback_start(event, interval);
}

/**
 * @brief   Get the interval in which the periodic callback event repeats
 *
 * @param[in]   event           event_periodic_callback context object to use
 *
 * @returns     The interval of the underlying timer in which the event repeats
 */
static inline uint32_t event_periodic_callback_get_interval(const event_periodic_callback_t *event)
{
    return event->periodic.timer.interval;
}

/**
 * @brief   Set the amount of times the periodic callback event should repeat itself.
 *
 * @param[in]   event           event_periodic_callback context object to use
 * @param[in]   count           times the event should repeat itself,
 *                              EVENT_PERIODIC_FOREVER to run for ever.
 */
static inline void event_periodic_callback_set_count(event_periodic_callback_t *event,
                                                     uint32_t count)
{
    event_periodic_set_count(&event->periodic, count);
}

/**
 * @brief   Get the amount of times the periodic callback event should repeat itself.
 *
 * @param[in]   event           event_periodic_callback context object to use
 * @returns     times the event should repeat itself,
 *              EVENT_PERIODIC_FOREVER if it will repeat forever or is exhausted
 */
static inline uint32_t event_periodic_callback_get_count(const event_periodic_callback_t *event)
{
    return event->periodic.count;
}

/**
 * @brief   Stop a periodic callback event
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
 * @param[in]   event           event_periodic_callback context object to use
 */
static inline void event_periodic_callback_stop(event_periodic_callback_t *event)
{
    event_periodic_stop(&event->periodic);
}

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* EVENT_PERIODIC_CALLBACK_H */
