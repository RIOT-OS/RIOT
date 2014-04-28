#ifndef	_TIME_H
#define _TIME_H	1

/* Get size_t and NULL from <stddef.h>.  */
#include <stddef.h>

#include "hwtimer.h"
#include "vtimer.h"

/* This defines CLOCKS_PER_SEC, which is the number of processor clock
 ticks per second.  */
#define US_PER_S (1000 * 1000)
#define CLOCKS_PER_SEC HWTIMER_TICKS((US_PER_S))

/* Returned by `time'.  */
typedef long int time_t;

/* POSIX.1b structure for a time value.  This is like a `struct timeval' but
 has nanoseconds instead of microseconds.  */
//struct timespec {
//    time_t tv_sec; /* Seconds.  */
//    time_t tv_nsec; /* Nanoseconds.  */
//};

/* Used by other time functions.  */
struct tm {
    int tm_sec; /* Seconds.	[0-60] (1 leap second) */
    int tm_min; /* Minutes.	[0-59] */
    int tm_hour; /* Hours.	[0-23] */
    int tm_mday; /* Day.		[1-31] */
    int tm_mon; /* Month.	[0-11] */
    int tm_year; /* Year	- 1900.  */
    int tm_wday; /* Day of week.	[0-6] */
    int tm_yday; /* Days in year.[0-365]	*/
    int tm_isdst; /* DST.		[-1/0/1]*/

    long int tm_gmtoff; /* Seconds east of UTC.  */
    const char *tm_zone; /* Timezone abbreviation.  */
};

/* POSIX.1b structure for timer start values and intervals.  */
//struct itimerspec {
//    struct timespec it_interval;
//    struct timespec it_value;
//};

/* We can use a simple forward declaration.  */
struct sigevent;

//typedef uint8_t pid_t;
//typedef uint64_t clock_t;
typedef uint32_t locale_t;
//typedef uint8_t clockid_t;
//typedef vtimer_t timer_t;

/* Time used by the program so far (user time + system time).
 The result / CLOCKS_PER_SECOND is program time in seconds.  */
clock_t clock(void);

/* Return the current time and put it in *TIMER if TIMER is not NULL.  */
time_t time(time_t *timer);

/* Return the difference between TIME1 and TIME0.  */
double difftime(time_t time1, time_t time0);

/* Return the `time_t' representation of TP and normalize TP.  */
time_t mktime(struct tm *tp);

/* Format TP into S according to FORMAT.
 Write no more than MAXSIZE characters and return the number
 of characters written, or 0 if it would exceed MAXSIZE.  */
size_t strftime(char * s, size_t maxsize, const char * format,
        const struct tm * tp);

/* Parse S according to FORMAT and store binary time information in TP.
 The return value is a pointer to the first unparsed character in S.  */
char *strptime(const char * s, const char * fmt, struct tm *tp);

size_t strftime_l(char * s, size_t maxsize, const char * format,
        const struct tm * tp, locale_t loc);/* Return the `struct tm' representation of *TIMER
 in Universal Coordinated Time (aka Greenwich Mean Time).  */

struct tm *gmtime(const time_t *timer);

/* Return the `struct tm' representation
 of *TIMER in the local timezone.  */
struct tm *localtime(const time_t *timer);

/* Return the `struct tm' representation of *TIMER in UTC,
 using *TP to store the result.  */
struct tm *gmtime_r(const time_t * timer, struct tm * tp);

/* Return the `struct tm' representation of *TIMER in local time,
 using *TP to store the result.  */
struct tm *localtime_r(const time_t * timer, struct tm * tp);

/* Return a string of the form "Day Mon dd hh:mm:ss yyyy\n"
 that is the representation of TP in this format.  */
char *asctime(const struct tm *tp);

/* Equivalent to `asctime (localtime (timer))'.  */
char *ctime(const time_t *timer);

/* Reentrant versions of the above functions.  */

/* Return in BUF a string of the form "Day Mon dd hh:mm:ss yyyy\n"
 that is the representation of TP in this format.  */
char *asctime_r(const struct tm * tp, char * buf);

/* Equivalent to `asctime_r (localtime_r (timer, *TMP*), buf)'.  */
char *ctime_r(const time_t * timer, char * buf);

/* Defined in localtime.c.  */
extern char *tzname[2]; /* Current timezone names.  */
extern int daylight; /* If daylight-saving time is ever in use.  */
extern long int timezone; /* Seconds west of UTC.  */

/* Same as above.  */
extern char *tzname[2];

/* Set time conversion information from the TZ environment variable.
 If TZ is not defined, a locale-dependent default is used.  */
void tzset(void);

int daylight;
long int timezone;

/* Nonzero if YEAR is a leap year (every 4 years,
 except every 100th isn't, and every 400th is).  */
# define isleap(year)	\
  ((year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0))

/* Pause execution for a number of nanoseconds.

 This function is a cancellation point and therefore not marked with
 THROW.  */
int nanosleep(const struct timespec *requested_time, struct timespec *remaining);

/* Get resolution of clock CLOCK_ID.  */
int clock_getres(clockid_t clock_id, struct timespec *res);

/* Get current value of clock CLOCK_ID and store it in TP.  */
int clock_gettime(clockid_t clock_id, struct timespec *tp);

/* Set clock CLOCK_ID to value TP.  */
int clock_settime(clockid_t clock_id, const struct timespec *tp);

/* High-resolution sleep with the specified clock.

 This function is a cancellation point and therefore not marked with
 THROW.  */
int clock_nanosleep(clockid_t clock_id, int flags, const struct timespec *req,
        struct timespec *rem);

/* Return clock ID for CPU-time clock.  */
int clock_getcpuclockid(pid_t pid, clockid_t *clock_id);

/* Create new per-process timer using CLOCK_ID.  */
int timer_create(clockid_t clock_id, struct sigevent * evp, timer_t * timerid);

/* Delete timer TIMERID.  */
int timer_delete(timer_t timerid);

/* Set timer TIMERID to VALUE, returning old value in OVALUE.  */
int timer_settime(timer_t timerid, int flags, const struct itimerspec * value,
        struct itimerspec * ovalue);

/* Get current value of timer TIMERID and store it in VALUE.  */
int timer_gettime(timer_t timerid, struct itimerspec *value);

/* Get expiration overrun for timer TIMERID.  */
int timer_getoverrun(timer_t timerid);

/* Set to one of the following values to indicate an error.
 1  the DATEMSK environment variable is null or undefined,
 2  the template file cannot be opened for reading,
 3  failed to get file status information,
 4  the template file is not a regular file,
 5  an error is encountered while reading the template file,
 6  memory allication failed (not enough memory available),
 7  there is no line in the template that matches the input,
 8  invalid input specification Example: February 31 or a time is
 specified that can not be represented in a time_t (representing
 the time in seconds since 00:00:00 UTC, January 1, 1970) */
extern int getdate_err;

/* Parse the given string as a date specification and return a value
 representing the value.  The templates from the file identified by
 the environment variable DATEMSK are used.  In case of an error
 `getdate_err' is set.

 This function is a possible cancellation point and therefore not
 marked with THROW.  */
struct tm *getdate(const char *string);

/* Since `getdate' is not reentrant because of the use of `getdate_err'
 and the static buffer to return the result in, we provide a thread-safe
 variant.  The functionality is the same.  The result is returned in
 the buffer pointed to by RESBUFP and in case of an error the return
 value is != 0 with the same values as given above for `getdate_err'.

 This function is not part of POSIX and therefore no official
 cancellation point.  But due to similarity with an POSIX interface
 or due to the implementation it is a cancellation point and
 therefore not marked with THROW.  */
int getdate_r(const char * string, struct tm * resbufp);

#endif /* <time.h> included.  */
