/*
 * Copyright (C) 2014 Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    drivers_periph_rtc RTC
 * @ingroup     drivers_periph
 * @brief       Low-level RTC (Real Time Clock) peripheral driver
 *
 * @note
 * The values used for setting and getting the time/alarm should
 * conform to the `struct tm` specification.
 * Compare: http://pubs.opengroup.org/onlinepubs/7908799/xsh/time.h.html
 *
 * # (Low-) Power Implications
 *
 * After the RTC has been initialized (i.e. after calling rtc_init()), the RTC
 * should be powered on and running. The RTC can then be powered off manually
 * at a later point in time by calling the rtc_poweroff() function. When the RTC
 * is powered back on using the rtc_poweron() function, it **should**
 * transparently continue its previously configured operation.
 *
 * On many CPUs, certain power states might need to be blocked in rtc_init(), so
 * that it is ensured that the RTC will function properly while it is enabled.
 *
 * @{
 * @file
 * @brief       Low-level RTC peripheral driver interface definitions
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 */

#ifndef PERIPH_RTC_H
#define PERIPH_RTC_H

#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(RIOT_EPOCH) || DOXYGEN
/**
 * @brief Earliest year of the RTC
 *
 * 01.01.$RIOT_EPOCH will be the reset value of the RTC if supported.
 *
 * Internal RTC helper functions such as @ref rtc_mktime and @ref rtc_localtime
 * will not work on dates earlier than that.
 */
#define RIOT_EPOCH (2020)
#endif

/**
 * @brief Signature for alarm Callback
 *
 * @param[in] arg           optional argument to put the callback in the right context
 */
typedef void(*rtc_alarm_cb_t)(void *arg);

/**
 * @brief Initialize RTC module
 */
void rtc_init(void);

/**
 * @brief Set RTC to given time.
 *
 * @param[in] time          Pointer to the struct holding the time to set.
 *
 * @return  0 for success
 * @return -1 an error occurred
 */
int rtc_set_time(struct tm *time);

/**
 * @brief Get current RTC time.
 *
 * @param[out] time         Pointer to the struct to write the time to.
 *
 * @return  0 for success
 * @return -1 an error occurred
 */
int rtc_get_time(struct tm *time);

/**
 * @brief Get current RTC time with sub-second component.
 *        Requires the `periph_rtc_ms` feature.
 *
 * @param[out] time         Pointer to the struct to write the time to.
 * @param[out] ms           Pointer to a variable to hold the microsecond
 *                          component of the current RTC time.
 *
 * @return  0 for success
 * @return -1 an error occurred
 */
int rtc_get_time_ms(struct tm *time, uint16_t *ms);

/**
 * @brief Set an alarm for RTC to the specified value.
 *
 * @note Any already set alarm will be overwritten.
 *
 * @param[in] time          The value to trigger an alarm when hit.
 * @param[in] cb            Callback executed when alarm is hit.
 * @param[in] arg           Argument passed to callback when alarm is hit.
 *
 * @return  0 for success
 * @return -2 invalid `time` parameter
 * @return -1 other errors
 */
int rtc_set_alarm(struct tm *time, rtc_alarm_cb_t cb, void *arg);

/**
 * @brief Gets the current alarm setting
 *
 * @param[out]  time        Pointer to structure to receive alarm time
 *
 * @return  0 for success
 * @return -1 an error occurred
 */
int rtc_get_alarm(struct tm *time);

/**
 * @brief Clear any set alarm, do nothing if nothing set
 */
void rtc_clear_alarm(void);

/**
 * @brief Turns the RTC hardware module on
 */
void rtc_poweron(void);

/**
 * @brief Turns the RTC hardware module off
 */
void rtc_poweroff(void);

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
 * @pre   The time structs @p a and @p b are assumed to be normalized.
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

#endif /* PERIPH_RTC_H */
/** @} */
