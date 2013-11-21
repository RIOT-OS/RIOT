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
#include "sys/time.h"

int gettimeofday(struct timeval * tv, timezone_ptr_t tz)
{
    (void) tz;

    swtime_t ticks = swtimer_now();
    tv->tv_sec = HWTIMER_TICKS_TO_US(ticks) / (1000000L);
    tv->tv_usec = HWTIMER_TICKS_TO_US(ticks) % (1000000L);
    return 0;
}

int settimeofday(const struct timeval *tv, const struct timezone *tz)
{
    (void) tv;
    (void) tz;

    return -1; /* not supported */
}

int adjtime(const struct timeval *delta, struct timeval *olddelta)
{
    (void) delta;
    (void) olddelta;

    return -1; /* not supported */
}

#define POSIX_ITIMER_MAX 10
swtimer_t posix_itimer[POSIX_ITIMER_MAX];

int getitimer(itimer_which_t which, struct itimerval *value)
{
    swtimer_t *cur = posix_itimer[which];
    value->it_interval = cur->interval;
    value->it_value = cur->start;
    return 0;
}

int setitimer(itimer_which_t which, const struct itimerval *new,
        struct itimerval *old)
{
    swtimer_t *cur = posix_itimer[which];

    old->it_interval = cur->interval;
    old->it_value = cur->start;

    cur->interval = new->it_interval;
    cur->start = new->it_value;
    swtimer_restart(cur);

    return 0;
}
