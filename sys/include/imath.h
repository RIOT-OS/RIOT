/*
 * Copyright (C) 2023 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef IMATH_H
#define IMATH_H

/**
 * @defgroup    sys_imath   imath: Integer Math functions
 * @ingroup     sys
 *
 * @brief       This modules provides some integer-only math functions.
 *              They can be used when no FPU is available or no exact
 *              precision is needed.
 *
 * @author      Karl Fessel <karl.fessel@ovgu.de>
 * @{
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SINI_PERIOD      0x8000 /**< Period of the fast_sini() function */
#define SINI_MAX         0x1000 /**< Max value of the fast_sini() function */
#define SINI_MIN        -0x1000 /**< Min value of the fast_sini() function */

/**
 * @brief       Internal fast_sini/fast_cosi helper function
 * @internal
 *
 * @param[in] x Input parameter
 *
 * @return      absolute value of cosine
 */
static inline  __attribute__((always_inline))
int32_t _ihelp(int32_t x)
{
    int32_t y;
    static const int32_t qN = 13,
                         qA = 12,
                          B = 19900,
                          C = 3516;

    x = x << (31 - qN);         /* Mask with PI                 */
    x = x >> (31 - qN);         /* Note: SIGNED shift! (to qN)  */
    x = x * x >> (2 * qN - 14); /* x=x^2 To Q14                 */

    y = B - (x * C >> 14);      /* B - x^2*C                    */
    y = (1 << qA)               /* A - x^2*(B-x^2*C)            */
      - (x * y >> 16);

    return y;
}

/**
 * @brief A sine approximation via a fourth-order cosine approx.
 *        source: https://www.coranac.com/2009/07/sines/
 *
 * @param x     angle (with 2^15 units/circle)
 * @return      sine value (-2^12 ≤ y ≤ 2^12)
 */
static inline int32_t fast_sini(int32_t x)
{
    static const int32_t qN = 13;

    int32_t c = x << (30 - qN);
    int32_t y = _ihelp(x - (1 << qN));

    return c >= 0 ? y :-y;
}

/**
 * @brief A a fourth-order cosine approx.
 *        source: https://www.coranac.com/2009/07/sines/
 *
 * @param x     angle (with 2^15 units/circle)
 * @return      sine value (-2^12 ≤ y ≤ 2^12)
 */
static inline int32_t fast_cosi(int32_t x)
{
    static const int32_t qN = 13;

    int32_t c = (x + (SINI_PERIOD >> 2)) << (30 - qN);
    int32_t y = _ihelp(x);

    return c >= 0 ? y :-y;
}

/**
 * @brief Square root of an integer
 *
 * @param x unsigned integer value
 * @return  square root of @p x
 */
static inline unsigned sqrti(unsigned x)
{
    if (x <= 1) {
        return x;
    }

    /* initial estimate */
    unsigned y0 = x >> 1;
    unsigned y1 = (y0 + x / y0) >> 1;

    while (y1 < y0) {
        y0 = y1;
        y1 = (y0 + x / y0) >> 1;
    }

    return y0;
}

/**
 * @brief Returns the value of x to the power of y
 *
 * @param x base
 * @param y exponent
 *
 * @return x^y
 */
static inline uint32_t powi(unsigned x, unsigned y)
{
    uint32_t res = 1;

    while (y--) {
        res *= x;
    }

    return res;
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* IMATH_H */
