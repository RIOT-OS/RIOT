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

#if defined(MODULE_PERIPH_RTC_SETTER_CALLBACK) || DOXYGEN
/**
 * @brief Time change callback for user implementation
 *
 * This callback allows an application or module to react
 * to changes of the RTC. On execution of the callback,
 * the RTC time has already been changed.
 *
 * Available after loading the module `periph_rtc_setter_callback`.
 * On loading of the module, this callback must be implemented,
 * either by another module or the user's application.
 *
 * The callback will only be executed if the RTC is set
 * through @ref rtc_set_time_with_callback.
 *
 * @param[in] tm_before     Date and time before the update
 * @param[in] us_before     The microsecond part of the time
 *                          before the update
 * @param[in] tm_after      Date and time after the update
 * @param[in] us_after      The microsecond part of the time
 *                          after the update
 *
 * @return                  The calculated time difference,
 *                          intended for testing
 */
extern void rtc_setter_callback(
        struct tm *tm_before, uint32_t us_before,
        struct tm *tm_after, uint32_t us_after);

/**
 * @brief Wrapper for @ref rtc_set_time, adding a callback execution
 *
 * Available after loading the module `periph_rtc_setter_callback`.
 *
 * The supplied callback will be executed just after the time
 * change happened.
 *
 * @param[in] time          The input struct to @ref rtc_set_time
 *
 * @return                  The return value of @ref rtc_set_time
 */
int rtc_set_time_with_callback(struct tm *time);

/**
 * @brief Automatic choice of @ref rtc_set_time implementation
 *
 * @ref rtc_set_time_with_callback will be used when loading
 * the module `periph_rtc_setter_callback`. @ref rtc_set_time
 * will be used otherwise.
 */
#define RTC_SET_TIME rtc_set_time_with_callback
#else
#define RTC_SET_TIME rtc_set_time
#endif /* MODULE_PERIPH_RTC_SETTER_CALLBACK */

#ifdef __cplusplus
}
#endif

#endif /* RTC_UTILS_H */
/** @} */
