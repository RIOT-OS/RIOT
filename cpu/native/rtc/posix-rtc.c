/**
 * Native CPU rtc.h implementation
 *
 * The native rtc implementation uses POSIX system calls to simulate a
 * real-time clock.
 *
 * Setting the clock will be implemented using a delta variable.
 *
 * Copyright (C) 2013 Ludwig Ortmann
 *
 * This file subject to the terms and conditions of the GNU General Public
 * License. See the file LICENSE in the top level directory for more details.
 *
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * @ingroup native_cpu
 * @ingroup rtc
 * @file
 */

#include <time.h>
#include <err.h>

#include "debug.h"

#include <rtc.h>

static int native_rtc_enabled;

void rtc_init(void)
{
    native_rtc_enabled = 0;
    printf("native rtc initialized\n");
}

void rtc_enable(void)
{
    DEBUG("rtc_enable\n");
    native_rtc_enabled = 1;
}

void rtc_disable(void)
{
    DEBUG("rtc_disable()\n");
    native_rtc_enabled = 0;
}

void rtc_set_localtime(struct tm *localt)
{
    DEBUG("rtc_set_localtime()\n");
    printf("setting time not supported.");
}

void rtc_get_localtime(struct tm *localt)
{
    time_t t;

    if(native_rtc_enabled == 1) {
        t = time(NULL);

        if(localtime_r(&t, localt) == NULL) {
            err(1, "rtc_get_localtime: localtime_r");
        }
    }
}
