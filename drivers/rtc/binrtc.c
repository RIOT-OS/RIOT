/*
 * Copyright (C) 2015-2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_rtc
 *
 * @{
 *
 * @file
 * @brief       RTC interface wrapper for use with RTT modules
 *
 * The name binrtc means Binary RTC, i.e. an RTC with a binary time counter
 * instead of different calendar registers.
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include <stdint.h>
#include <time.h>
#include "cpu.h"
#include "rtc.h"
#include "periph/rtt.h"
#include "periph_conf.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static int _binrtc_init(rtc_t *rtc);
static int _binrtc_set_time(rtc_t *rtc, const struct tm *time);
static int _binrtc_get_time(rtc_t *rtc, struct tm *time);
static int _binrtc_set_alarm(rtc_t *rtc, const struct tm *time, rtc_alarm_cb_t cb, void *arg);
static int _binrtc_get_alarm(rtc_t *rtc, struct tm *time);
static void _binrtc_clear_alarm(rtc_t *rtc);

const rtc_ops_t binrtc_ops = {
    .init = _binrtc_init,
    .set_time = _binrtc_set_time,
    .get_time = _binrtc_get_time,
    .set_alarm = _binrtc_set_alarm,
    .get_alarm = _binrtc_get_alarm,
    .clear_alarm = _binrtc_clear_alarm,
};

static int _binrtc_init(rtc_t *rtc)
{
    (void)rtc;
    DEBUG("_binrtc_init: %p\n", (void *)rtc);
    rtt_init();
    return 0;
}

static int _binrtc_set_time(rtc_t *rtc, const struct tm *time)
{
    (void)rtc;
    DEBUG("_binrtc_set_time: %p, %p\n", (void *)rtc, (void *)time);
    struct tm tmp = *time;
    time_t t = mktime(&tmp);
    DEBUG("_binrtc_set_time: -> %lu\n", (unsigned long)t);

    rtt_set_counter((uint32_t)t);

    return 0;
}

static int _binrtc_get_time(rtc_t *rtc, struct tm *time)
{
    (void)rtc;
    DEBUG("_binrtc_get_time: %p, %p\n", (void *)rtc, (void *)time);
    time_t t = (time_t)rtt_get_counter();
    DEBUG("_binrtc_get_time: -> %lu\n", (unsigned long)t);

    gmtime_r(&t, time);

    return 0;
}

static int _binrtc_set_alarm(rtc_t *rtc, const struct tm *time, rtc_alarm_cb_t cb, void *arg)
{
    (void)rtc;
    DEBUG("_binrtc_set_alarm: %p, %p, %p, %p\n", (void *)rtc, (void *)time, (void *)cb, (void *)arg);
    struct tm tmp = *time;
    time_t t = mktime(&tmp);
    DEBUG("_binrtc_set_alarm: -> %lu\n", (unsigned long)t);

    rtt_set_alarm((uint32_t)t, cb, arg);

    return 0;
}

static int _binrtc_get_alarm(rtc_t *rtc, struct tm *time)
{
    (void)rtc;
    DEBUG("_binrtc_get_alarm: %p, %p\n", (void *)rtc, (void *)time);
    time_t t = (time_t)rtt_get_alarm();
    DEBUG("_binrtc_get_alarm: -> %lu\n", (unsigned long)t);

    gmtime_r(&t, time);

    return 0;
}

static void _binrtc_clear_alarm(rtc_t *rtc)
{
    (void)rtc;
    DEBUG("_binrtc_clear_alarm: %p\n", (void *)rtc);
    rtt_clear_alarm();
}
