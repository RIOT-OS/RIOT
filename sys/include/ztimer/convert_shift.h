/*
 * Copyright (C) 2018 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup  sys_ztimer_convert_shift conversion using shifts
 * @ingroup   sys_ztimer_convert
 * @brief     Translates between clock tick rates
 *
 * Translates the ticks of an underlying clock into virtual ticks at a different
 * frequency, by using bit shifts. Thus it works only for fractions that are
 * powers of 2.
 *
 * @{
 * @file
 * @brief   ztimer_convert_shift interface definitions
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 */
#ifndef ZTIMER_CONVERT_SHIFT_H
#define ZTIMER_CONVERT_SHIFT_H

#include <stdint.h>

#include "ztimer.h"
#include "ztimer/convert.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ztimer_convert_shift frequency conversion layer class
 */
typedef struct {
    /**
     * @brief   Superclass instance
     */
    ztimer_convert_t super;
    /**
     * @brief   Frequency conversion scaling constant from lower to self
     *
     * This value is saved as positive integer. The functions within the "ops"
     * struct decides whether to left or right shift.
     */
    unsigned shift;
} ztimer_convert_shift_t;

/**
 * @brief   ztimer_convert_shift init() for (fake) increasing timer frequency
 *
 * Will cause every lower now() to be left-shifted and every set() to be
 * right-shifted.
 *
 * @param[in]   clock       pointer to instance being initialized
 * @param[in]   lower       pointer to underlying clock
 * @param[in]   shift       shift value to use
 */
void ztimer_convert_shift_up_init(ztimer_convert_shift_t *clock,
                                  ztimer_clock_t *lower, unsigned shift);

#ifdef __cplusplus
}
#endif

#endif /* ZTIMER_CONVERT_SHIFT_H */
/** @} */
