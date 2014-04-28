/**
 * POSIX implementation of basic time operations.
 *
 * Copyright (C) 2013, Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup posix
 * @{
 * @file    posix_sys_time.c
 * @brief   Implementation of basic time functionality.
 * @author  Christian Mehlis <mehlis@inf.fuberlin.de>
 * @}
 */
#include <sys/time.h>

int gettimeofday(struct timeval *tv, timezone_ptr_t tz)
{
    /*
     * The use of the timezone structure is obsolete;
     * the tz argument should normally be specified as NULL.
     */
    (void) tz;

    timex_t now;
    vtimer_now(&now);

    tv->tv_sec = now.seconds;
    tv->tv_usec = now.microseconds;

    return 0;
}

int settimeofday(const struct timeval *tv, const struct timezone *tz)
{
    return 0; /* TODO */
}

int adjtime(const struct timeval *delta, struct timeval *olddelta)
{
    return 0; /* TODO */
}

int getitimer(itimer_which_t which, struct itimerval *value)
{
    return 0; /* TODO */
}

int setitimer(itimer_which_t which, const struct itimerval *new,
              struct itimerval *old)
{
    return 0; /* TODO */
}
