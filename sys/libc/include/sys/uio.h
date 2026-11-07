/*
 * SPDX-FileCopyrightText: 2015 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @addtogroup  posix
 * @{
 */

/**
 * @file
 * @brief   libc header for scatter/gather I/O
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 */

#include <stdlib.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Structure for scatter/gather I/O.
 */
struct iovec {
    void *iov_base;     /**< Pointer to data.   */
    size_t iov_len;     /**< Length of data.    */
};

#ifdef __cplusplus
}
#endif
/** @} */
