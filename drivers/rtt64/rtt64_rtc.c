/*
 * Copyright (C) 2022 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_rtt64
 * @{
 *
 * @file
 * @brief       Basic RTC implementation based on RTT64
 *
 * @note        Unlike a real RTC, this emulated version is not guaranteed to keep
 *              time across reboots or deep sleep.
 *              If your hardware provides the means to implement a real RTC, always
 *              prefer that over this emulated version!
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <time.h>

#include "irq.h"
#include "periph/rtc.h"
#include "rtt64.h"
#include "time_units.h"

#define ENABLE_DEBUG    0
#include "debug.h"

void rtc_init(void)
{
    rtt64_init();
}

int rtc_set_time(struct tm *time)
{
    rtt64_set_time(mktime(time), 0);
    return 0;
}

int rtc_get_time_ms(struct tm *time, uint16_t *ms)
{
    uint64_t secs;
    uint32_t us;

    rtt64_get_time(&secs, &us);

    if (ms) {
        *ms = us / US_PER_MS;
    }
    localtime_r((time_t *)&secs, time);

    return 0;
}

void rtt_rtc_gettimeofday(uint32_t *secs, uint32_t *us)
{
    uint64_t secs64;

    rtt64_get_time(&secs64, us);
    *secs = (uint32_t)secs64;
}

void rtt_rtc_settimeofday(uint32_t s, uint32_t us)
{
    rtt64_set_time(s, us);
}

int rtc_get_time(struct tm *time)
{
    return rtc_get_time_ms(time, NULL);
}

int rtc_get_alarm(struct tm *time)
{
    uint64_t secs;
    uint32_t us;

    rtt64_get_alarm_time(&secs, &us);
    localtime_r((time_t *)&secs, time);

    return 0;
}

int rtc_set_alarm(struct tm *time, rtc_alarm_cb_t cb, void *arg)
{
    rtt64_set_alarm_time(mktime(time), 0, cb, arg);

    return 0;
}

void rtc_clear_alarm(void)
{
    rtt64_clear_alarm();
}

void rtc_poweron(void)
{
    rtt_poweron();
}

void rtc_poweroff(void)
{
    rtt_poweroff();
}
