/*
 * Copyright (C) 2020 Kaspar Schleiser <kaspar@schleiser.de>
 *               2020 Freie Universität Berlin
 *               2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     sys_ztimer
 * @brief       Periodic ztimer API
 *
 * Once started, the periodic timer will call the configured callback function
 * once each interval until the timer is either stopped using
 * ztimer_periodic_stop or the callback function returns a non-zero value.
 *
 * Should the timer underflow ((time_at_interrupt + interval) % 2**32 >
 * interval), the next timer will be scheduled with an offset of zero, thus
 * fire right away.  This leads to a callback for each missed tick, until the
 * original period can be honoured again.
 *
 * Example:
 *
 * ```
 * #include "ztimer/periodic.h"
 *
 * static bool callback(void *arg)
 * {
 *    puts(arg);
 *
 *    // keep the timer running
 *    return true;
 * }
 *
 *
 * int main(void)
 * {
 *     // allocate timer in .bss with static, so that timer can keep
 *     // running when leaving function scope
 *     static ztimer_periodic_t timer;
 *     // initialize timer
 *     ztimer_periodic_init(ZTIMER_SEC, &timer, callback, "test", 1);
 *
 *     // start timer
 *     ztimer_periodic_start(&timer);
 *     // timer will tick every 1 second
 *
 *     // call again to reset timer (will abort current period and trigger next
 *     // after full interval)
 *     ztimer_periodic_start(&timer);
 *
 *     // stop timer (will not trigger anymore)
 *     ztimer_periodic_stop(&timer);
 *
 *     // NOTE: if using a stack allocated timer, *it must be stopped before
 *     //       going out of scope*!
 * }
 * ```
 *
 * @{
 *
 * @file
 * @brief       ztimer periodic API
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#include <stdbool.h>
#include <stdint.h>

#include "ztimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Periodic timer stop unless it returns this value
 */
#define ZTIMER_PERIODIC_KEEP_GOING true

/**
 * @brief Type of callbacks in @ref ztimer_periodic_t "periodic timers"
 */
typedef bool (*ztimer_periodic_callback_t)(void *);

/**
 * @brief   ztimer periodic structure
 */
typedef struct {
    ztimer_t timer;                         /**< timer object used for this timer   */
    ztimer_clock_t *clock;                  /**< clock for this timer               */
    uint32_t interval;                      /**< interval of this timer             */
    ztimer_now_t last;                      /**< last trigger time                  */
    ztimer_periodic_callback_t callback;    /**< called on each trigger             */
    void *arg;                              /**< argument for callback              */
} ztimer_periodic_t;

/**
 * @brief    Initialize a periodic timer structure
 *
 * This sets up the underlying structure of a periodic timer.
 * After initializing, use @ref ztimer_periodic_start() to start the timer.
 *
 * @param[in]       clock       the clock to configure this timer on
 * @param[in,out]   timer       periodic timer object to initialize
 * @param[in]       callback    function to call on each trigger
 *                              returns `true` if the timer should keep going
 * @param[in]       arg         argument to pass to callback function
 * @param[in]       interval    period length of this timer instance
 */
void ztimer_periodic_init(ztimer_clock_t *clock, ztimer_periodic_t *timer,
                          bool (*callback)(void *),
                          void *arg, uint32_t interval);

/**
 * @brief    Start or restart a periodic timer
 *
 * When called on a newly initialized timer, the timer will start.
 *
 * When called on an already running timer, the current interval is reset to its
 * start (thus the next callback will be called after the configured interval
 * has passed).
 *
 * @param[in]   timer   periodic timer object to work on
 */
void ztimer_periodic_start(ztimer_periodic_t *timer);

/**
 * @brief    Start or restart a periodic timer without initial timer delay
 *
 * When called on a newly initialized timer, the timer will start.
 *
 * When called on an already running timer, the current interval is reset to its
 * start (thus the next callback will be called after the configured interval
 * has passed).
 *
 * @param[in]   timer   periodic timer object to work on
 */
void ztimer_periodic_start_now(ztimer_periodic_t *timer);

/**
 * @brief   Stop a periodic timer
 *
 * The periodic timer will not trigger anymore.
 *
 * @param[in]   timer   periodic timer object to stop
 */
void ztimer_periodic_stop(ztimer_periodic_t *timer);

#ifdef __cplusplus
}
#endif

/** @} */
