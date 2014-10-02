/*
 * Copyright (C) 2014 Oliver Hahm
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef KERNEL_TYPES_H
#define KERNEL_TYPES_H

#include <stdint.h>
#include <inttypes.h>
#include <limits.h>

#ifndef NATIVE_INCLUDES
#   include <stddef.h>
#   include <sys/types.h>

#   ifndef SSIZE_MAX
#       ifdef _POSIX_SSIZE_MAX
#           define SSIZE_MAX _POSIX_SSIZE_MAX
#       else
#           define SSIZE_MAX ((ssize_t) (SIZE_MAX / 2))
#       endif
#   endif

#   if defined (MODULE_MSP430_COMMON) || defined (MODULE_ATMEGA_COMMON)
        typedef signed ssize_t;
#   endif
#endif

/**
 * @def MAXTHREADS
 * @brief The maximum number of threads to be scheduled
 */
#ifndef MAXTHREADS
#define MAXTHREADS 32
#endif

/**
 * Canonical identifier for an invalid PID.
 */
#define KERNEL_PID_UNDEF 0

/**
 * The first valid PID (inclusive).
 */
#define KERNEL_PID_FIRST (KERNEL_PID_UNDEF + 1)

/**
 * The last valid PID (inclusive).
 */
#define KERNEL_PID_LAST (KERNEL_PID_FIRST + MAXTHREADS - 1)

/**
 * Macro for printing formatter
 */
#define PRIkernel_pid PRIi16

/**
 * Unique process identifier
 */
typedef int16_t kernel_pid_t;

#endif /* KERNEL_TYPES_H */
