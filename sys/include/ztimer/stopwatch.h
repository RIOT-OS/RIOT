/*
 * Copyright (C) 2023 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    sys_ztimer_stopwatch ztimer stop watch
 * @ingroup     sys_ztimer
 * @brief       Measure time with ztimer
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @{
 */

#include "ztimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ztimer stop watch struct
 */
typedef struct {
    ztimer_clock_t *clock;  /**< the clock to use     */
    uint32_t start_time;    /**< start of measurement */
} ztimer_stopwatch_t;

/**
 * @brief   Initialize a ztimer stop watch
 *          The stop watch is not running yet.
 *
 * @param[in]  clock    The clock to use for the stopwatch
 * @param[out] timer    The stop watch clock to initialize
 */
static inline void ztimer_stopwatch_init(ztimer_clock_t *clock, ztimer_stopwatch_t *timer)
{
    timer->clock = clock;
}

/**
 * @brief   Start the stop watch timer
 *
 * @param[in] timer     The stop watch timer to start
 */
static inline void ztimer_stopwatch_start(ztimer_stopwatch_t *timer)
{
    ztimer_acquire(timer->clock);
    timer->start_time = ztimer_now(timer->clock);
}

/**
 * @brief   Take a measurement from the stop watch timer
 *
 * @param[in] timer     The stop watch timer to take a measurement of
 *
 * @return  Timer ticks since the stop watch was started / reset
 */
static inline uint32_t ztimer_stopwatch_measure(ztimer_stopwatch_t *timer)
{
    return ztimer_now(timer->clock) - timer->start_time;
}

/**
 * @brief   Reset the stop watch start time.
 *          The Stop Watch will start counting from 0 again.
 *
 * @param[in] timer     The stop watch timer to reset
 *
 * @return Timer ticks since the last reset / start of the watch
 */
static inline uint32_t ztimer_stopwatch_reset(ztimer_stopwatch_t *timer)
{
    uint32_t now = ztimer_now(timer->clock);
    uint32_t diff = now - timer->start_time;

    timer->start_time = now;
    return diff;
}

/**
 * @brief   Stop the stop watch.
 *          The stop watch will no longer run.
 *
 * @param[in] timer     The stop watch timer to stop
 */
static inline void ztimer_stopwatch_stop(ztimer_stopwatch_t *timer)
{
    ztimer_release(timer->clock);
}

#ifdef __cplusplus
}
#endif

/** @} */
