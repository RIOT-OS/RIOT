/*
 * SPDX-FileCopyrightText: 2019 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    sys_ztimer_convert ztimer frequency conversion modules
 * @ingroup     sys_ztimer
 * @brief       ztimer frequency conversion modules
 *
 * ztimer provides multiple conversion modules:
 *
 * - ztimer_convert_shift: should be used if the fraction is a power of two.
 *
 * - ztimer_convert_frac: should be used if the fraction is not a power of two.
 *                        rounding might be a bit off for some fractions.
 *
 * - ztimer_convert_muldiv64: can be used instead of ztimer_convert_frac,
 *                            if 64bit division is cheap on the target board.
 *
 * @{
 * @file
 * @brief       ztimer frequency conversion base module
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#include "ztimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief base type for ztimer convert modules
 *
 * This type is supposed to be extended. It provides common fields for a ztimer
 * clock that has a parent clock.
 * */
typedef struct {
    ztimer_clock_t super;       /**< ztimer_clock super class       */
    ztimer_clock_t *lower;      /**< lower clock device             */
    ztimer_t lower_entry;       /**< timer entry in parent clock     */
} ztimer_convert_t;

/**
 * @brief    Initialization function for ztimer_convert_t
 *
 * @p max_value needs to be set to the maximum value that can be converted
 * without overflowing. E.g., if the conversion module slows down a lower
 * clock by factor X, max_value needs to be set to UINT32_MAX / X.
 *
 * @param[in,out]   ztimer_convert  object to initialize
 * @param[in]       lower           lower ztimer clock
 * @param[in]       max_value       maximum value for this clock's set()
 */
void ztimer_convert_init(ztimer_convert_t *ztimer_convert,
                         ztimer_clock_t *lower, uint32_t max_value);

/**
 * @brief   ztimer_convert common cancel() op
 *
 * Used by some conversion modules as ztimer_clock_t::ops.cancel().
 *
 * @param[in]       clock   ztimer clock to operate on
 */
void ztimer_convert_cancel(ztimer_clock_t *clock);

/**
 * @brief   ztimer_convert common start() op
 *
 * Used by some conversion modules as ztimer_clock_t::ops.start().
 *
 * @param[in]       clock   ztimer clock to operate on
 */
void ztimer_convert_start(ztimer_clock_t *clock);

/**
 * @brief   ztimer_convert common stop() op
 *
 * Used by some conversion modules as ztimer_clock_t::ops.stop().
 *
 * @param[in]       clock   ztimer clock to operate on
 */
void ztimer_convert_stop(ztimer_clock_t *clock);

#ifdef __cplusplus
}
#endif

/** @} */
