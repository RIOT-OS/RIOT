/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_bus
 * @{
 *
 * @file
 * @brief       RTC event bus
 *
 * @author      Daniel Lockau <daniel.lockau@ml-pa.com>
 * @}
 */

#include "periph/rtc.h"
#include "sys/bus.h"
#include "timex.h"

/**
 * @brief Compute millisecond difference between two time stamps
 *
 * Internal function.
 */
int32_t sys_bus_rtc_compute_diff_ms(
        struct tm *tm_before, uint32_t us_before,
        struct tm *tm_after, uint32_t us_after)
{
    assert(us_before < US_PER_SEC);
    assert(us_after < US_PER_SEC);

    uint32_t s_before = rtc_mktime(tm_before);
    uint32_t s_after = rtc_mktime(tm_after);

    /* default content is out of range special value INT32_MIN */
    int32_t diff_ms = RTC_EVENT_OUT_OF_RANGE;

    /* calculate time difference if in range */
    if (((s_after > s_before) ? (s_after - s_before) : (s_before - s_after))
            < (INT32_MAX / MS_PER_SEC)) {
        diff_ms = ((int32_t)(s_after - s_before) * (int32_t)MS_PER_SEC)
            + ((int32_t)(us_after - us_before) / (int32_t)US_PER_MS);
    }

    return diff_ms;
}

#ifdef MODULE_SYS_BUS_RTC

/**
 * @brief Setter callback for use with @ref rtc_set_time_with_callback
 *
 * Internal function.
 */
void rtc_setter_callback(
        struct tm *tm_before, uint32_t us_before,
        struct tm *tm_after, uint32_t us_after)
{
    int32_t diff_ms = sys_bus_rtc_compute_diff_ms(tm_before, us_before,
            tm_after, us_after);

    /* post result on a non-zero millisecond difference */
    if (diff_ms != 0) {
        msg_bus_post(sys_bus_get(SYS_BUS_RTC), SYS_BUS_RTC_EVENT_TIME_CHANGE,
                     (void *)(uintptr_t)diff_ms);
    }
}

#endif /* MODULE_SYS_BUS_RTC */
