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
 * @file    posix_time.c
 * @brief   Implementation of basic time functionality.
 * @author  Christian Mehlis <mehlis@inf.fuberlin.de>
 * @}
 */

#include "swtimer.h"

#include "time.h"

typedef swtimer_t clockid_t;

clock_t clock(void)
{
    return NULL; /* TODO */
}

time_t time(time_t *timer)
{
    return NULL; /* TODO */
}

double difftime(time_t time1, time_t time0)
{
    return 0; /* TODO */
}

time_t mktime(struct tm *tp)
{
    return NULL; /* TODO */
}

size_t strftime(char * s, size_t maxsize, const char * format,
        const struct tm * tp)
{
    return NULL; /* TODO */
}

char *strptime(const char * s, const char * fmt, struct tm *tp)
{
    return NULL; /* TODO */
}

char *strptime_l(const char * s, const char * fmt, struct tm *tp, locale_t loc)
{
    return NULL; /* TODO */
}

struct tm *gmtime_r(const time_t * timer, struct tm * tp)
{
    return NULL; /* TODO */
}

struct tm *localtime_r(const time_t * timer, struct tm * tp)
{
    return 0; /* TODO */
}

char *asctime(const struct tm *tp)
{
    return 0; /* TODO */
}

char *ctime(const time_t *timer)
{
    return 0; /* TODO */
}

char *asctime_r(const struct tm * tp, char * buf)
{
    return 0; /* TODO */
}

char *ctime_r(const time_t * timer, char * buf)
{
    return 0; /* TODO */
}

char *tzname[2]; /* Current timezone names.  */
int daylight; /* If daylight-saving time is ever in use.  */
long int timezone; /* Seconds west of UTC.  */

/* Same as above.  */
char *tzname[2];

void tzset(void)
{
    /* unspported */
}

int nanosleep(const struct timespec *requested_time, struct timespec *remaining)
{
    return -1; /* TODO */
}

int clock_getres(clockid_t clock_id, struct timespec *res)
{
    return -1; /* TODO */
}

int clock_gettime(clockid_t clock_id, struct timespec *tp)
{
    return -1; /* TODO */
}

int clock_settime(clockid_t clock_id, const struct timespec *tp)
{
    return -1;
}

int clock_nanosleep(clockid_t clock_id, int flags, const struct timespec *req,
        struct timespec *rem)
{
    return 1; /* TODO */
}

int clock_getcpuclockid(pid_t pid, clockid_t *clock_id)
{
    return -1; /* TODO */
}

/*****************************************************************************/
#define POSIX_TIMER_MAX 10
swtimer_t posix_timer[POSIX_TIMER_MAX];

int timer_create(clockid_t clock_id, struct sigevent * evp, timer_t * timerid)
{
    return 1;
}

int timer_delete(timer_t timerid)
{
    swtimer_t *cur = posix_timer[timerid];
    swtimer_remove(cur);
    memset(cur, 0, sizeof(swtimer_t));
    return 1;
}

int timer_settime(timer_t timerid, int flags, const struct itimerspec * value,
        struct itimerspec * ovalue)
{
    swtimer_t *cur = posix_timer[timerid];

    ovalue->it_interval = cur->interval;
    ovalue->it_value = cur->start;

    cur->start = value->it_value;
    swtimer_set(cur, value->it_interval);
    return 1; /* TODO */
}

int timer_gettime(timer_t timerid, struct itimerspec *value)
{
    swtimer_t *cur = posix_timer[timerid];
    return cur->start;
}

int timer_getoverrun(timer_t timerid)
{
    return -1; /* TODO */
}

/*****************************************************************************/

int getdate_err;

struct tm *getdate(const char *string)
{
    return NULL; /* TODO */
}

int getdate_r(const char * string, struct tm * resbufp)
{
    return -1; /* TODO */
}
