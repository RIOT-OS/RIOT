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

#include <reent.h>
#include <errno.h>
#include <sys/time.h>
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
 *          On error, return -1, cast to time_t.
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

/**
 * Current time in seconds since epoch.
 *
 * This call is provided so gettimeofday() works. This is because some code
 * in pkg claims to be written in C99, yet uses some POSIX functions.
 *
 * Note that according to POSIX:
 * "The gettimeofday() function shall return 0 and no value shall be reserved
 * to indicate an error."
 *
 * @param[out]  r    Reentrant structure.
 * @param[out]  tp   Wall time. Because of limitations of the RIOT RTC, the
 *                   tv_usec component will always be zero.
 * @param  tpz       Timezone. MUST be NULL. If it is not, EINVAL will result.
 *
 * @return  0 on success, -1 on failure. The only failure is if tpz is not
 *          null.
 */
int _gettimeofday_r(struct _reent *r, struct timeval *restrict tp,
                    void *restrict tzp)
{
    if (tzp) {
        r->_errno = EINVAL;
        return -1;
    }

    /* The linux docs seem to imply that it is not an error if tp is null, so
     * let's do the same for compatibility .*/
    if (tp) {
        tp->tv_sec = time(NULL);
        tp->tv_usec = 0;
    }

    return 0;
}
