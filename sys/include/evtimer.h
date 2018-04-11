/*
 * Copyright (C) 2016-17 Kaspar Schleiser <kaspar@schleiser.de>
 *               2017 Freie Universität Berlin
 *               2018 Josua Arndt
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_evtimer Millisecond interval event timers
 * @ingroup     sys
 * @brief       Provides timers for events up to @$2^{32}@$ milliseconds in the
 *              future
 *
 * @note    Experimental and likely to replaced with unified timer API
 *
 * RIOT's main timer subsystem is @ref sys_xtimer "xtimer", but for many
 * applications @ref sys_xtimer "xtimer's" 64-bit absolute time values are
 * wasteful or clumsy to use.
 *
 * Compared to @ref sys_xtimer "xtimer", evtimer offers:
 *
 * - only relative 32-bit millisecond timer values
 *   Events can be scheduled with a relative offset of up to ~49.7 days in the
 *   future.
 *   **For time-critical stuff, use @ref sys_xtimer "xtimer"!**
 * - more flexible, "intrusive" timer type @ref evtimer_event_t only contains
 *   the necessary fields, which can be extended as needed, and handlers define
 *   actions taken on timer triggers. Check out @ref evtimer_msg_event_t as
 *   example.
 * - uses @ref sys_xtimer "xtimer" as backend
 *
 * @{
 *
 * @file
 * @brief       evtimer API definitions
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 */

#ifndef EVTIMER_H
#define EVTIMER_H

#include <stdint.h>

#include "xtimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Generic event
 */
typedef struct evtimer_event {
    struct evtimer_event *next; /**< the next event in the queue */
    uint32_t offset_ms;            /**< offset in milliseconds from previous event */
} evtimer_event_t;

/**
 * @brief   Event timer callback type
 */
typedef void(*evtimer_callback_t)(evtimer_event_t* event);

/**
 * @brief   Event timer
 */
typedef struct {
    xtimer_t timer;                 /**< Timer */
    evtimer_callback_t callback;    /**< Handler function for this evtimer's
                                         event type */
    evtimer_event_t *events;        /**< Event queue */
} evtimer_t;

/**
 * @brief   Initializes an event timer
 *
 * @param[in] evtimer   An event timer
 * @param[in] handler   An event handler function
 */
void evtimer_init(evtimer_t *evtimer, evtimer_callback_t handler);

/**
 * @brief   Adds event to an event timer
 *
 * @param[in] evtimer       An event timer
 * @param[in] event         An event
 */
void evtimer_add(evtimer_t *evtimer, evtimer_event_t *event);

/**
 * @brief   Removes an event from an event timer
 *
 * @param[in] evtimer       An event timer
 * @param[in] event         An event
 */
void evtimer_del(evtimer_t *evtimer, evtimer_event_t *event);

/**
 * @brief   Print overview of current state of an event timer
 *
 * @param[in] evtimer   An event timer
 */
void evtimer_print(const evtimer_t *evtimer);

#ifdef __cplusplus
}
#endif

#endif /* EVTIMER_H */
/** @} */
