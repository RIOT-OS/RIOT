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
 *              Use these functions to access system time, don't directly use the
 *              `periph_rtc` API.
 *              This allows to write applications that are independent of the RTC
 *              peripheral and allows the implementation of on-time change callbacks.
 * @note
 * The values used for setting and getting the time/alarm should
 * conform to the `struct tm` specification.
 * Compare: http://pubs.opengroup.org/onlinepubs/7908799/xsh/time.h.html
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
 * @param[in, out]  ctx User supplied context
 * @param[in]   diff_sec    seconds portion of the time change
 * @param[in]   diff_ms     millisecond portion of the time change
 *
 */
typedef void (*walltime_change_cb_t)(void *ctx, int32_t diff_sec, int16_t diff_ms);

/**
 * @brief   Time change notification subscription
 * @{
 */
typedef struct {
    void *next;                 /**< next pointer, internal use only */
    walltime_change_cb_t cb;    /**< time change callback function   */
    void *ctx;                  /**< callback function context       */
} walltime_change_sub_t;
/** @} */

/**
 * @brief   Add a time change notification subscription
 *          This will be called whenever the system time changes
 *
 * @note    @p sub must remain valid as long as the subscription is active.
 *
 * @param[in] sub   The time change notify subscription to add
 */
void walltime_change_subscribe(walltime_change_sub_t *sub);

/**
 * @brief   Remove a time change notification subscription
 *
 * @param[in] sub   The time change notify subscription to remove
 *
 * @retval  true  The subscription was removed
 * @retval  false The subscription could not be found
 */
bool walltime_change_unsubscribe(walltime_change_sub_t *sub);

/**
 * @brief   Set the system date / time
 *
 * @param[in] time  The current data / time to set
 *
 * @returns 0 on success
 * @returns -ERANGE if the difference to the current time is too large
 */
int walltime_set(struct tm *time);

/**
 * @brief   Get the system date / time
 *
 * @param[out] time current time output
 * @param[out] ms   current milliseconds output, may be NULL
 *                  always returns 0 if the backend supports no millisecond resolution
 *
 * @returns 0 on success
 * @returns -ERANGE if the difference to the current time is too large
 */
int walltime_get(struct tm *time, uint16_t *ms);

/**
 * @brief   Get the current system time in seconds since @ref RIOT_EPOCH
 *
 * @param[out] ms   current milliseconds output, may be NULL
 *                  always returns 0 if the backend supports no millisecond resolution
 *
 * @returns seconds since `RIOT_EPOCH`
 */
uint32_t walltime_get_riot(uint16_t *ms);

/**
 * @brief   Get the current system time in seconds since 01.01.1970
 *
 * @param[out] ms   current milliseconds output, may be NULL
 *                  always returns 0 if the backend supports no millisecond resolution
 *
 * @returns seconds since 01.01.1970
 */
time_t walltime_get_unix(uint16_t *ms);

/**
 * @brief   Get seconds elapsed since last reset
 *
 * @note    The @p full option will only have an effect if @ref BACKUP_RAM
 *          is available.
 *
 * @param[in] full  set to false to get seconds since last (warm) boot / wake-up
 *                  set to true to get seconds since last cold boot / full reset
 *
 */
uint32_t walltime_uptime(bool full);

/**
 * @brief   Backend implementation init, only implement when necessary.
 */
void walltime_impl_init(void);

/**
 * @brief   Backend implementation to set the system time
 *
 * @param[in] time  The current data / time to set
 *
 * @returns 0 on success
 */
int walltime_impl_set(struct tm *time);

/**
 * @brief   Backend implementation to get the system time
 *
 * @param[out] time current time output
 * @param[out] ms   current milliseconds output (will never be NULL)
 *
 * @returns 0 on success
 */
int walltime_impl_get(struct tm *time, uint16_t *ms);

#ifdef __cplusplus
}
#endif

/** @} */
