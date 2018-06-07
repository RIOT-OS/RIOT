/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2016 Eistec AB
 *               2018 Acutam Automation, LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   sys_xtimer
 * @{
 *
 * @file
 * @brief   xtimer hardware access layer default configuration
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 * @author  Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author  Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * @}
 */

#ifndef XTIMER_PARAMS_H
#define XTIMER_PARAMS_H

#include "board.h"
#include "xtimer.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef XTIMER_DEV
/**
 * @brief Default low-level hardware timer device
 */
#define XTIMER_DEV TIMER_DEV(0)
/**
 * @brief Default low-level hardware timer channel
 */
#define XTIMER_CHAN (0)

#if (TIMER_0_MAX_VALUE) == 0xfffffful
#define XTIMER_WIDTH (24)
#elif (TIMER_0_MAX_VALUE) == 0xffff
#define XTIMER_WIDTH (16)
#endif /* TIMER_0_MAX_VALUE */
#endif /* XTIMER_DEV */

#ifndef XTIMER_WIDTH
/**
 * @brief xtimer default timer width
 *
 * This value specifies the width (in bits) of the default hardware timer.
 * Default is 32.
 */
#define XTIMER_WIDTH (32)
#endif

#ifndef XTIMER_HZ
/**
 * @brief  Frequency of the default hardware timer
 */
#define XTIMER_HZ XTIMER_HZ_BASE
#endif

/**
 * @brief xtimer backoff value in ticks for default hardware timer
 *
 * All timers that are less than the backoff in ticks will fall back to
 * a faster timer. If there is no faster timer, xtimer will just spin.
 */
#ifndef XTIMER_BACKOFF
#define XTIMER_BACKOFF 30
#endif

#ifndef XTIMER_ISR_BACKOFF
/**
 * @brief   xtimer IRQ backoff time in ticks for default hardware timer
 *
 * When scheduling the next IRQ, if it is less than the backoff time in ticks
 * in the future, fall back to a faster timer. If there is no faster timer,
 * then just spin.
 */
#define XTIMER_ISR_BACKOFF 20
#endif

/**
 * @brief xtimer overhead value in ticks for default hardware timer
 *
 * This value specifies the time a timer will be late if uncorrected, e.g.,
 * the system-specific xtimer execution time from timer ISR to executing
 * a timer's callback's first instruction.
 *
 * E.g., with XTIMER_OVERHEAD == 0
 * start=xtimer_now();
 * xtimer_set(&timer, X);
 * (in callback:)
 * overhead=xtimer_now()-start-X;
 *
 * xtimer automatically substracts XTIMER_OVERHEAD from a timer's target time,
 * but when the timer triggers, xtimer will spin-lock until a timer's target
 * time is reached, so timers will never trigger early.
 */
#ifndef XTIMER_OVERHEAD
#define XTIMER_OVERHEAD 20
#endif

/**
 * @name    Default low-level parameter struct for xtimer hal
 * @{
 */
#ifndef XTIMER_LLPARAMS_TIMER
#define XTIMER_LLPARAMS_TIMER    { .devid = XTIMER_DEV, \
                                   .chan  = XTIMER_CHAN }
#endif

static const xtimer_llparams_timer_t xtimer_llparams_timer0
                                     = XTIMER_LLPARAMS_TIMER;
/** @} */

/**
 * @name    Default parameter struct for xtimer hal
 * @{
 */
#ifndef XTIMER_PARAMS
#define XTIMER_PARAMS    { .hz          = XTIMER_HZ ,                      \
                           .ll          = (void *)&xtimer_llparams_timer0, \
                           .width       = XTIMER_WIDTH,                    \
                           .backoff     = XTIMER_BACKOFF,                  \
                           .backoff_isr = XTIMER_ISR_BACKOFF,              \
                           .overhead    = XTIMER_OVERHEAD }
#endif

static const xtimer_params_t xtimer_params[] =
{
    XTIMER_PARAMS
};
/** @} */

/**
 * @name    Default driver list for xtimer hal
 * @{
 */
extern xtimer_driver_t xtimer_driver_timer;

static const xtimer_driver_t *xtimer_drivers[] =
{
    &xtimer_driver_timer
};
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* XTIMER_PARAMS_H */
/** @} */
