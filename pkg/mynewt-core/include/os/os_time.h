/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     pkg_mynewt_core
 * @{
 *
 * @file
 * @brief       mynewt-core time abstraction
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#include "os/os_error.h"
#include "ztimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Ticks per seconds, ztimer_msec is used as system timer
 */
#define OS_TICKS_PER_SEC (MS_PER_SEC)

/**
 * @brief Returns the low 32 bits of cputime.
 *
 * @return uint32_t The lower 32 bits of cputime
 */
static inline os_time_t os_time_get(void)
{
    return ztimer_now(ZTIMER_MSEC);
}

/**
 * @brief Converts the given number of milliseconds into cputime ticks.
 *
 * @param[in]   ms          The number of milliseconds to convert to ticks
 * @param[out]  out_ticks   The number of ticks corresponding to 'ms'
 *
 * @return os_error_t  OS_OK - no error
 */
static inline os_error_t os_time_ms_to_ticks(uint32_t ms, os_time_t *out_ticks)
{
    *out_ticks = ms;
    return OS_OK;
}

/**
 * @brief Convert the given number of ticks into milliseconds.
 *
 * @param[in]   ticks   The number of ticks to convert to milliseconds.
 * @param[out]  out_ms  The converted milliseconds from 'ticks'
 *
 * @return os_error_t  OS_OK - no error
 */
static inline os_error_t  os_time_ticks_to_ms(os_time_t ticks, uint32_t *out_ms)
{
    *out_ms = ticks;
    return OS_OK;
}

/**
 * @brief   Converts the given number of milliseconds into cputime ticks.
 *
 * @param[in]   ms  The number of milliseconds to convert to ticks
 *
 * @return  uint32_t    The number of ticks corresponding to 'ms'
 */
static inline os_time_t os_time_ms_to_ticks32(uint32_t ms)
{
    return ms;
}

/**
 * @brief   Convert the given number of ticks into milliseconds.
 *
 * @param[in]   ticks   The number of ticks to convert to milliseconds.
 *
 * @return  uint32_t    The number of milliseconds corresponding to 'ticks'
 */
static inline os_time_t os_time_ticks_to_ms32(os_time_t ticks)
{
    return ticks;
}

/**
 * @brief   Wait until the number of ticks has elapsed, BLOICKING.
 *
 * @param[in]   ticks   The number of ticks to wait.
 */
static inline void os_time_delay(os_time_t ticks)
{
    if (irq_is_in()) {
        ztimer_spin(ZTIMER_MSEC, ticks);
    }
    else {
        ztimer_sleep(ZTIMER_MSEC, ticks);
    }
}

#ifdef __cplusplus
}
#endif
