/*
 * SPDX-FileCopyrightText: 2022 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     core_macros
 * @{
 *
 * @file
 * @brief       Math helper macros
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Returns the sign of @p a, either -1 or 1
 */
#define SIGNOF(a) _Generic(a, unsigned char: 1,          \
                              unsigned short: 1,         \
                              unsigned int: 1,           \
                              unsigned long: 1,          \
                              unsigned long long: 1,     \
                              default: ((a) <= 0 ? ((a) == 0 ? 1L : -1L ): 1L))
/**
 * @brief Calculates @p a/ @p b with arithmetic rounding (.5 away from zero)
 */
#define DIV_ROUND(a, b)  ((SIGNOF(a) * ((SIGNOF(a) * (a)) + (SIGNOF(b) * (b)) / 2) / (b)))

/**
 * @brief Calculates @p a/ @p b, always rounding up (towards positive infinity)
 */
#define DIV_ROUND_UP(a, b) ((SIGNOF(a)>0 && SIGNOF(b)>0) ? (((a) + (b) - 1) / (b)):\
                            (SIGNOF(a)<0 && SIGNOF(b)<0) ? (((a) + (b) + 1) / (b)): (a) / (b))

/**
 * @brief Calculates @p a/ @p b, always rounding away from zero (towards positive and negative inf)
 */
#define DIV_ROUND_INF(a, b) (SIGNOF(a) * (((SIGNOF(a) * (a)) + (SIGNOF(b) * (b)) - 1) / (b)))

/**
 * @brief Align @p num with the next multiple of @p chunk
 */
#define MATH_ALIGN(num, chunk) ((chunk) * DIV_ROUND_INF(num, chunk))

#ifdef __cplusplus
}
#endif

/** @} */
