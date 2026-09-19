/*
 * Copyright (C) 2025 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    sys_time_q48        48.16 Unix timestamps
 * @ingroup     sys
 *
 * @{
 *
 * @file
 * @brief       Q48.16 Unix time
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#ifndef TIME_Q48_H
#define TIME_Q48_H

#include <stdint.h>

#include "time_units.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Epoch Q48.16 time format
 *
 * The upper 48 bits of the counter are the seconds since 1st January 1970
 * The lower 16 bits are the fractions of the second with 1/64k prec
 */
typedef int64_t time_q48_t;

/**
 * @brief Get seconds from Q48.16 time format
 *
 * @param[in] time  timestamp in Q48.16 format
 *
 * @return Second part of the timestamp
 */
static inline int64_t time_q48_get_sec(time_q48_t time)
{
    return time >> 16;
}

/**
 * @brief Get microseconds from Q48.16 time format
 *
 * @param[in] time  timestamp in Q48.16 format
 *
 * @return Microsecond part of the timestamp
 */
static inline uint32_t time_q48_get_usec(time_q48_t time)
{
    return ((uint64_t)US_PER_SEC * (time & 0xFFFF)) >> 16;
}

/**
 * @brief Convert time value to Q48.16 time format
 *
 * @param[in] secs  Seconds since 01.01.1970
 * @param[in] us    Microseconds of the current second
 *
 * @return Timestamp in Q48.16 format
 */
static inline time_q48_t time_to_q48(int64_t secs, uint32_t us)
{
    int64_t now = secs << 16;
    now += ((uint64_t)us * 0xFFFF) / US_PER_SEC;

    return now;
}

#ifdef __cplusplus
}
#endif

#endif /* TIME_Q48_H */
/** @} */
