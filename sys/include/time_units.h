/*
 * Copyright (C) 2021 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2014 Oliver Hahm <oliver.hahm@inria.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_time_units Time unit representations
 * @brief       Timestamp representation, computation, and conversion
 * @ingroup     sys
 *
 * @{
 * @file
 * @brief       Utility header providing time unit defines
 */

#ifndef TIME_UNITS_H
#define TIME_UNITS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The number of microseconds per second
 */
#define US_PER_SEC          (1000000LU)

/**
 * @brief The number of seconds per minute
 */
#define SEC_PER_MIN         (60LU)

/**
 * @brief The number of centiseconds per second
 */
#define CS_PER_SEC          (100LU)

/**
 * @brief The number of milliseconds per second
 */
#define MS_PER_SEC          (1000LU)

/**
 * @brief The number of microseconds per millisecond
 */
#define US_PER_MS           (1000LU)

/**
 * @brief The number of microseconds per centisecond
 */
#define US_PER_CS  (10000U)

/**
 * @brief The number of milliseconds per centisecond
 */
#define MS_PER_CS  (10U)

/**
 * @brief The number of nanoseconds per microsecond
 */
#define NS_PER_US           (1000LU)

/**
 * @brief The number of nanoseconds per second
 */
#define NS_PER_SEC  (1000000000LLU)

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* TIME_UNITS_H */
