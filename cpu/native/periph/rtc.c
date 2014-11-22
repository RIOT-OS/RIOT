/*
 * Copyright (C) 2013, 2014 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * Native CPU periph/rtc.h implementation
 *
 * The implementation uses POSIX system calls to emulate a real-time
 * clock based on the system clock.
 *
 * @author Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * @ingroup native_cpu
 * @defgroup native_rtc
 * @file
 */

#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

#include "debug.h"

#include "periph/rtc.h"
#include "cpu.h"

#include "native_internal.h"

static int native_rtc_initialized = 0;
static int native_rtc_powered = 0;
static struct tm native_rtc_alarm;
static rtc_alarm_cb_t native_rtc_alarm_callback;
static void *native_rtc_alarm_argument;

void rtc_init(void)
{
    DEBUG("rtc_init\n");

    memset(&native_rtc_alarm, 0, sizeof(native_rtc_alarm));
    native_rtc_alarm_callback = NULL;
    native_rtc_alarm_argument = NULL;

    native_rtc_initialized = 1;
    printf("Native RTC initialized.\n");

    rtc_poweron();
}

void rtc_poweron(void)
{
    DEBUG("rtc_poweron\n");

    if (!native_rtc_initialized) {
        warnx("rtc_poweron: not initialized");
        return;
    }

    native_rtc_powered = 1;
}

void rtc_poweroff(void)
{
    DEBUG("rtc_poweroff()\n");

    if (!native_rtc_initialized) {
        warnx("rtc_poweroff: not initialized");
    }
    if (!native_rtc_powered) {
        warnx("rtc_poweroff: not powered on");
    }

    native_rtc_powered = 0;
}

/* TODO: implement time setting using a delta */
int rtc_set_time(struct tm *ttime)
{
    (void) ttime;

    DEBUG("rtc_set_time()\n");

    if (!native_rtc_initialized) {
        warnx("rtc_set_time: not initialized");
        return -1;
    }
    if (!native_rtc_powered) {
        warnx("rtc_set_time: not powered on");
        return -1;
    }

    warnx("rtc_set_time: not implemented");

    return -1;
}

int rtc_get_time(struct tm *ttime)
{
    time_t t;

    if (!native_rtc_initialized) {
        warnx("rtc_get_time: not initialized");
        return -1;
    }
    if (!native_rtc_powered) {
        warnx("rtc_get_time: not powered on");
        return -1;
    }

    _native_syscall_enter();
    t = time(NULL);

    if (localtime_r(&t, ttime) == NULL) {
        err(EXIT_FAILURE, "rtc_get_time: localtime_r");
    }
    _native_syscall_leave();

    return 0;
}

/* TODO: implement alarm scheduling */
int rtc_set_alarm(struct tm *time, rtc_alarm_cb_t cb, void *arg)
{
    (void) time;
    (void) cb;
    (void) arg;

    if (!native_rtc_initialized) {
        warnx("rtc_set_alarm: not initialized");
        return -1;
    }
    if (!native_rtc_powered) {
        warnx("rtc_set_alarm: not powered on");
        return -1;
    }

    memcpy(&native_rtc_alarm, time, sizeof(native_rtc_alarm));

    warnx("rtc_set_alarm: not implemented");

    return -1;
}

int rtc_get_alarm(struct tm *time)
{
    (void) time;

    if (!native_rtc_initialized) {
        warnx("rtc_get_alarm: not initialized");
        return -1;
    }
    if (!native_rtc_powered) {
        warnx("rtc_get_alarm: not powered on");
        return -1;
    }

    memcpy(time, &native_rtc_alarm, sizeof(native_rtc_alarm));

    return 0;
}

/* TODO: implement alarm unscheduling once rtc_set_alarm is
 * implemented */
void rtc_clear_alarm(void)
{
    DEBUG("rtc_clear_alarm()\n");

    if (!native_rtc_initialized) {
        warnx("rtc_clear_alarm: not initialized");
    }
    if (!native_rtc_powered) {
        warnx("rtc_clear_alarm: not powered on");
    }

    memset(&native_rtc_alarm, 0, sizeof(native_rtc_alarm));
}
