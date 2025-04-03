/*
 * Copyright (C) 2018 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2018 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#ifndef ZTIMER_CONVERT_FRAC_H
#define ZTIMER_CONVERT_FRAC_H
/**
 * @defgroup  sys_ztimer_convert_frac ztimer_convert_frac frequency conversion layer
 * @ingroup   sys_ztimer_convert
 * @brief     Translates between clock tick rates
 *
 * Translates the ticks of an underlying clock into virtual ticks at a different
 * frequency.
 * This module makes use of frac for calculating fractions using multiplicative
 * inversions, avoiding integer divisions. frac trades accuracy for speed.
 * Please see the documentation of frac for more details.
 *
 * @{
 * @file
 * @brief   ztimer_convert_frac interface definitions
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 * @author  Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#include <stdint.h>
#include "ztimer.h"
#include "ztimer/convert.h"
#include "ztimer/convert_frac.h"
#include "frac.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ztimer_convert_frac frequency conversion layer class
 */
typedef struct {
    /**
     * @brief   Superclass instance
     */
    ztimer_convert_t super;
    /**
     * @brief   Frequency conversion scaling constant from lower to self
     */
    frac_t scale_now;
    /**
     * @brief   Frequency conversion scaling constant from self to lower
     */
    frac_t scale_set;
    /**
     * @brief   Rounding value, will be added to all lower set().
     *
     * E.g., 1000000/32768== ~30.5. `round` will be set to 30.
     */
    uint32_t round;
} ztimer_convert_frac_t;

/**
 * @brief   @ref ztimer_convert_frac_t constructor
 *
 * @param[in]   self        pointer to instance being initialized
 * @param[in]   lower       pointer to underlying clock
 * @param[in]   freq_self   desired frequency of this clock
 * @param[in]   freq_lower  frequency of the underlying clock
 */
void ztimer_convert_frac_init(ztimer_convert_frac_t *self,
                              ztimer_clock_t *lower,
                              uint32_t freq_self, uint32_t freq_lower);

/**
 * @brief   Change the scaling without affecting the current count
 *
 * @param[in]   self        pointer to instance being initialized
 * @param[in]   freq_self   desired frequency of this clock
 * @param[in]   freq_lower  frequency of the underlying clock
 */
void ztimer_convert_frac_change_rate(ztimer_convert_frac_t *self,
                                     uint32_t freq_self, uint32_t freq_lower);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* ZTIMER_CONVERT_FRAC_H */
