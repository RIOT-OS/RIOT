/*
 * Copyright (C) 2014 Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * Copyright (C) 2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    drivers_rtc RTC
 * @ingroup     drivers
 * @brief       RTC (Real Time Clock) interface
 *
 * @note
 * The values used for setting and getting the time/alarm should
 * conform to the `struct tm` specification.
 * @see http://pubs.opengroup.org/onlinepubs/7908799/xsh/time.h.html
 *
 * @{
 * @file
 * @brief       RTC interface definitions
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef RTC_H
#define RTC_H

#include <time.h>
#include "clist.h" /* for clist_node_t */

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declaration */
struct rtc_ops;

/**
 * @brief Basic RTC device descriptor
 *
 * This is extended in device drivers by creating a new struct which has an
 * @c rtc_t object as the first member variable. The new struct will then be
 * safe to cast to @c rtc_t during function calls.
 */
typedef struct {
    clist_node_t list_entry; /**< List entry handle */
    const struct rtc_ops *rtc_op; /**< RTC operations table */
    const char *name; /**< Device instance user-friendly name, for use in shell commands etc. */
} rtc_t;

/**
 * @brief Signature for alarm Callback
 *
 * @param[in] arg           optional argument to put the callback in the right context
 */
typedef void(*rtc_alarm_cb_t)(void *arg);

/**
 * @brief RTC device operations table
 *
 * This table contains pointers to all the device functions in an RTC driver.
 */
typedef struct rtc_ops {
    /**
     * @brief Initialize the given RTC device
     *
     * @param[in]  rtc    RTC device descriptor to initialize
     *
     * @return  0 on success
     * @return <0 on error
     */
    int (*init) (rtc_t *rtc);

    /**
     * @brief Set the RTC device to the given time
     *
     * @param[in]  rtc    RTC device descriptor
     * @param[in]  time   Pointer to the struct holding the time to set.
     *
     * @return  0 on success
     * @return <0 on error
     */
    int (*set_time) (rtc_t *rtc, const struct tm *time);

    /**
     * @brief Get the current time from RTC
     *
     * @param[in]  rtc    RTC device descriptor
     * @param[in]  time   Pointer to the struct to write the time to
     *
     * @return  0 on success
     * @return <0 on error
     */
    int (*get_time) (rtc_t *rtc, struct tm *time);

    /**
     * @brief Set an alarm on the RTC to happen at the given @p time
     *
     * It is expected that any previously set alarms will be discarded when a
     * new alarm is set.
     *
     * @param[in]  rtc    RTC device descriptor
     * @param[in]  time   Pointer to the struct to write the time to
     * @param[in]  cb     Callback executed when alarm is hit.
     * @param[in]  arg    Argument passed to callback when alarm is hit.
     *
     * @return  0 on success
     * @return <0 on error
     */
    int (*set_alarm) (rtc_t *rtc, const struct tm *time, rtc_alarm_cb_t cb, void *arg);

    /**
     * @brief Get the current alarm time from RTC
     *
     * @param[in]  rtc    RTC device descriptor
     * @param[in]  time   Pointer to the struct to write the time to
     *
     * @return  0 on success
     * @return <0 on error
     */
    int (*get_alarm) (rtc_t *rtc, struct tm *time);

    /**
     * @brief Clear any previously set alarms from the RTC
     *
     * @param[in]  rtc    RTC device descriptor
     *
     * @return  0 on success
     * @return <0 on error
     */
    void (*clear_alarm) (rtc_t *rtc);
} rtc_ops_t;

/**
 * @brief Register an RTC instance in the system list of RTC devices
 *
 * @param[in]  rtc  RTC device descriptor
 *
 * @return  0 on success
 * @return @c -EEXIST if @p rtc is already registered
 * @return <0 on error
 */
int rtc_register(rtc_t *rtc);

/**
 * @brief Remove a registered RTC instance from the system list of RTC devices
 *
 * @param[in]  rtc  RTC device descriptor
 *
 * @return  0 on success
 * @return @c -ENOENT if @p rtc is not registered
 * @return <0 on error
 */
int rtc_unregister(rtc_t *rtc);

/**
 * @brief Find a registered RTC instance by name, or any
 *
 * If @p name is NULL, an arbitrary item from the list of registered RTC devices
 * will be returned instead.
 *
 * @param[in]  name  Name to search for
 *
 * @return Pointer to rtc_t instance matching the given string
 * @return Pointer to an arbitrary rtc_t instance, if @p name is NULL
 * @return NULL if no match was found or if the list was empty
 */
rtc_t *rtc_find(const char *name);

/**
 * @brief Iterate through registered RTC devices
 *
 * Not thread safe!
 *
 * Set @p cur to @c NULL to start from the beginning
 *
 * @see @c sc_vfs.c (@c df command) for a usage example
 *
 * @param[in]  cur  current iterator value
 *
 * @return     Pointer to next mounted file system in list after @p cur
 * @return     NULL if @p cur is the last element in the list
 */
rtc_t *rtc_iterate(const rtc_t *cur);

/**
 * @brief Initialize an RTC device
 *
 * @param[in]  rtc  RTC device descriptor
 *
 * @return  0 on success
 * @return <0 on error
 */
int rtc_init(rtc_t *rtc);

/**
 * @brief Set RTC to given time.
 *
 * @param[in]  rtc   RTC device descriptor
 * @param[in]  time  Pointer to the struct holding the time to set
 *
 * @return  0 on success
 * @return <0 on error
 */
int rtc_set_time(rtc_t *rtc, const struct tm *time);

/**
 * @brief Get current RTC time.
 *
 * @param[in]  rtc   RTC device descriptor
 * @param[out] time  Pointer to the struct to write the time to
 *
 * @return  0 on success
 * @return <0 on error
 */
int rtc_get_time(rtc_t *rtc, struct tm *time);

/**
 * @brief Set an alarm for RTC to the specified value.
 *
 * @note Any previously set alarm will be overwritten.
 *
 * @param[in]  rtc   RTC device descriptor
 * @param[in]  time  The value to trigger an alarm when hit
 * @param[in]  cb    Callback executed when alarm is hit
 * @param[in]  arg   Argument passed to callback when alarm is hit
 *
 * @return  0 on success
 * @return <0 on error
 */
int rtc_set_alarm(rtc_t *rtc, const struct tm *time, rtc_alarm_cb_t cb, void *arg);

/**
 * @brief Get the current alarm setting
 *
 * @param[in]   rtc   RTC device descriptor
 * @param[out]  time  Pointer to structure to receive alarm time
 *
 * @return  0 on success
 * @return <0 on error
 */
int rtc_get_alarm(rtc_t *rtc, struct tm *time);

/**
 * @brief Clear any set alarm, do nothing if nothing set
 */
void rtc_clear_alarm(rtc_t *rtc);

#ifdef __cplusplus
}
#endif

#endif /* RTC_H */
/** @} */
