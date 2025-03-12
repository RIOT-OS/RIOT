/*
 * Copyright (C) 2025 carl-tud
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @brief   timer.h-like implementation for macOS
 * @ingroup drivers_timer_darwin
 * @ingroup cpu_native
 * @author  carl-tud
 */

#include "modules.h"

#if IS_USED(MODULE_PERIPH_TIMER_DARWIN)
#  include "timer_darwin.h"

/* These macros convert between itimerspec and itimerval. */

#  if !defined(TIMEVAL_TO_TIMESPEC)
#   define TIMEVAL_TO_TIMESPEC(tv, ts) do {    \
         (ts)->tv_sec = (tv)->tv_sec;          \
         (ts)->tv_nsec = (tv)->tv_usec * 1000; \
     } while (0)
#  endif

#  if !defined(TIMESPEC_TO_TIMEVAL)
#   define TIMESPEC_TO_TIMEVAL(tv, ts) do {    \
         (tv)->tv_sec = (ts)->tv_sec;          \
         (tv)->tv_usec = (ts)->tv_nsec / 1000; \
     } while (0)
#  endif

#  if !defined(ITIMERVAL_TO_ITIMERSPEC)
#   define ITIMERVAL_TO_ITIMERSPEC(tv, ts) do {                  \
         TIMEVAL_TO_TIMESPEC(&(tv)->it_interval, &(ts)->it_interval); \
         TIMEVAL_TO_TIMESPEC(&(tv)->it_value, &(ts)->it_value);       \
     } while (0)
#  endif

#  if !defined(ITIMERSPEC_TO_ITIMERVAL)
#   define ITIMERSPEC_TO_ITIMERVAL(tv, ts) do {                  \
         TIMESPEC_TO_TIMEVAL(&(tv)->it_interval, &(ts)->it_interval); \
         TIMESPEC_TO_TIMEVAL(&(tv)->it_value, &(ts)->it_value);       \
     } while (0)
#  endif

int timer_settime(timer_t tim, int flags, const struct itimerspec* new, struct itimerspec* old) {
    (void)flags;
    struct itimerval _old;
    struct itimerval _new;
    ITIMERSPEC_TO_ITIMERVAL(&_new, new);
    int res = setitimer(tim, &_new, &_old);
    if (old) {
        ITIMERVAL_TO_ITIMERSPEC(&_old, old);
    }
    return res;
}

int timer_delete(timer_t tim) {
    struct itimerval new = {
        .it_value = {
            .tv_sec = 0,
            .tv_usec = 0
        }
    };
    return setitimer(tim, &new, NULL);
}

#endif
