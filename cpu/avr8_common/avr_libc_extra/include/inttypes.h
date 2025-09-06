/*
 * SPDX-FileCopyrightText: 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @addtogroup  cpu_atmega_common
 * @{
 *
 * @file
 * @brief   Adds include for missing inttype definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include_next <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PRIo64  "llo"   /**< Format string for octal 64-bit number */
#define PRIx64  "llx"   /**< Format string for hexadecimal 64-bit number */
#define PRIu64  "llu"   /**< Format string for unsigned 64-bit number */
#define PRId64  "lld"   /**< Format string for signed 64-bit number */

#ifdef __cplusplus
}
#endif

/** @} */
