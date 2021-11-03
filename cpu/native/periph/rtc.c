/*
 * Copyright (C) 2013, 2014 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_native
 * @ingroup     drivers_periph_rtc
 * @{
 *
 * @file
 * @brief Native CPU periph/rtc.h implementation
 *
 * The implementation uses POSIX system calls to emulate a real-time
 * clock based on the system clock.
 *
 * @author Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * @}
 */

#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

#include "periph/rtc.h"
#include "cpu.h"
#include "timex.h"
#include "ztimer.h"

#include "native_internal.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static int _native_rtc_initialized = 0;
static int _native_rtc_powered = 0;

static struct tm _native_rtc_alarm;
static rtc_alarm_cb_t _native_rtc_alarm_callback;

static time_t _native_rtc_offset;

static ztimer_t _native_rtc_timer;

static void _native_rtc_cb(void *arg) {
    if (_native_rtc_alarm_callback) {
        _native_rtc_alarm_callback(arg);
    }
    _native_rtc_alarm_callback = NULL;
}

/* RIOT does not expect DST or TZ information */
static void _remove_struct_tm_extra( struct tm * t ){
    struct tm tmp = {.tm_year = t->tm_year,
                     .tm_mon = t->tm_mon,
                     .tm_mday = t->tm_mday,
                     .tm_hour = t->tm_hour,
                     .tm_min = t->tm_min,
                     .tm_sec = t->tm_sec,
                     .tm_wday = t->tm_wday
    };
    *t = tmp;
}

void rtc_init(void)
{
    DEBUG("rtc_init\n");

    ztimer_remove(ZTIMER_MSEC, &_native_rtc_timer);
    _native_rtc_timer.callback = _native_rtc_cb;

    memset(&_native_rtc_alarm, 0, sizeof(_native_rtc_alarm));
    _native_rtc_alarm_callback = NULL;

    _native_rtc_offset = 0;

    _native_rtc_initialized = 1;
    printf("Native RTC initialized.\n");

    rtc_poweron();
}

void rtc_poweron(void)
{
    DEBUG("rtc_poweron\n");

    if (!_native_rtc_initialized) {
        warnx("rtc_poweron: not initialized");
        return;
    }

    _native_rtc_powered = 1;
}

void rtc_poweroff(void)
{
    DEBUG("rtc_poweroff()\n");

    if (!_native_rtc_initialized) {
        warnx("rtc_poweroff: not initialized");
    }
    if (!_native_rtc_powered) {
        warnx("rtc_poweroff: not powered on");
    }

    if (_native_rtc_alarm_callback) {
        ztimer_remove(ZTIMER_MSEC, &_native_rtc_timer);
        memset(&_native_rtc_alarm, 0, sizeof(_native_rtc_alarm));
        _native_rtc_alarm_callback = NULL;
    }

    _native_rtc_powered = 0;
}

int rtc_set_time(struct tm *ttime)
{
    DEBUG_PUTS("rtc_set_time()");

    if (!_native_rtc_initialized) {
        warnx("rtc_set_time: not initialized");
        return -1;
    }
    if (!_native_rtc_powered) {
        warnx("rtc_set_time: not powered on");
        return -1;
    }
    /* ensure there is no accidental extra information */
    struct tm itime = *ttime;
    _remove_struct_tm_extra(&itime);

    /* mktime() and localtime are only inverse functions if tm_isdst == -1 */
    itime.tm_isdst = -1;
    time_t tnew = mktime(&itime);

    if (tnew == -1) {
        warnx("rtc_set_time: out of time_t range");
        return -1;
    }
    _native_syscall_enter();
    _native_rtc_offset = tnew - time(NULL);
    _native_syscall_leave();

    if (_native_rtc_alarm_callback) {
        rtc_set_alarm(&_native_rtc_alarm, _native_rtc_alarm_callback,
                _native_rtc_timer.arg);
    }

    return 0;
}

int rtc_get_time(struct tm *ttime)
{
    time_t t;

    if (!_native_rtc_initialized) {
        warnx("rtc_get_time: not initialized");
        return -1;
    }
    if (!_native_rtc_powered) {
        warnx("rtc_get_time: not powered on");
        return -1;
    }

    _native_syscall_enter();
    t = time(NULL) + _native_rtc_offset;

    if (localtime_r(&t, ttime) == NULL) {
        err(EXIT_FAILURE, "rtc_get_time: localtime_r");
    }
    _native_syscall_leave();

    /* RIOT does not handle DST or TZ information */
    _remove_struct_tm_extra(ttime);

    return 0;
}

int rtc_set_alarm(struct tm *time, rtc_alarm_cb_t cb, void *arg)
{
    if (!_native_rtc_initialized) {
        warnx("rtc_set_alarm: not initialized");
        return -1;
    }
    if (!_native_rtc_powered) {
        warnx("rtc_set_alarm: not powered on");
        return -1;
    }

    struct tm now;
    rtc_get_time(&now);

    /* ensure there is no accidental extra information */
    struct tm intime = *time;
    _remove_struct_tm_extra(&intime);

    /* tm_idst are ignored for these mktime calls since
     * both times carry the same (00) timezone information */
    time_t tdiff_secs = mktime(&intime) - mktime(&now);

    if (_native_rtc_alarm_callback) {
        ztimer_remove(ZTIMER_MSEC, &_native_rtc_timer);
    }

    _native_rtc_alarm = *time;
    _native_rtc_alarm_callback = cb;
    _native_rtc_timer.arg = arg;

    if (tdiff_secs >= 0) {
        ztimer_set(ZTIMER_MSEC, &_native_rtc_timer, tdiff_secs * MS_PER_SEC);
    }

    return 0;
}

int rtc_get_alarm(struct tm *time)
{
    if (!_native_rtc_initialized) {
        warnx("rtc_get_alarm: not initialized");
        return -1;
    }
    if (!_native_rtc_powered) {
        warnx("rtc_get_alarm: not powered on");
        return -1;
    }

    *time = _native_rtc_alarm;

    return 0;
}

void rtc_clear_alarm(void)
{
    DEBUG("rtc_clear_alarm()\n");

    if (!_native_rtc_initialized) {
        warnx("rtc_clear_alarm: not initialized");
    }
    if (!_native_rtc_powered) {
        warnx("rtc_clear_alarm: not powered on");
    }

    ztimer_remove(ZTIMER_MSEC, &_native_rtc_timer);
    memset(&_native_rtc_alarm, 0, sizeof(_native_rtc_alarm));
    _native_rtc_alarm_callback = NULL;
}
