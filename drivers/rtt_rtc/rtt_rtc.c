/*
 * Copyright (C) 2020 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_rtt_rtc
 * @{
 *
 * @file
 * @brief       Basic RTC implementation based on a RTT
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

#include <stdlib.h>
#include <string.h>

#include "irq.h"
#include "periph/rtc.h"
#include "periph/rtt.h"
#include "timex.h"
#include "rtt_rtc.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#define RTT_SECOND      (RTT_FREQUENCY)

#define _RTT(n)         ((n) & RTT_MAX_VALUE)

#define RTT_SECOND_MAX  (RTT_MAX_VALUE/RTT_FREQUENCY)

#define TICKS(x)        (    (x) * RTT_SECOND)
#define SECONDS(x)      (_RTT(x) / RTT_SECOND)
#define SUBSECONDS(x)   (_RTT(x) % RTT_SECOND)

/* Place counter in .noinit section if no backup RAM is available.
   This means the date is undefined at cold boot, but will likely still
   survive a reboot. */
#ifndef BACKUP_RAM
#define BACKUP_RAM      __attribute__((section(".noinit")))
#endif

static uint32_t rtc_now BACKUP_RAM;         /**< The RTC timestamp when the last RTT alarm triggered */
static uint32_t last_alarm BACKUP_RAM;      /**< The RTT timestamp of the last alarm */

static uint32_t alarm_time;                 /**< The RTC timestamp of the (user) RTC alarm */
static rtc_alarm_cb_t alarm_cb;             /**< RTC alarm callback */
static void *alarm_cb_arg;                  /**< RTC alarm callback argument */

static void _rtt_alarm(void *arg);

/* convert RTT counter into RTC timestamp */
static inline uint32_t _rtc_now(uint32_t now)
{
    return rtc_now + SECONDS(now - last_alarm);
}

static inline void _set_alarm(uint32_t now, uint32_t next_alarm)
{
    rtt_set_alarm(now + next_alarm, _rtt_alarm, NULL);
}

/* This calculates when the next alarm should happen.
   Always chooses the longest possible period min(alarm, overflow)
   to minimize the amount of wake-ups. */
static void _update_alarm(uint32_t now)
{
    uint32_t next_alarm;

    last_alarm = TICKS(SECONDS(now));

    /* no alarm or alarm beyond this period */
    if ((alarm_cb == NULL)     ||
        (alarm_time < rtc_now) ||
        (alarm_time - rtc_now > RTT_SECOND_MAX)) {
        next_alarm = RTT_SECOND_MAX;
    } else {
        /* alarm triggers in this period */
        next_alarm = alarm_time - rtc_now;
    }

    /* alarm triggers NOW */
    if (next_alarm == 0) {
        next_alarm = RTT_SECOND_MAX;
        alarm_cb(alarm_cb_arg);
    }

    _set_alarm(now, TICKS(next_alarm));
}

/* the RTT alarm callback */
static void _rtt_alarm(void *arg)
{
    (void) arg;

    uint32_t now = rtt_get_counter();
    rtc_now = _rtc_now(now);

    _update_alarm(now);
}

void rtc_init(void)
{
    /* only update last_alarm on cold boot */
    if (last_alarm == 0) {
        last_alarm = rtt_get_counter();
    }

    _set_alarm(last_alarm, TICKS(RTT_SECOND_MAX));
}

int rtc_set_time(struct tm *time)
{
    rtc_tm_normalize(time);

    /* disable alarm to prevent race condition */
    rtt_clear_alarm();

    uint32_t now = rtt_get_counter();
    rtc_now      = rtc_mktime(time);

    /* calculate next wake-up period */
    _update_alarm(now);

    return 0;
}

int rtc_get_time_ms(struct tm *time, uint16_t *ms)
{
    uint32_t now, tmp;
    unsigned state = irq_disable();

    now  = rtt_get_counter();
    tmp  = _rtc_now(now);

    *ms = (SUBSECONDS(now - last_alarm) * MS_PER_SEC)
        / RTT_SECOND;

    irq_restore(state);
    rtc_localtime(tmp, time);

    return 0;
}

int rtc_get_time(struct tm *time)
{
    uint32_t now, tmp;
    unsigned state = irq_disable();

    now  = rtt_get_counter();
    tmp  = _rtc_now(now);

    irq_restore(state);
    rtc_localtime(tmp, time);

    return 0;
}

int rtc_get_alarm(struct tm *time)
{
    rtc_localtime(alarm_time, time);

    return 0;
}

int rtc_set_alarm(struct tm *time, rtc_alarm_cb_t cb, void *arg)
{
    /* disable alarm to prevent race condition */
    rtt_clear_alarm();

    uint32_t now = rtt_get_counter();

    alarm_time   = rtc_mktime(time);
    alarm_cb_arg = arg;
    alarm_cb     = cb;

    /* RTT interrupt is disabled here */
    rtc_now      = _rtc_now(now);
    _update_alarm(now);

    return 0;
}

void rtc_clear_alarm(void)
{
    alarm_cb = NULL;
}

void rtc_poweron(void)
{
    rtt_poweron();
}

void rtc_poweroff(void)
{
    rtt_poweroff();
}

void rtt_rtc_settimeofday(uint32_t s, uint32_t us)
{
    /* disable alarm to prevent race condition */
    rtt_clear_alarm();
    uint32_t now = ((uint64_t)us * RTT_SECOND) / US_PER_SEC;
    rtc_now      = s;
    rtt_set_counter(now);
    /* calculate next wake-up period */
    _update_alarm(0);
}

void rtt_rtc_gettimeofday(uint32_t *s, uint32_t *us)
{
    uint32_t now;
    unsigned state = irq_disable();

    now  = rtt_get_counter();
    *s   = _rtc_now(now);
    *us  = ((uint64_t)SUBSECONDS(now - last_alarm) * US_PER_SEC)
         / RTT_SECOND;

    irq_restore(state);
}
