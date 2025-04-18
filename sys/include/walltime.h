/*
 * Copyright (C) 2025 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
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
 * @brief   Set the system date / time
 *
 * @param[in] time  The current data / time to set
 */
void walltime_set(struct tm *time);

/**
 * @brief   Get the system date / time
 *
 * @param[out] time current time output
 * @param[out] ms   current milliseconds output, may be NULL
 *                  always returns 0 if the backend supports no millisecond resolution
 */
void walltime_get(struct tm *time, uint16_t *ms);

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

#ifdef __cplusplus
}
#endif

/** @} */
