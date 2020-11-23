/*
 * Copyright (C) 2014 Oliver Hahm
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_util
 * @{
 *
 * @file
 * @brief       Types used by the kernel
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 */

#ifndef KERNEL_TYPES_H
#define KERNEL_TYPES_H

#include <stdint.h>
#include <inttypes.h>
#include <limits.h>

#ifndef NATIVE_INCLUDES
#   include <stddef.h>
#   include <sys/types.h>

/**
 * @brief Maximum value for ssize_t
 */
#   ifndef SSIZE_MAX
#       ifdef _POSIX_SSIZE_MAX
#           define SSIZE_MAX _POSIX_SSIZE_MAX
#       else
#           define SSIZE_MAX ((ssize_t)(SIZE_MAX / 2))
#       endif
#   endif

#   ifdef MODULE_MSP430_COMMON
typedef signed ssize_t;
#   endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* KERNEL_TYPES_H */
/** @} */
