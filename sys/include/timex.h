
/**
 * @defgroup    sys_timex Timex
 * @ingroup     sys
 * @brief       Utility library for comparing and computing timestamps
 */

#ifndef __TIMEX_H
#define __TIMEX_H

#include <stdint.h>
#include <inttypes.h>

// mspgcc bug : PRIxxx macros not defined before mid-2011 versions
#ifndef PRIu32
#define PRIu32 "lu"
#endif

typedef struct timex_t {
    uint32_t seconds;
    uint32_t microseconds;
} timex_t;

/* a+b */
/* cppcheck-suppress passedByValue */
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
void timex_normalize(timex_t *time);

/**
 * @brief Tests a timex_t for normalization
 *
 * @return true for a normalized timex_t, false otherwise
 */
int timex_isnormalized(timex_t *time);

/**
 * @brief Denormalizes a timex_t to a uint64_t
 *
 * @return timex representation as uint64_t
 */
uint64_t timex_uint64(const timex_t a);

/**
 * @brief Converts a uint64_t of microseconds to a timex_t
 *
 * @return a timex representation of an uint64 timestamp.
 */
timex_t timex_from_uint64(const uint64_t timestamp);

/**
 * @brief Prints a timex_t
 */
void timex_print(const timex_t t);

#endif /* __TIMEX_H */
