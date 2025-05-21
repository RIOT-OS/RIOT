/*
 * Copyright (C) 2018 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup  sys_frac   Fractional integer operations
 * @ingroup   sys
 *
 * This header provides some functions for scaling integers by fractions, while
 * preserving as many bits as possible.
 *
 * The implementation requires that @ref frac_t is initialized properly, either
 * by calling @ref frac_init, which will compute the algorithm parameters at
 * runtime, or via a precomputed initializer.
 *
 * Precomputing the frac_t values can be done via the application found in
 * `tests/sys/frac-config` in the RIOT tree.
 *
 * ### Numeric precision
 *
 * The algorithm will under certain circumstances give an incorrectly rounded
 * result, more precisely, the result may sometimes be rounded up instead of
 * rounded down when the product in the numerator, @$p = x * num@$, would
 * result in @$p >= 2^{31}@$. Fortunately, the relative error of this rounding
 * mistake is small.
 *
 * This tradeoff is a design choice to make the algorithm faster.
 *
 * @see       Libdivide homepage: http://libdivide.com/
 *
 * @file
 * @ingroup   sys
 * @author    Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @{
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   frac descriptor for fraction consisting of two 32 bit integers
 */
typedef struct {
    uint32_t frac; /**< fraction */
    uint8_t shift; /**< exponent */
} frac_t;

/**
 * @brief   Compute greatest common divisor of @p u and @p v
 *
 * @param[in]   u    first operand
 * @param[in]   v    second operand
 *
 * @return  Greatest common divisor of @p u and @p v
 */
uint32_t gcd32(uint32_t u, uint32_t v);

/**
 * @brief   Initialize frac_t struct
 *
 * This function computes the mathematical parameters used by the frac algorithm.
 *
 * @note If @p num > @p den, the result from @ref frac_scale modulo 2**32.
 *
 * @pre @p den must not be 0
 *
 * @param[out]  frac    pointer to frac descriptor to initialize
 * @param[in]   num     numerator
 * @param[in]   den     denominator
 */
void frac_init(frac_t *frac, uint32_t num, uint32_t den);

/**
 * @brief   Scale a 32 bit integer by a 32/32 rational number
 *
 * @param[in]   frac  scaling fraction
 * @param[in]   x     unscaled integer
 *
 * @return      (x * frac) % 2**32, avoiding truncation
 */
static inline uint32_t frac_scale(const frac_t *frac, uint32_t x)
{
    uint32_t scaled = ((uint64_t)frac->frac * x) >> frac->shift;
    return scaled;
}

#ifdef __cplusplus
}
#endif
/** @} */
