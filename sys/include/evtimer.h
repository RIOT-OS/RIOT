/*
 * Copyright (C) 2016-17 Kaspar Schleiser <kaspar@schleiser.de>
 *               2017 Freie Universität Berlin
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
 * - uses @ref sys_xtimer "xtimer" as backend by default. Alternatively, with
 *   the pseudomodule "evtimer_on_ztimer" compiled in, evtimer is backend by
 *   @ref sys_ztimer "ZTIMER_MSEC".
 *
 * @{
 *
 * @file
 * @brief       evtimer API definitions
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 */

#ifndef EVTIMER_H
#define EVTIMER_H

#include <stdint.h>
#include "kernel_defines.h"

#if IS_USED(MODULE_EVTIMER_ON_ZTIMER)
#include "ztimer.h"
#else
#include "xtimer.h"
#endif

#include "timex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Generic event
 */
typedef struct evtimer_event {
    struct evtimer_event *next; /**< the next event in the queue */
    uint32_t offset;            /**< offset in milliseconds from previous event */
} evtimer_event_t;

/**
 * @brief   Event timer callback type
 */
typedef void(*evtimer_callback_t)(evtimer_event_t* event);

/**
 * @brief   Event timer
 */
typedef struct {
#if IS_USED(MODULE_EVTIMER_ON_ZTIMER)
    ztimer_t timer;                 /**< Timer */
    uint32_t base;                  /**< Absolute time the first event is built on */
#else
    xtimer_t timer;                 /**< Timer */
#endif
    evtimer_callback_t callback;    /**< Handler function for this evtimer's
                                         event type */
    evtimer_event_t *events;        /**< Event queue */
} evtimer_t;

/**
 * @brief   Initializes an event timer
 *
 * @warning BEWARE! Callbacks from evtimer_init() are being executed
 * in interrupt context.
 * DON'T USE THIS FUNCTION unless you know *exactly* what that means.
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

/**
 * @brief   Return the current system time in msec
 */
static inline uint32_t evtimer_now_msec(void)
{
#if IS_USED(MODULE_EVTIMER_ON_ZTIMER)
    return ztimer_now(ZTIMER_MSEC);
#else
    return xtimer_now_usec64() / US_PER_MS;
#endif
}

/**
 * @brief   Return the current system time in minutes
 *
 * @deprecated wrongly placed convenience function, that
 *             may not work as expected with ztimer (32 bit)
 */
static inline uint32_t evtimer_now_min(void)
{
#if IS_USED(MODULE_EVTIMER_ON_ZTIMER)
    return ztimer_now(ZTIMER_MSEC) / (MS_PER_SEC * SEC_PER_MIN);
#else
    return xtimer_now_usec64() / (US_PER_SEC * SEC_PER_MIN);
#endif
}

#ifdef __cplusplus
}
#endif

#endif /* EVTIMER_H */
/** @} */
