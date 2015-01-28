/*
 * Copyright (C) 2010 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2014 Oliver Hahm <oliver.hahm@inria.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */


/**
 * @defgroup    sys_timex Timex
 * @brief       Timestamp representation, computation, and conversion
 * @ingroup     sys
 *
 * @{
 * @file
 * @brief       Utility library for comparing and computing timestamps
 */

#ifndef __TIMEX_H
#define __TIMEX_H

#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Formater for unsigned 32 bit values
 *
 *        mspgcc bug : PRIxxx macros not defined before mid-2011 versions
 */
#ifndef PRIu32
#define PRIu32 "lu"
#endif

/**
 * @brief The number of microseconds per second
 */
#define SEC_IN_USEC 1000000

/**
 * @brief The maximum length of the string representation of a timex timestamp
 */
#define TIMEX_MAX_STR_LEN   (18)

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
/* cppcheck-suppress passedByValue */
timex_t timex_add(const timex_t a, const timex_t b);

/**
 * @brief Subtracts two timestamps
 *
 * @param[in] a     The minuend
 * @param[in] b     The subtrahend
 *
 * @return The difference a - b
 */
/* cppcheck-suppress passedByValue */
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
/* cppcheck-suppress passedByValue */
int timex_cmp(const timex_t a, const timex_t b);

/**
 * @brief Corrects timex structure so that microseconds < 1000000
 *
 * @param[in, out] time Pointer to the timestamp to normalize
 */
static inline void timex_normalize(timex_t *time)
{
    time->seconds += (time->microseconds / SEC_IN_USEC);
    time->microseconds %= SEC_IN_USEC;
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
    return (time->microseconds < SEC_IN_USEC);
}

/**
 * @brief Converts a timex timestamp to a 64 bit value
 *
 * @param[in] a The timestamp to convert
 *
 * @return timex representation as uint64_t
 */
/* cppcheck-suppress passedByValue */
static inline uint64_t timex_uint64(const timex_t a)
{
    return (uint64_t) a.seconds * SEC_IN_USEC + a.microseconds;
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
    return timex_set(timestamp / SEC_IN_USEC, timestamp % SEC_IN_USEC);
}

/**
 * @brief Converts a timex timestamp to a string
 *
 * @param[in]  t            The timestamp to convert
 * @param[out] timestamp    The output char buffer for the converted timestamp
 *
 * @note The timestamp will be normalized
 * @note The buffer must have a size of TIMEX_MAX_STR_LEN characters
 *
 * @return A pointer to the string representation of the timestamp
 */
static inline const char *timex_to_str(timex_t t, char *timestamp)
{
    timex_normalize(&t);
    /* 2^32 seconds have maximum 10 digits, microseconds are always < 1000000
     * in a normalized timestamp, plus two chars for the point and terminator
     * => 10 + 6 + 2 = 20 */
    /* TODO: replace call to snprintf by something more efficient */
    snprintf(timestamp, TIMEX_MAX_STR_LEN, "%" PRIu32 ".%06" PRIu32 " s",
             t.seconds, t.microseconds);
    return timestamp;
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* __TIMEX_H */
