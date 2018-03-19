/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  posix_sockets
 */
/**
 * @{
 *
 * @file
 * @brief   System-internal byte operations.
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef SYS_BYTES_H
#define SYS_BYTES_H

#include <stddef.h>

#include "byteorder.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MACH__
typedef size_t socklen_t;           /**< socket address length */
#else
/* Defined for OSX with a different type */
typedef __darwin_socklen_t socklen_t;   /**< socket address length */
#endif

#ifdef __cplusplus
}
#endif

#endif /* SYS_BYTES_H */
/** @} */
