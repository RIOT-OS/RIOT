/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  pkg_lwip_sys
 * @brief       Describes compiler and processor to lwIP
 * @{
 *
 * @file
 * @brief   Compiler/platform abstraction
 * @see     http://www.nongnu.org/lwip/2_1_x/group__compiler__abstraction.html
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef ARCH_CC_H
#define ARCH_CC_H

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "irq.h"
#include "byteorder.h"
#include "mutex.h"

#ifdef MODULE_LOG
#define LOG_LEVEL LOG_INFO
#include "log.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BYTE_ORDER
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#   define BYTE_ORDER  (LITTLE_ENDIAN)  /**< platform's endianness */
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#   define BYTE_ORDER  (BIG_ENDIAN)     /**< platform's endianness */
#else
#   error "Byte order is neither little nor big!"
#endif
#endif

/**
 * @name    (sn)printf formatters for the generic lwIP types
 * @{
 */
#define X8_F    "02" PRIx8
#define U16_F   PRIu16
#define S16_F   PRId16
#define X16_F   PRIx16
#define U32_F   PRIu32
#define S32_F   PRId32
#define X32_F   PRIx32

#define SZT_F   PRIuPTR
/** @} */

/**
 * @name    Compiler hints for packing structures
 * @{
 */
#define PACK_STRUCT_FIELD(x)    x
#define PACK_STRUCT_STRUCT      __attribute__((packed))
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END
/** @} */

/**
 * @name    Platform specific diagnostic output
 * @{
 */
#ifdef MODULE_LOG
#  define LWIP_PLATFORM_DIAG(x)   LOG_INFO x
#  ifdef NDEBUG
#    define LWIP_PLATFORM_ASSERT(x)
#  else
#    define LWIP_PLATFORM_ASSERT(x)     \
        do { \
            LOG_ERROR("Assertion \"%s\" failed at %s:%d\n", x, __FILE__, __LINE__); \
            fflush(NULL); \
            abort(); \
        } while (0)
#  endif
#else
#  define LWIP_PLATFORM_DIAG(x)   printf x
#  ifdef NDEBUG
#    define LWIP_PLATFORM_ASSERT(x)
#  else
#    define LWIP_PLATFORM_ASSERT(x)     \
        do { \
            printf("Assertion \"%s\" failed at %s:%d\n", x, __FILE__, __LINE__); \
            fflush(NULL); \
            abort(); \
        } while (0)
#  endif
#endif
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* ARCH_CC_H */
/** @} */
