/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  cpu_atmega_common
 * @{
 *
 * @file
 * @brief   Adds include for missing inttype definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef RIOT_INTTYPES_H_
#define RIOT_INTTYPES_H_

#include_next <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PRIo64  "oll"   /**< Format string for octal 64-bit number */
#define PRIx64  "xll"   /**< Format string for hexadecimal 64-bit number */
#define PRIu64  "ull"   /**< Format string for unsigned 64-bit number */
#define PRId64  "dll"   /**< Format string for signed 64-bit number */

#ifdef __cplusplus
}
#endif

#endif /* RIOT_INTTYPES_H_ */
/** @} */
