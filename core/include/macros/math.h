/*
 * Copyright (C) 2022 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_macros
 * @{
 *
 * @file
 * @brief       Math helper macros
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#ifndef MACROS_MATH_H
#define MACROS_MATH_H

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
                              default: (long long)(a) < 0 ? -1 : 1)
/**
 * @brief Calculates @p a/ @p b with arithmetic rounding
 */
#define DIV_ROUND(a, b) (((a) + SIGNOF(a) * (b) / 2) / (b))

/**
 * @brief Calculates @p a/ @p b, always rounding up to the
 *        next whole number
 */
#define DIV_ROUND_UP(a, b) (((a) + SIGNOF(a) * ((b) - SIGNOF(b))) / (b))

/**
 * @brief Align @p num with the next multiple of @p chunk
 */
#define MATH_ALIGN(num, chunk) ((chunk) * DIV_ROUND_UP(num, chunk))

#ifdef __cplusplus
}
#endif

#endif /* MACROS_MATH_H */
/** @} */
