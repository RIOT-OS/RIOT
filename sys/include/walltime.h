/*
 * SPDX-FileCopyrightText: 2025 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */
#pragma once

/**
 * @defgroup    sys_walltime    Wall-Clock Time Helper Functions
 * @ingroup     sys
 * @brief       Common functions to access the wall-clock / real time clock
 *
 * Use these functions to access system time, don't directly use the
 * `periph_rtc` API.
 * This allows to write applications that are independent of the RTC
 * peripheral and allows the implementation of on-time change callbacks.
 *
 * @note The values used for setting and getting the time/alarm should
 *       conform to the `struct tm` specification.
 *       You can use the @ref fmt_time_tm_iso8601() and @ref scn_time_tm_iso8601_date()
 *       functions from the `fmt` package.
 *       Compare: http://pubs.opengroup.org/onlinepubs/7908799/xsh/time.h.html
 *
 * @{
 * @file
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Time change notification callback
 *
 * @param[in,out]   ctx     User supplied context
 * @param[in]       diff_sec Seconds portion of the time change
 * @param[in]       diff_ms Millisecond portion of the time change
 *
 */
typedef void (*walltime_change_cb_t)(void *ctx, int32_t diff_sec, int16_t diff_ms);

/**
 * @brief Signature for Alarm Callback
 *
 * @param[in] arg   Optional argument to put the callback in the right context
 */
typedef void(*walltime_alarm_cb_t)(void *arg);

/**
 * @brief   Time change notification subscription
 * @{
 */
typedef struct {
    void *next;                 /**< Next pointer, internal use only */
    walltime_change_cb_t cb;    /**< Time change callback function   */
    void *ctx;                  /**< Callback function context       */
} walltime_change_sub_t;
/** @} */

/**
 * @brief   Add a time change notification subscription.
 *          This will be called whenever the system time changes.
 *
 * @note    @p sub must remain valid as long as the subscription is active.
 *
 * @param[in] sub   The time change notify subscription to add.
 */
void walltime_change_subscribe(walltime_change_sub_t *sub);

/**
 * @brief   Remove a time change notification subscription
 *
 * @param[in] sub   The time change notify subscription to remove
 *
 * @retval  true    The subscription was removed
 * @retval  false   The subscription could not be found
 */
bool walltime_change_unsubscribe(walltime_change_sub_t *sub);

/**
 * @brief   Set the system date / time
 *
 * @param[in] time  The current data / time to set
 *
 * @retval 0        on success
 * @retval -ERANGE  if the difference to the current time is too large
 */
int walltime_set(struct tm *time);

/**
 * @brief   Get the system date / time
 *
 * @param[out] time Current time output
 * @param[out] ms   Current milliseconds output, may be NULL.
 *                  Always set to 0 if the backend does not support millisecond
 *                  resolution.
 *
 * @retval 0        on success
 * @retval -ERANGE  if the difference to the current time is too large
 */
int walltime_get(struct tm *time, uint16_t *ms);

/**
 * @brief   Get the current system time in seconds since @ref RIOT_EPOCH
 *
 * @param[out] ms   Current milliseconds output, may be NULL.
 *                  Always set to 0 if the backend does not support millisecond
 *                  resolution.
 *
 * @returns seconds since `RIOT_EPOCH`
 */
uint32_t walltime_get_riot(uint16_t *ms);

/**
 * @brief   Get the current system time in seconds since 01.01.1970
 *
 * @param[out] ms   Current milliseconds output, may be NULL.
 *                  Always set to 0 if the backend does not support millisecond
 *                  resolution.
 *
 * @returns seconds since 01.01.1970
 */
time_t walltime_get_unix(uint16_t *ms);

/**
 * @brief Set an alarm for RTC to the specified value.
 *
 * @note Any already set alarm will be overwritten.
 *
 * @param[in] time      The value to trigger an alarm when hit.
 * @param[in] cb        Callback executed when alarm is hit.
 *                      Set this to NULL to clear the alarm.
 * @param[in] arg       Argument passed to callback when alarm is hit, may be
 *                      NULL.
 *
 * @retval  0           on success
 * @retval  -EINVAL     @p time was invalid (e.g. in the past, out of range)
 * @retval  <0          other error (negative errno code to indicate cause,
 *                      depends on the implementation!)
 */
int walltime_set_alarm(struct tm *time, walltime_alarm_cb_t cb, void *arg);

/**
 * @brief Gets the current alarm setting
 *
 * @param[out] time     Pointer to structure to receive alarm time
 *
 * @retval  0           on success
 * @retval  -EINVAL     no alarm is configured
 * @retval  <0          other error (negative errno code to indicate cause,
 *                      depends on the implementation!)
 */
int walltime_get_alarm(struct tm *time);

/**
 * @brief   Get seconds elapsed since last reset
 *
 * @note    The @p full option will only have an effect if @ref BACKUP_RAM
 *          is available.
 *
 * @param[in] full  Set to `false` to get seconds since last (warm) boot / wake-up.
 *                  Set to `true` to get seconds since last cold boot / full reset.
 *
 * @return  Seconds since cold/warm boot.
 */
uint32_t walltime_uptime(bool full);

/**
 * @brief   Backend implementation init, only implement this function when
 *          required.
 */
void walltime_impl_init(void);

/**
 * @brief   Backend implementation to set the system time
 *
 * @param[in] time  The current data / time to set
 *
 * @retval 0 on success
 */
int walltime_impl_set(struct tm *time);

/**
 * @brief   Backend implementation to get the system time
 *
 * @param[out] time Current time output
 * @param[out] ms   Current milliseconds output (will never be NULL)
 *
 * @retval 0 on success
 */
int walltime_impl_get(struct tm *time, uint16_t *ms);

/**
 * @brief   Backend implementation to set the alarm
 *
 * @note    The @p time value is normalized by the upper layer.
 *
 * @param[in] time      The value to trigger an alarm when hit.
 * @param[in] cb        Callback executed when alarm is hit.
 *                      Set this to NULL to clear the alarm.
 * @param[in] arg       Argument passed to callback when alarm is hit.
 *
 * @retval  0           on success
 * @retval  -EINVAL     @p time was invalid (e.g. in the past, out of range)
 * @retval  <0          other error (negative errno code to indicate cause,
 *                      depends on the implementation!)
 */
int walltime_impl_alarm_set(struct tm *time, walltime_alarm_cb_t cb, void *arg);

/**
 * @brief Backend implementation to get the current alarm setting
 *
 * @param[out] time     Pointer to structure to receive alarm time
 *
 * @retval  0           on success
 * @retval  -EINVAL     if no alarm is configured
 * @retval  <0          other error (negative errno code to indicate cause,
 *                      depends on the implementation!)
 */
int walltime_impl_alarm_get(struct tm *time);

#ifdef __cplusplus
}
#endif

/** @} */
