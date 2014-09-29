/*
 * Copyright (C) 2014 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef MSP430_TYPES_H
#define MSP430_TYPES_H

#include <sys/_types.h>
#include <errno.h>

#ifndef EINVAL
/**
 * @brief defines EINVAL if MSP430 toolchain is too old to provide it itself
 *        via errno.h
 */
#define EINVAL  (28)
#endif

#ifndef EOVERFLOW
/**
 * @brief defines EOVERFLOW if MSP430 toolchain is too old to provide it itself
 *        via errno.h
 */
#define EOVERFLOW   (65)
#endif

/** defining signed type for size_t */
typedef _ssize_t ssize_t;

typedef unsigned long time_t;

struct timespec {
    time_t  tv_sec;   /* Seconds */
    long  tv_nsec;    /* Nanoseconds */
};

/* TODO: remove once msp430 libc supports struct timeval */
struct timeval {
    time_t tv_sec;
    time_t tv_usec;
};

/* TODO: remove once msp430 libc supports clockid_t */
typedef int clockid_t;

#endif /* MSP430_TYPES_H */
