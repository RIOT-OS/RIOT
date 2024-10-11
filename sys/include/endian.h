/*
 * Copyright (C) 2024 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    sys_endian  endian conversions as provided by most libcs
 * @ingroup     sys
 *
 * This module provides architecture-independent access to architecture details.
 *
 * @{
 *
 * @file
 * @brief       libc header for endian conversion
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 */

#ifndef ENDIAN_H
#define ENDIAN_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DOXYGEN
/**
 * @brief   A numeric constant representing little endian byte order
 */
#define LITTLE_ENDIAN   magic-number

/**
 * @brief   A numeric constant representing big endian byte order
 */
#define BIG_ENDIAN      magic-number

/**
 * @brief   A numeric constant representing PDP endian byte order
 */
#define PDP_ENDIAN      magic-number

/**
 * @brief   The byte order of this machines indicated by the constant
 *          @ref BIG_ENDIAN or @ref LITTLE_ENDIAN
 *
 * @note    This numeric constant is available at preprocessor time, so you
 *          can compare this to @ref BIG_ENDIAN or @ref LITTLE_ENDIAN in
 *          `#if` directives.
 */
#define BYTE_ORDER      <LITTLE_ENDIAN or BIG_ENDIAN>

uint16_t htobe16(uint16_t host_16bits);         /**< host to big endian, 16 bit */
uint16_t htole16(uint16_t host_16bits);         /**< host to little endian, 16 bit */
uint16_t be16toh(uint16_t big_endian_16bits);   /**< big endian to host, 16 bit */
uint16_t le16toh(uint16_t little_endian_16bits);/**< little endian to host, 16 bit */

uint32_t htobe32(uint32_t host_32bits);         /**< host to big endian, 32 bit */
uint32_t htole32(uint32_t host_32bits);         /**< host to little endian, 32 bit */
uint32_t be32toh(uint32_t big_endian_32bits);   /**< big endian to host, 32 bit */
uint32_t le32toh(uint32_t little_endian_32bits);/**< little endian to host, 32 bit */

uint64_t htobe64(uint64_t host_64bits);         /**< host to big endian, 64 bit */
uint64_t htole64(uint64_t host_64bits);         /**< host to little endian, 64 bit */
uint64_t be64toh(uint64_t big_endian_64bits);   /**< big endian to host, 64 bit */
uint64_t le64toh(uint64_t little_endian_64bits);/**< little endian to host, 64 bit */

#else /* DOXYGEN */

/* Depending on the version of newlib used, newlib may provide them indirectly
 * as well. We don't want to redefine them in this case */
#ifndef LITTLE_ENDIAN
#  define LITTLE_ENDIAN 1234
#endif
#ifndef BIG_ENDIAN
#  define BIG_ENDIAN    4321
#endif
#ifndef PDP_ENDIAN
#  define PDP_ENDIAN    3412
#endif
#ifndef BYTE_ORDER
#  define BYTE_ORDER    __BYTE_ORDER__
#endif

/* But to avoid lots of pain in the ass: Let's at least make sure everyone
 * agrees on what magic number is what */
#if (LITTLE_ENDIAN != 1234) || (BIG_ENDIAN != 4321) || (PDP_ENDIAN != 3412)
#  error "Mismatching magic numbers to refer to endianness"
#endif

#if BYTE_ORDER == LITTLE_ENDIAN
#  define htobe16(_x) __builtin_bswap16(_x)
#  define htole16(_x) ((uint16_t)(_x))
#  define be16toh(_x) __builtin_bswap16(_x)
#  define le16toh(_x) ((uint16_t)(_x))
#  define htobe32(_x) __builtin_bswap32(_x)
#  define htole32(_x) ((uint32_t)(_x))
#  define be32toh(_x) __builtin_bswap32(_x)
#  define le32toh(_x) ((uint32_t)(_x))
#  define htobe64(_x) __builtin_bswap64(_x)
#  define htole64(_x) ((uint64_t)(_x))
#  define be64toh(_x) __builtin_bswap64(_x)
#  define le64toh(_x) ((uint64_t)(_x))
#elif BYTE_ORDER == BIG_ENDIAN
#  define htole16(_x) __builtin_bswap16(_x)
#  define htobe16(_x) ((uint16_t)(_x))
#  define le16toh(_x) __builtin_bswap16(_x)
#  define be16toh(_x) ((uint16_t)(_x))
#  define htole32(_x) __builtin_bswap32(_x)
#  define htobe32(_x) ((uint32_t)(_x))
#  define le32toh(_x) __builtin_bswap32(_x)
#  define be32toh(_x) ((uint32_t)(_x))
#  define htole64(_x) __builtin_bswap64(_x)
#  define htobe64(_x) ((uint64_t)(_x))
#  define le64toh(_x) __builtin_bswap64(_x)
#  define be64toh(_x) ((uint64_t)(_x))
#else
#  error "Byte order not supported"
#endif

#endif /* DOXYGEN */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* ENDIAN_H */
