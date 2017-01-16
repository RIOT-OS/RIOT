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

/** defining signed type for size_t */
#include "kernel_types.h"

#ifdef __cplusplus
extern "C" {
#endif

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

/**
 * @brief   Definition of `time_t` for the MSP430
 *
 * @todo    Remove once msp430 libc supports `time_t`
 */
typedef unsigned long time_t;

/**
 * @brief   Definition of `struct timespec` for the MSP430
 *
 * @todo    Remove once msp430 libc supports `struct timespec`
 */
struct timespec {
    time_t  tv_sec;     /**< seconds */
    long  tv_nsec;      /**< nanoseconds */
};

/**
 * @brief   Definition of `struct timeval` for the MSP430
 *
 * @todo    Remove once msp430 libc supports struct timeval
 */
struct timeval {
    time_t tv_sec;      /**< seconds */
    time_t tv_usec;     /**< microseconds */
};

/**
 * @brief   Definition of `clockid_t` for the MSP430
 *
 * @todo    Remove once msp430 libc supports clockid_t
 */
typedef int clockid_t;

#ifdef __cplusplus
}
#endif

#endif /* MSP430_TYPES_H */
