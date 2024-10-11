/*
 * Copyright (C) 2018 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

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

#ifndef ZTIMER_PERIPH_TIMER_H
#define ZTIMER_PERIPH_TIMER_H

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
 */
void ztimer_periph_timer_init(ztimer_periph_timer_t *clock, tim_t dev,
                              uint32_t freq, uint32_t max_val);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* ZTIMER_PERIPH_TIMER_H */
