/**
 * Native CPU periph/rtc.h implementation
 *
 * The native rtc implementation uses POSIX system calls to simulate a
 * real-time clock.
 *
 * Setting the clock will be implemented using a delta variable.
 *
 * Copyright (C) 2013 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * @ingroup native_cpu
 * @ingroup rtc
 * @file
 */

#include <time.h>
#include <stdlib.h>
#include <err.h>

#include "debug.h"

#include "periph/rtc.h"
#include "cpu.h"

#include "native_internal.h"

static int native_rtc_powered;
static int native_rtc_initialized;

void rtc_init(void)
{
    native_rtc_powered = 0;
    native_rtc_initialized = 1;
    printf("Native RTC initialized.\n");
    rtc_poweron();
}

void rtc_poweron(void)
{
    DEBUG("rtc_poweron\n");
    if (native_rtc_initialized == 1) {
        native_rtc_powered = 1;
    }
    else {
        DEBUG("rtc not initialized, not powering on\n");
    }
}

void rtc_poweroff(void)
{
    DEBUG("rtc_poweroff()\n");
    native_rtc_powered = 0;
}

int rtc_set_time(struct tm *ttime)
{
    DEBUG("rtc_set_time()\n");

    (void)ttime; /* not implemented atm */
    printf("setting time not supported.");

    return -1;
}

int rtc_get_time(struct tm *ttime)
{
    time_t t;

    if (native_rtc_powered == 1) {
        _native_syscall_enter();
        t = time(NULL);

        if (localtime_r(&t, ttime) == NULL) {
            err(EXIT_FAILURE, "rtc_get_time: localtime_r");
        }
        _native_syscall_leave();
    }

    return 0;
}

int rtc_set_alarm(struct tm *time, rtc_alarm_cb_t cb, void *arg)
{
    (void) time;
    (void) cb;
    (void) arg;

    warnx("rtc_set_alarm: not implemeted");

    return -1;
}

int rtc_get_alarm(struct tm *time)
{
    (void) time;

    warnx("rtc_get_alarm: not implemeted");

    return -1;
}

void rtc_clear_alarm(void)
{
    warnx("rtc_clear_alarm: not implemeted");
}
