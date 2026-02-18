/*
 * SPDX-FileCopyrightText: 2021 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-FileCopyrightText: 2014 Oliver Hahm <oliver.hahm@inria.fr>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    sys_time_units Time unit representations
 * @brief       Timestamp representation, computation, and conversion
 * @ingroup     sys
 *
 * @{
 * @file
 * @brief       Utility header providing time unit defines
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The number of hours per day
 */
#define HOURS_PER_DAY       (24LU)

/**
 * @brief The number of minutes per day
 */
#define MIN_PER_DAY         (1440LU)

/**
 * @brief The number of seconds per day
 */
#define SEC_PER_DAY         (86400LU)

/**
 * @brief The number of minutes per hour
 */
#define MIN_PER_HOUR        (60LU)

/**
 * @brief The number of seconds per hour
 */
#define SEC_PER_HOUR        (3600LU)

/**
 * @brief The number of milliseconds per hour
 */
#define MS_PER_HOUR        (3600000LU)

/**
 * @brief The number of microseconds per hour
 */
#define US_PER_HOUR        (3600000000LU)

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
 * @brief The number of milliseconds per centisecond
 */
#define MS_PER_CS           (10U)

/**
 * @brief The number of microseconds per second
 */
#define US_PER_SEC          (1000000LU)

/**
 * @brief The number of microseconds per millisecond
 */
#define US_PER_MS           (1000LU)

/**
 * @brief The number of microseconds per centisecond
 */
#define US_PER_CS           (10000U)

/**
 * @brief The number of nanoseconds per microsecond
 */
#define NS_PER_US           (1000LU)

/**
 * @brief The number of nanoseconds per millisecond
 */
#define NS_PER_MS           (1000000LU)

/**
 * @brief The number of nanoseconds per second
 */
#define NS_PER_SEC          (1000000000LLU)

#ifdef __cplusplus
}
#endif

/** @} */
