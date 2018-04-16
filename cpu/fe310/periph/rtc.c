/*
 * Copyright 2017 Ken Rabold
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_fe310
 * @{
 *
 * @file        rtc.c
 * @brief       RTC interface wrapper for use with RTT modules
 *
 * @author      Ken Rabold
 * @}
 */

#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "cpu.h"
#include "periph_cpu.h"
#include "periph_conf.h"
#include "periph/rtt.h"
#include "periph/rtc.h"

#define ENABLE_DEBUG (0)
#include "debug.h"


typedef struct {
    rtc_alarm_cb_t cb; /**< callback called from RTC interrupt */
}rtc_state_t;

static rtc_state_t rtc_callback;

static void rtc_cb(void *arg);

void rtc_init(void)
{
    rtt_init();
}

int rtc_set_time(struct tm *time)
{
    time_t t = mktime(time);

    rtt_set_counter((uint32_t)t);

    return 0;
}

int rtc_get_time(struct tm *time)
{
    time_t t = (time_t)rtt_get_counter();

    gmtime_r(&t, time);

    return 0;
}

int rtc_set_alarm(struct tm *time, rtc_alarm_cb_t cb, void *arg)
{
    time_t t = mktime(time);

    rtc_callback.cb = cb;

    rtt_set_alarm((uint32_t)t, rtc_cb, arg);

    return 0;
}

int rtc_get_alarm(struct tm *time)
{
    time_t t = (time_t)rtt_get_alarm();

    gmtime_r(&t, time);

    return 0;
}

void rtc_clear_alarm(void)
{
    rtt_clear_alarm();
    rtc_callback.cb = NULL;
}

void rtc_poweron(void)
{
    rtt_poweron();
}

void rtc_poweroff(void)
{
    rtt_poweroff();
}

static void rtc_cb(void *arg)
{
    if (rtc_callback.cb != NULL) {
        rtc_callback.cb(arg);
    }
}
