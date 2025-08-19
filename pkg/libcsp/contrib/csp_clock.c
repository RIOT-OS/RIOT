/*
 * Copyright (C) 2022 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @{
 * @file
 * @brief       LibCSP clock implementation
 *
 * @author      Pierre Millot <pierre.millot@grenoble-inp.org>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#include "csp/csp_types.h"

#include "cpu.h"
#include "timex.h"
#if IS_USED(MODULE_PERIPH_RTC) && !(MODULE_ZTIMER_PERIPH_RTC)
#include "periph/rtc.h"
#else
#include "ztimer.h"
#endif

__attribute__((weak)) void csp_clock_get_time(csp_timestamp_t *time)
{

#if IS_USED(MODULE_PERIPH_RTC) && !(MODULE_ZTIMER_PERIPH_RTC)
    struct tm time_s;
    if (IS_USED(MODULE_PERIPH_RTC_MS)) {
        uint16_t ms;
        rtc_get_time_ms(&time_s, &ms);
    }
    else {
        rtc_get_time(&time_s, &ms);
    }
    time->tv_sec = rtc_mktime(&time_s);
    time->tv_nsec = (ms % MS_PER_SEC) * NS_PER_US * US_PER_MS;
#else
    time->tv_sec = ztimer_now(ZTIMER_SEC);
#endif
    time->tv_nsec = 0;
}

__attribute__((weak)) int csp_clock_set_time(const csp_timestamp_t *time)
{
    (void)time;
#if IS_USED(MODULE_PERIPH_RTC) && !(MODULE_ZTIMER_PERIPH_RTC)
    struct tm time_s;
    rtc_localtime(time->tv_sec, &time_s);
    rtc_set_time(&time_s);
#endif
    return CSP_ERR_NOTSUP;
}
