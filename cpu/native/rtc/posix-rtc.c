/**
 * Native CPU rtc.h implementation
 *
 * The native rtc implementation uses POSIX system calls to simulate a
 * real-time clock.
 *
 * Setting the clock will be implemented using a delta variable.
 *
 * Copyright (C) 2013 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General Public
 * License. See the file LICENSE in the top level directory for more details.
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

#include "rtc.h"
#include "cpu.h"

#include "native_internal.h"

static int native_rtc_enabled;
static int native_rtc_initialized;

void rtc_init(void)
{
    native_rtc_enabled = 0;
    native_rtc_initialized = 1;
    printf("native rtc initialized\n");
}

void rtc_enable(void)
{
    DEBUG("rtc_enable\n");
    if (native_rtc_initialized == 1) {
        native_rtc_enabled = 1;
    }
    else {
        DEBUG("rtc not initialized, not enabling\n");
    }
}

void rtc_disable(void)
{
    DEBUG("rtc_disable()\n");
    native_rtc_enabled = 0;
}

void rtc_set_localtime(struct tm *localt)
{
    DEBUG("rtc_set_localtime()\n");

    (void)localt; /* not implemented atm */
    printf("setting time not supported.");
}

void rtc_get_localtime(struct tm *localt)
{
    time_t t;

    if (native_rtc_enabled == 1) {
        _native_syscall_enter();
        t = time(NULL);

        if (localtime_r(&t, localt) == NULL) {
            err(EXIT_FAILURE, "rtc_get_localtime: localtime_r");
        }
        _native_syscall_leave();
    }
}

time_t rtc_time(struct timeval *time)
{
    if (native_rtc_enabled == 1) {
        _native_syscall_enter();
        if (gettimeofday(time, NULL) == -1) {
            err(EXIT_FAILURE, "rtc_time: gettimeofday");
        }
        _native_syscall_leave();
    }
    return time->tv_sec;
}
