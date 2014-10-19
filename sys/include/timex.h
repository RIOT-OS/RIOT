/*
 * Copyright (C) 2010 Kaspar Schleiser
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */


/**
 * @defgroup    sys_timex Timex
 * @ingroup     sys
 * @brief       Utility library for comparing and computing timestamps
 */

#ifndef __TIMEX_H
#define __TIMEX_H

#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>

// mspgcc bug : PRIxxx macros not defined before mid-2011 versions
#ifndef PRIu32
#define PRIu32 "lu"
#endif

#define SEC_IN_USEC 1000000

typedef struct timex_t {
    uint32_t seconds;
    uint32_t microseconds;
} timex_t;

/* a+b */
timex_t timex_add(const timex_t a, const timex_t b);

/* a-b*/
timex_t timex_sub(const timex_t a, const timex_t b);

timex_t timex_set(uint32_t seconds, uint32_t microseconds);

/**
 * @brief Compares two timex values.
 *
 * @return -1 when a is smaller, 0 if equal, 1 if a is bigger
 */
int timex_cmp(const timex_t a, const timex_t b);

/**
 * @brief Corrects timex_t structure so that microseconds < 1000000
 */
static inline void timex_normalize(timex_t *time)
{
    time->seconds += (time->microseconds / SEC_IN_USEC);
    time->microseconds %= SEC_IN_USEC;
}

/**
 * @brief Tests a timex_t for normalization
 *
 * @return true for a normalized timex_t, false otherwise
 */
static inline int timex_isnormalized(timex_t *time)
{
    return (time->microseconds < SEC_IN_USEC);
}

/**
 * @brief Denormalizes a timex_t to a uint64_t
 *
 * @return timex representation as uint64_t
 */
static inline uint64_t timex_uint64(const timex_t a)
{
    return (uint64_t) a.seconds * SEC_IN_USEC + a.microseconds;
}

/**
 * @brief Converts a uint64_t of microseconds to a timex_t
 *
 * @return a timex representation of an uint64 timestamp.
 */
static inline timex_t timex_from_uint64(const uint64_t timestamp)
{
    return timex_set(timestamp / SEC_IN_USEC, timestamp % SEC_IN_USEC);
}

/**
 * @brief Prints a timex_t
 */
static inline void timex_print(const timex_t t)
{
    printf("Seconds: %" PRIu32 " - Microseconds: %" PRIu32 "\n", t.seconds, t.microseconds);
}

#endif /* __TIMEX_H */
