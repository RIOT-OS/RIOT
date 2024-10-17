/*
 * Copyright (C) 2023 Otto-von-Guericke-Universität Magdeburg
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
 * @brief       Various helper macros
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef MACROS_UTILS_H
#define MACROS_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Concatenate the tokens @p a and @p b
 */
#define CONCAT(a, b) a ## b

/**
 * @brief   Concatenate the tokens @p a , @p b , and @p c
 */
#define CONCAT3(a, b, c) a ## b ## c

/**
 * @brief   Concatenate the tokens @p a , @p b , @p c , and @p d
 */
#define CONCAT4(a, b, c, d) a ## b ## c ## d

/* For compatibility with vendor headers, only provide MAX() and MIN() if not
 * provided. (The alternative approach of using #undef has the downside that
 * vendor header files may provide a smarter version of MAX() / MIN() that does
 * not evaluate the argument twice and rely on this).
 */
#ifndef MAX
/**
 * @brief   Get the maximum of the two parameters
 *
 * @note    This is the trivial implementation that does evaluate the arguments
 *          more than once
 */
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
/**
 * @brief   Get the minimum of the two parameters
 *
 * @note    This is the trivial implementation that does evaluate the arguments
 *          more than once
 */
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef ABS
/**
 * @brief Returns the absolute value of @p x
 *
 * @note    This is the trivial implementation that does evaluate the arguments
 *          more than once
 */
#define ABS(x) ((x) > 0 ? (x) : -(x))
#endif

/**
 * @brief   Limit a value to an inclusive range
 *
 * If @p val is > @p high, @p high is returned. If @p val is < @p low, @p low
 * is returned. Otherwise, @p val is returned.
 *
 * @note    This macro evaluate its arguments more than once.
 *
 * @param[in] val       value to limit
 * @param[in] low       minimum limit
 * @param[in] high      maximum limit
 *
 * @return range limited value
 */
#define LIMIT(val, low, high) ((val < low) ? low : (val > high) ? high : val)

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* MACROS_UTILS_H */
