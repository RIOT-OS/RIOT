/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/* If building on native we need to use the system libraries instead */
#ifdef CPU_NATIVE
#pragma GCC system_header
/* without the GCC pragma above #include_next will trigger a pedantic error */
#include_next <sys/time.h>
#else
#ifndef SYS_TIME_H_
#define SYS_TIME_H_

#include <sys/types.h> /* for time_t, suseconds_t */

#ifdef __cplusplus
extern "C" {
#endif

struct timeval {
    time_t         tv_sec;
    suseconds_t    tv_usec;
};

#ifdef __cplusplus
}
#endif

#endif /* SYS_TIME_H_ */

#endif /* CPU_NATIVE */
