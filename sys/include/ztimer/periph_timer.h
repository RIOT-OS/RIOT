/*
 * SPDX-FileCopyrightText: 2018 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    sys_ztimer_periph_timer  ztimer periph/timer backend
 * @ingroup     sys_ztimer
 * @brief       ztimer periph/timer backend
 *
 * This ztimer module implements a ztimer virtual clock on top of periph/timer.
 *
 * This module has two tuning values:
 * "adjust": will be subtracted from every timer set.
 * "min": Every timer will be set to max("min", value).
 * @{
 *
 * @file
 * @brief       ztimer periph/timer backend API
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#include "ztimer.h"
#include "periph/timer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ztimer periph context structure
 */
typedef struct {
    ztimer_clock_t super;   /**< super class            */
    tim_t dev;              /**< periph timer device    */
    uint16_t min;           /**< optional minimum value */
} ztimer_periph_timer_t;

/**
 * @brief   ztimer periph initialization
 *
 * Initializes the given periph timer and sets up the ztimer device.
 *
 * @param[in]   clock   ztimer periph device to initialize
 * @param[in]   dev     periph timer to use
 * @param[in]   freq    frequency to configure
 * @param[in]   max_val maximum value this timer supports
 *
 * @return  The actual frequency the timer has been configured to
 */
uint32_t ztimer_periph_timer_init(ztimer_periph_timer_t *clock, tim_t dev,
                                  uint32_t freq, uint32_t max_val);

#ifdef __cplusplus
}
#endif

/** @} */
