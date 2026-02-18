/*
 * SPDX-FileCopyrightText: 2010 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-FileCopyrightText: 2014 Oliver Hahm <oliver.hahm@inria.fr>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    sys_timex Timex
 * @brief       Timestamp representation, computation, and conversion
 * @ingroup     sys
 *
 * @{
 * @file
 * @brief       Utility library for comparing and computing timestamps
 */

#include <stdint.h>
#include <inttypes.h>
#include "time_units.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The maximum length of the string representation of a timex timestamp
 */
#define TIMEX_MAX_STR_LEN   (20)
/* 20 =
 *  + 10 byte: 2^32-1 for seconds
 *  + 1 byte: decimal point
 *  + 6 byte: microseconds (normalized)
 *  + 2 byte: " s" (unit)
 *  + 1 byte: '\0'
 */

/**
 * @brief A timex timestamp
 *
 * @note  If a timestamp is not normalized, the number of microseconds might be
 *        > 1000000
 */
typedef struct {
    uint32_t seconds;       /**< number of seconds */
    uint32_t microseconds;  /**< number of microseconds */
} timex_t;

/**
 * @brief Adds two timestamps
 *
 * @param[in] a     First summand
 * @param[in] b     Second summand
 *
 * @return The sum of the two timestamps
 */
timex_t timex_add(const timex_t a, const timex_t b);

/**
 * @brief Subtracts two timestamps
 *
 * @param[in] a     The minuend
 * @param[in] b     The subtrahend
 *
 * @return The difference a - b
 */
timex_t timex_sub(const timex_t a, const timex_t b);

/**
 * @brief Initializes a timex timestamp
 *
 * @param[in] seconds       Number of seconds to set
 * @param[in] microseconds  Number of microseconds to set
 *
 * @return The initialized timex timestamp
 */
timex_t timex_set(uint32_t seconds, uint32_t microseconds);

/**
 * @brief Compares two timex timestamps
 *
 * @param[in] a The first timestamp to compare to
 * @param[in] b The second timestamp to compare with
 *
 * @return -1 when a is smaller
 * @return 0 if equal
 * @return 1 if a is bigger
 */
int timex_cmp(const timex_t a, const timex_t b);

/**
 * @brief Corrects timex structure so that microseconds < 1000000
 *
 * @param[in, out] time Pointer to the timestamp to normalize
 */
static inline void timex_normalize(timex_t *time)
{
    time->seconds += (time->microseconds / US_PER_SEC);
    time->microseconds %= US_PER_SEC;
}

/**
 * @brief Tests a timex timestamp for normalization
 *
 * @param[in] time  Pointer to the timestamp to check
 *
 * @return true for a normalized timex_t
 * @return false otherwise
 */
static inline int timex_isnormalized(const timex_t *time)
{
    return (time->microseconds < US_PER_SEC);
}

/**
 * @brief Converts a timex timestamp to a 64 bit value
 *
 * @param[in] a The timestamp to convert
 *
 * @return timex representation as uint64_t
 */
static inline uint64_t timex_uint64(const timex_t a)
{
    return (uint64_t) a.seconds * US_PER_SEC + a.microseconds;
}

/**
 * @brief Converts a 64 bit value of microseconds to a timex timestamp
 *
 * @param[in] timestamp The timestamp to convert.
 *
 * @return a timex representation of an uint64 timestamp.
 */
static inline timex_t timex_from_uint64(const uint64_t timestamp)
{
    return timex_set(timestamp / US_PER_SEC, timestamp % US_PER_SEC);
}

/**
 * @brief Converts a timex timestamp to a string
 *
 * @pre memory at timestamp >= TIMEX_MAX_STR_LEN
 *
 * @param[in]  t            The timestamp to convert
 * @param[out] timestamp    The output char buffer for the converted timestamp
 *
 * @note The timestamp will be normalized
 *
 * @return A pointer to the string representation of the timestamp
 */
const char *timex_to_str(timex_t t, char *timestamp);

#ifdef __cplusplus
}
#endif

/** @} */
