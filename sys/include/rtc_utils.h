/*
 * Copyright (C) 2019 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    sys_rtc_utils RTC helpers
 * @ingroup     sys
 * @brief       Common RTC helper functions
 * @{
 * @file
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#ifndef RTC_UTILS_H
#define RTC_UTILS_H

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Normalize the time struct
 *
 * @note  The function modifies the fields of the tm structure as follows:
 *        If structure members are outside their valid interval,
 *        they will be normalized.
 *        So that, for example, 40 October is changed into 9 November.
 *
 *        If RTC_NORMALIZE_COMPAT is 1 `tm_wday` and `tm_yday` are set
 *        to values determined from the contents of the other fields.
 *
 * @param time        Pointer to the struct to normalize.
 */
void rtc_tm_normalize(struct tm *time);

/**
 * @brief Compare two time structs.
 *
 * @pre   The time structs @p a and @p b are assumed to be normalized.
 *        Use @ref rtc_tm_normalize to normalize a struct tm that has been
 *        manually edited.
 *
 * @param[in] a       The first time struct.
 * @param[in] b       The second time struct.
 *
 * @return an integer < 0 if a is earlier than b
 * @return an integer > 0 if a is later than b
 * @return              0 if a and b are equal
 */
int rtc_tm_compare(const struct tm *a, const struct tm *b);

/**
 * @brief Convert time struct into timestamp.
 *
 * @pre   The time struct t is assumed to be normalized.
 *        Use @ref rtc_tm_normalize to normalize a struct tm that has been
 *        manually edited.
 *
 * @param[in] t       The time struct to convert
 *
 * @return            elapsed seconds since `RIOT_EPOCH`
 */
uint32_t rtc_mktime(struct tm *t);

/**
 * @brief Converts an RTC timestamp into a  time struct.
 *
 * @param[in]  time   elapsed seconds since `RIOT_EPOCH`
 * @param[out] t      the corresponding timestamp
 */
void rtc_localtime(uint32_t time, struct tm *t);

/**
 * @brief Verify that a time struct @p t contains valid data.
 *
 * @note    This function checks whether the fields of the
 *          struct @p t are positive and within the bounds set
 *          by @ref rtc_tm_normalize.
 *
 * @param[in] t       The struct to be checked.
 *
 * @return            true when valid, false if not
 */
bool rtc_tm_valid(const struct tm *t);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* RTC_UTILS_H */
