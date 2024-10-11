/*
 * Copyright (C) 2019 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    sys_ztimer_convert_muldiv64 plain 64bit carithmetic
 * @ingroup     sys_ztimer_convert
 * @brief       ztimer frequency conversion module (64bit arithmetic)
 *
 * This ztimer module allows converting a lower-level ztimer clockwith a given
 * frequency to another frequency.
 *
 * It is configured by passing two parameters (div, mul).
 * Given a lower clock frequency f_low and a desired upper frequency f_upper,
 * div and mul must be chosen such that
 *
 *     (f_upper * mul / div) == f_lower
 *
 * A div or mul value of 0 is treated as 1 (no multiplication or division by 0 is
 * done).
 *
 * On every ztimer_set(), the target offset is first multiplied by mul and
 * then divided by div, before passing it to the lower ztimer's ztimer_set().
 *
 * On every ztimer_now(), the value from the lower ztimer is first multiplied
 * by div and then divided by mul.
 *
 * Multiplication and division is done using 64bit multiplication / division,
 * thus its use should be avoided in favour of more optimized conversion
 * modules.
 *
 * Example:
 *
 * 1. if a ztimer_periph_timer with 250kHz is to be "sped up" to 1MHz,
 *    use div=4, mul=0
 *
 * 2. if a ztimer with 1024Hz is to be converted to 1000Hz, use div=125, mul=128
 *
 * @{
 * @file
 * @brief       ztimer frequency conversion module API
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef ZTIMER_CONVERT_MULDIV64_H
#define ZTIMER_CONVERT_MULDIV64_H

#include "ztimer.h"
#include "ztimer/convert.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ztimer_convert_muldiv64 structure
 */
typedef struct {
    ztimer_convert_t super;         /**< super class    */
    uint16_t mul;                   /**< please read    */
    uint16_t div;                   /**< module docs    */
} ztimer_convert_muldiv64_t;

/**
 * @brief   ztimer_convert_muldiv64 initialization function
 *
 * @param[in]  ztimer_convert_muldiv64     instance to initialize
 * @param[in]  lower                       lower timer to convert
 * @param[in]  div                         see module doc
 * @param[in]  mul                         see module doc
 */
void ztimer_convert_muldiv64_init(
    ztimer_convert_muldiv64_t *ztimer_convert_muldiv64, ztimer_clock_t *lower,
    unsigned div, unsigned mul);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* ZTIMER_CONVERT_MULDIV64_H */
