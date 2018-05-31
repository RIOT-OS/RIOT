/*
 * Copyright (C) 2018 FU Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_newlib_syscalls_riot
 * @{
 *
 * @file
 * @brief       Implement standard C's time() using the RTC.
 *
 * @author      Juan Carrano <j.carrano@fu-berlin.de>
 *
 * @}
 */

#include <time.h>

#include "periph/rtc.h"

/**
 * Seconds since the epoch.
 *
 * This function oveerides the standard libraries time(). It gets the time
 * directly from the RTC. It is compiled automatically if the RTC peripheral
 * is enabled.
 *
 * time() is implemented instead of the lower level _gettimeofday_r because
 * _gettimeofday_r is defined in microseconds, which RIOT's RTC does not provide.
 *
 * @param[out]  tloc    If tloc is non-NULL, the return value is also stored in
 *                  the memory pointed to by tloc. Note that this parameter is
 *                  deprecated and must be set to null in new code.
 *
 * @return  Number of seconds since 1970-01-01 00:00:00 +0000 (UTC).
 */
time_t time(time_t *tloc)
{
    struct tm t;
    time_t r;

    if (rtc_get_time(&t) >= 0) {
        r = mktime(&t);
    } else {
        r = (time_t)(-1);
    }

    if (tloc)
        *tloc = r;

    return r;
}
