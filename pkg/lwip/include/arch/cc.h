/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    lwip_arch_cc    Compiler and processor description
 * @ingroup     lwip
 * @brief       Describes compiler and processor to lwIP
 * @{
 *
 * @file
 * @brief   Compiler and processor definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef LWIP_ARCH_CC_H
#define LWIP_ARCH_CC_H

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "irq.h"
#include "byteorder.h"
#include "mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BYTE_ORDER
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#   define BYTE_ORDER  (LITTLE_ENDIAN)  /**< platform's endianess */
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#   define BYTE_ORDER  (BIG_ENDIAN)     /**< platform's endianess */
#else
#   error "Byte order is neither little nor big!"
#endif
#endif

/**
 * @brief   Generic types for lwIP
 * @{
 */
typedef uint8_t u8_t;                   /**< unsigned 8-bit type */
typedef int8_t  s8_t;                   /**< signed 8-bit type */
typedef uint16_t u16_t;                 /**< unsigned 16-bit type */
typedef int16_t  s16_t;                 /**< signed 16-bit type */
typedef uint32_t u32_t;                 /**< unsigned 32-bit type */
typedef int32_t  s32_t;                 /**< signed 32-bit type */

typedef unsigned long mem_ptr_t;        /**< A generic pointer type. It has to be an integer type
                                         *   (not void*, due to some pointer arithmetics). */
/**
 * @}
 */

/**
 * @brief   (sn)printf formatters for the generic lwIP types
 * @{
 */
#define X8_F    "02" PRIx8
#define U16_F   PRIu16
#define S16_F   PRId16
#define X16_F   PRIx16
#define U32_F   PRIu32
#define S32_F   PRId32
#define X32_F   PRIx32

#define SZT_F   "lu"
/**
 * @}
 */

/**
 * @brief   Compiler hints for packing structures
 * @{
 */
#define PACK_STRUCT_FIELD(x)    x
#define PACK_STRUCT_STRUCT      __attribute__((packed))
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END
/**
 * @}
 */

/**
 * @todo check for best value
 */
#define LWIP_CHKSUM_ALGORITHM   (3)

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

#define SYS_ARCH_PROTECT(x)         mutex_lock(&x)
#define SYS_ARCH_UNPROTECT(x)       mutex_unlock(&x)
#define SYS_ARCH_DECL_PROTECT(x)    mutex_t x = MUTEX_INIT

#ifdef __cplusplus
}
#endif

#endif /* LWIP_ARCH_CC_H */
/** @} */
