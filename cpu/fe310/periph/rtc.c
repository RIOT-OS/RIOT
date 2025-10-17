/*
 * SPDX-FileCopyrightText: 2017 Ken Rabold
 * SPDX-License-Identifier: LGPL-2.1-only
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

#include "cpu.h"
#include "periph_cpu.h"
#include "periph_conf.h"
#include "periph/rtt.h"
#include "periph/rtc.h"

#define ENABLE_DEBUG 0
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

__attribute__((weak))
void rtc_pre_set_time(struct tm *old_time, const struct tm *new_time)
{
    (void)old_time;
    (void)new_time;
}

__attribute__((weak))
void rtc_post_set_time(struct tm *old_time, const struct tm *new_time)
{
    (void)old_time;
    (void)new_time;
}

int rtc_set_time(struct tm *time)
{
    struct tm old_time;
    rtc_get_time(&old_time);
    rtc_pre_set_time(&old_time, time);

    uint32_t t = rtc_mktime(time);

    rtt_set_counter(t);

    rtc_post_set_time(&old_time, time);
    return 0;
}

int rtc_get_time(struct tm *time)
{
    uint32_t t = rtt_get_counter();

    rtc_localtime(t, time);

    return 0;
}

int rtc_set_alarm(struct tm *time, rtc_alarm_cb_t cb, void *arg)
{
    uint32_t t = rtc_mktime(time);

    rtc_callback.cb = cb;

    rtt_set_alarm(t, rtc_cb, arg);

    return 0;
}

int rtc_get_alarm(struct tm *time)
{
    uint32_t t = rtt_get_alarm();

    rtc_localtime(t, time);

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
