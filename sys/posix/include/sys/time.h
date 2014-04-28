#ifndef _SYS_TIME_H
#define _SYS_TIME_H	1

#include <time.h>

typedef long int suseconds_t;

/* Macros for converting between `struct timeval' and `struct timespec'.  */
# define TIMEVAL_TO_TIMESPEC(tv, ts) {                              \
    (ts)->tv_sec = (tv)->tv_sec;                                    \
    (ts)->tv_nsec = (tv)->tv_usec * 1000;                           \
}
# define TIMESPEC_TO_TIMEVAL(tv, ts) {                              \
    (tv)->tv_sec = (ts)->tv_sec;                                    \
    (tv)->tv_usec = (ts)->tv_nsec / 1000;                           \
}

/* Structure crudely representing a timezone.
 This is obsolete and should never be used.  */
struct timezone {
    int tz_minuteswest; /* Minutes west of GMT.  */
    int tz_dsttime; /* Nonzero if DST is ever in effect.  */
};

typedef struct timezone * timezone_ptr_t;

struct timeval {
    uint32_t tv_sec;
    uint32_t tv_usec;
};

/* Get the current time of day and timezone information,
 putting it into *TV and *TZ.  If TZ is NULL, *TZ is not filled.
 Returns 0 on success, -1 on errors.
 NOTE: This form of timezone information is obsolete.
 Use the functions and variables declared in <time.h> instead.  */
int gettimeofday(struct timeval * tv, timezone_ptr_t tz);

/* Set the current time of day and timezone information.
 This call is ed to the super-user.  */
int settimeofday(const struct timeval *tv, const struct timezone *tz);

/* Adjust the current time of day by the amount in DELTA.
 If OLDDELTA is not NULL, it is filled in with the amount
 of time adjustment remaining to be done from the last `adjtime' call.
 This call is ed to the super-user.  */
int adjtime(const struct timeval *delta, struct timeval *olddelta);

/* Values for the first argument to `getitimer' and `setitimer'.  */
enum itimer_which {
    /* Timers run in real time.  */
    ITIMER_REAL = 0,
#define ITIMER_REAL ITIMER_REAL
    /* Timers run only when the process is executing.  */
    ITIMER_VIRTUAL = 1,
#define ITIMER_VIRTUAL ITIMER_VIRTUAL
    /* Timers run when the process is executing and when
     the system is executing on behalf of the process.  */
    ITIMER_PROF = 2
#define ITIMER_PROF ITIMER_PROF
};

/* Type of the second argument to `getitimer' and
 the second and third arguments `setitimer'.  */
struct itimerval {
    /* Value to put into `it_value' when the timer expires.  */
    struct timeval it_interval;
    /* Time to the next timer expiration.  */
    struct timeval it_value;
};

typedef int itimer_which_t;

/* Set *VALUE to the current setting of timer WHICH.
 Return 0 on success, -1 on errors.  */
int getitimer(itimer_which_t which, struct itimerval *value);

/* Set the timer WHICH to *NEW.  If OLD is not NULL,
 set *OLD to the old value of timer WHICH.
 Returns 0 on success, -1 on errors.  */
int setitimer(itimer_which_t which, const struct itimerval * new,
        struct itimerval * old);

#endif /* sys/time.h */
