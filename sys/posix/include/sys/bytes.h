/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

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

#include <stddef.h>

#include "byteorder.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __socklen_t_defined
#if SIZE_MAX < UINT32_MAX
typedef size_t socklen_t;           /**< socket address length */
#else
// Specification calls for at least 32 bits
typedef uint32_t socklen_t;         /**< socket address length */
#endif
#endif

#ifdef __cplusplus
}
#endif

/** @} */
