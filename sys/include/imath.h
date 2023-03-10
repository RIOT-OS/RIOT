/*
 * Copyright (C) 2023 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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

#ifndef IMATH_H
#define IMATH_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ISIN_PERIOD      0x8000 /**< Period of the isin() function */
#define ISIN_MAX         0x1000 /**< Max value of the isin() function */
#define ISIN_MIN        -0x1000 /**< Min value of the isin() function */

/**
 * @brief       Internal isin/icos helper function
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
static inline int32_t isin(int32_t x)
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
static inline int32_t icos(int32_t x)
{
    static const int32_t qN = 13;

    int32_t c = (x + (ISIN_PERIOD >> 2)) << (30 - qN);
    int32_t y = _ihelp(x);

    return c >= 0 ? y :-y;
}

#ifdef __cplusplus
}
#endif

#endif /* IMATH_H */
/** @} */
