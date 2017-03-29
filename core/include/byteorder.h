/*
 * Copyright (C) 2014 René Kijewski
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup     core_util
 * @{
 *
 * @file
 * @brief          Functions to work with different byte orders.
 *
 * @author         René Kijewski <rene.kijewski@fu-berlin.de>
 */

#ifndef BYTEORDER_H
#define BYTEORDER_H

#include <stdint.h>

#if defined(__MACH__)
#   include "clang_compat.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* ******************************* INTERFACE ******************************* */


/**
 * @brief          A 16 bit integer in little endian.
 * @details        This is a wrapper around an uint16_t to catch missing conversions
 *                 between different byte orders at compile time.
 */
typedef union __attribute__((packed)) {
    uint16_t    u16;    /**< 16 bit representation */
    uint8_t      u8[2]; /**< 8 bit representation */
} le_uint16_t;

/**
 * @brief          A 32 bit integer in little endian.
 * @details        This is a wrapper around an uint32_t to catch missing conversions
 *                 between different byte orders at compile time.
 */
typedef union __attribute__((packed)) {
    uint32_t    u32;    /**< 32 bit representation */
    uint8_t      u8[4]; /**< 8 bit representation */
    uint16_t    u16[2]; /**< 16 bit representation */
    le_uint16_t l16[2]; /**< little endian 16 bit representation */
} le_uint32_t;

/**
 * @brief          A 64 bit integer in little endian.
 * @details        This is a wrapper around an uint64_t to catch missing conversions
 *                 between different byte orders at compile time.
 */
typedef union __attribute__((packed)) {
    uint64_t    u64;    /**< 64 bit representation */
    uint8_t      u8[8]; /**< 8 bit representation */
    uint16_t    u16[4]; /**< 16 bit representation */
    uint32_t    u32[2]; /**< 32 bit representation */
    le_uint16_t l16[4]; /**< little endian 16 bit representation */
    le_uint32_t l32[2]; /**< little endian 32 bit representation */
} le_uint64_t;

/**
 * @brief          A 16 bit integer in big endian aka network byte order.
 * @details        This is a wrapper around an uint16_t to catch missing conversions
 *                 between different byte orders at compile time.
 */
typedef union __attribute__((packed)) {
    uint16_t    u16;    /**< 16 bit representation */
    uint8_t      u8[2]; /**< 8 bit representation */
} be_uint16_t;

/**
 * @brief          A 32 bit integer in big endian aka network byte order.
 * @details        This is a wrapper around an uint32_t to catch missing conversions
 *                 between different byte orders at compile time.
 */
typedef union __attribute__((packed)) {
    uint32_t    u32;    /**< 32 bit representation */
    uint8_t      u8[4]; /**< 8 bit representation */
    uint16_t    u16[2]; /**< 16 bit representation */
    be_uint16_t b16[2]; /**< big endian 16 bit representation */
} be_uint32_t;

/**
 * @brief          A 64 bit integer in big endian aka network byte order.
 * @details        This is a wrapper around an uint64_t to catch missing conversions
 *                 between different byte orders at compile time.
 */
typedef union __attribute__((packed)) {
    uint64_t    u64;    /**< 64 bit representation */
    uint8_t      u8[8]; /**< 8 bit representation */
    uint16_t    u16[4]; /**< 16 bit representation */
    uint32_t    u32[2]; /**< 32 bit representation */
    be_uint16_t b16[4]; /**< big endian 16 bit representation */
    be_uint32_t b32[2]; /**< big endian 32 bit representation */
} be_uint64_t;

/**
 * @brief A 16 bit integer in network byte order.
 */
typedef be_uint16_t network_uint16_t;

/**
 * @brief A 32 bit integer in network byte order.
 */
typedef be_uint32_t network_uint32_t;

/**
 * @brief A 64 bit integer in network byte order.
 */
typedef be_uint64_t network_uint64_t;

/**
 * @brief          Convert from little endian to big endian, 16 bit.
 * @param[in]      v   The integer in little endian.
 * @returns        `v` converted to big endian.
 */
static inline be_uint16_t byteorder_ltobs(le_uint16_t v);

/**
 * @brief          Convert from little endian to big endian, 32 bit.
 * @param[in]      v   The integer in little endian.
 * @returns        `v` converted to big endian.
 */
static inline be_uint32_t byteorder_ltobl(le_uint32_t v);

/**
 * @brief          Convert from little endian to big endian, 64 bit.
 * @param[in]      v   The integer in little endian.
 * @returns        `v` converted to big endian.
 */
static inline be_uint64_t byteorder_ltobll(le_uint64_t v);

/**
 * @brief          Convert from big endian to little endian, 16 bit.
 * @param[in]      v   The integer in big endian.
 * @returns        `v` converted to little endian.
 */
static inline le_uint16_t byteorder_btols(be_uint16_t v);

/**
 * @brief          Convert from big endian to little endian, 32 bit.
 * @param[in]      v   The integer in big endian.
 * @returns        `v` converted to little endian.
 */
static inline le_uint32_t byteorder_btoll(be_uint32_t v);

/**
 * @brief          Convert from big endian to little endian, 64 bit.
 * @param[in]      v   The integer in big endian.
 * @returns        `v` converted to little endian.
 */
static inline le_uint64_t byteorder_btolll(be_uint64_t v);

/**
 * @brief          Convert from host byte order to network byte order, 16 bit.
 * @param[in]      v   The integer in host byte order.
 * @returns        `v` converted to network byte order.
 */
static inline network_uint16_t byteorder_htons(uint16_t v);

/**
 * @brief          Convert from host byte order to network byte order, 32 bit.
 * @param[in]      v   The integer in host byte order.
 * @returns        `v` converted to network byte order.
 */
static inline network_uint32_t byteorder_htonl(uint32_t v);

/**
 * @brief          Convert from host byte order to network byte order, 64 bit.
 * @param[in]      v   The integer in host byte order.
 * @returns        `v` converted to network byte order.
 */
static inline network_uint64_t byteorder_htonll(uint64_t v);

/**
 * @brief          Convert from network byte order to host byte order, 16 bit.
 * @param[in]      v   The integer in network byte order.
 * @returns        `v` converted to host byte order.
 */
static inline uint16_t byteorder_ntohs(network_uint16_t v);

/**
 * @brief          Convert from network byte order to host byte order, 32 bit.
 * @param[in]      v   The integer in network byte order.
 * @returns        `v` converted to host byte order.
 */
static inline uint32_t byteorder_ntohl(network_uint32_t v);

/**
 * @brief          Convert from network byte order to host byte order, 64 bit.
 * @param[in]      v   The integer in network byte order.
 * @returns        `v` converted to host byte order.
 */
static inline uint64_t byteorder_ntohll(network_uint64_t v);

/**
 * @brief          Swap byte order, 16 bit.
 * @param[in]      v   The integer to swap.
 * @returns        The swapped integer.
 */
static inline uint16_t byteorder_swaps(uint16_t v);

/**
 * @brief          Swap byte order, 32 bit.
 * @param[in]      v   The integer to swap.
 * @returns        The swapped integer.
 */
static inline uint32_t byteorder_swapl(uint32_t v);

/**
 * @brief          Swap byte order, 64 bit.
 * @param[in]      v   The integer to swap.
 * @returns        The swapped integer.
 */
static inline uint64_t byteorder_swapll(uint64_t v);


/* **************************** IMPLEMENTATION ***************************** */
/**
 * @brief   Compile time byte-order swapping for 16 bit constants
 */
#define CONST_BSWAP16(x)     ((uint16_t)(                        \
                             (((uint16_t)(x) >> 8) & 0x00FF)   | \
                             (((uint16_t)(x) << 8) & 0xFF00)))

/**
 * @brief   Compile time byte-order swapping for 32 bit constants
 */
#define CONST_BSWAP32(x)     ((uint32_t)(                             \
                             (((uint32_t)(x) >> 24) & 0x000000FF)   | \
                             (((uint32_t)(x) >> 8)  & 0x0000FF00)   | \
                             (((uint32_t)(x) << 8)  & 0x00FF0000)   | \
                             (((uint32_t)(x) << 24) & 0xFF000000)))

/**
 * @brief   Compile time byte-order swapping for 64 bit constants
 */
#define CONST_BSWAP64(x)     ((uint64_t)(                                   \
                             (((uint64_t)(x) >> 56) & 0x00000000000000FF) | \
                             (((uint64_t)(x) >> 40) & 0x000000000000FF00) | \
                             (((uint64_t)(x) >> 24) & 0x0000000000FF0000) | \
                             (((uint64_t)(x) >> 8)  & 0x00000000FF000000) | \
                             (((uint64_t)(x) << 8)  & 0x000000FF00000000) | \
                             (((uint64_t)(x) << 24) & 0x0000FF0000000000) | \
                             (((uint64_t)(x) << 40) & 0x00FF000000000000) | \
                             (((uint64_t)(x) << 56) & 0xFF00000000000000)))

#ifndef __BYTE_ORDER__
#error "__BYTE_ORDER__ macro not defined by the compiler"
#endif

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

/**
 * @brief   Convert a constant from host byte order to network byte
 *          order, 16 bit.
 */
#define CONST_HTONS(x)       ((network_uint16_t) CONST_BSWAP16(x))

/**
 * @brief   Convert a constant from host byte order to network byte
 *          order, 32 bit.
 */
#define CONST_HTONL(x)       ((network_uint32_t) CONST_BSWAP32(x))

/**
 * @brief   Convert a constant from host byte order to network byte
 *          order, 64 bit.
 */
#define CONST_HTONLL(x)      ((network_uint64_t) CONST_BSWAP64(x))

/**
 * @brief   Convert a constant from network byte order to host byte
 *          order, 16 bit.
 */
#define CONST_NTOHS(x)       CONST_BSWAP16(x)

/**
 * @brief   Convert a constant from network byte order to host byte
 *          order, 32 bit.
 */
#define CONST_NTOHL(x)       CONST_BSWAP32(x)

/**
 * @brief   Convert a constant from network byte order to host byte
 *          order, 64 bit.
 */
#define CONST_NTOHLL(x)      CONST_BSWAP64(x)

/**
 * @brief          Constant big endian, 16 bit.
 */
#define CONST_TOBES(x)       ((be_uint16_t) CONST_BSWAP16(x))

/**
 * @brief          Constant to big endian, 32 bit.
 */
#define CONST_TOBEL(x)       ((be_uint32_t) CONST_BSWAP32(x))

/**
 * @brief          Constant to big endian, 64 bit.
 */
#define CONST_TOBELL(x)      ((be_uint64_t) CONST_BSWAP64(x))

/**
 * @brief          Constant to little endian, 16 bit.
 */
#define CONST_TOLES(x)       ((le_uint16_t) (x))

/**
 * @brief          Constant to little endian, 32 bit.
 */
#define CONST_TOLEL(x)       ((le_uint32_t) (x))

/**
 * @brief          Constant to little endian, 64 bit.
 */
#define CONST_TOLELL(x)      ((le_uint64_t) (x))

#else

/**
 * @brief   Convert a constant from host byte order to network byte
 *          order, 16 bit.
 */
#define CONST_HTONS(x)       ((network_uint16_t) (x))

/**
 * @brief   Convert a constant from host byte order to network byte
 *          order, 32 bit.
 */
#define CONST_HTONL(x)       ((network_uint32_t) (x))

/**
 * @brief   Convert a constant from host byte order to network byte
 *          order, 64 bit.
 */
#define CONST_HTONLL(x)      ((network_uint64_t) (x))

/**
 * @brief   Convert a constant from network byte order to host byte
 *          order, 16 bit.
 */
#define CONST_NTOHS(x)       (((network_uint16_t) (x)).u16)

/**
 * @brief   Convert a constant from network byte order to host byte
 *          order, 32 bit.
 */
#define CONST_NTOHL(x)       (((network_uint32_t) (x)).u32)

/**
 * @brief   Convert a constant from network byte order to host byte
 *          order, 64 bit.
 */
#define CONST_NTOHLL(x)      (((network_uint64_t) (x)).u64)

/**
 * @brief          Constant big endian, 16 bit.
 */
#define CONST_TOBES(x)       ((be_uint16_t) (x))

/**
 * @brief          Constant to big endian, 32 bit.
 */
#define CONST_TOBEL(x)       ((be_uint32_t) (x))

/**
 * @brief          Constant to big endian, 64 bit.
 */
#define CONST_TOBELL(x)      ((be_uint64_t) (x))

/**
 * @brief          Constant to little endian, 16 bit.
 */
#define CONST_TOLES(x)       ((le_uint16_t) CONST_BSWAP16(x))

/**
 * @brief          Constant to little endian, 32 bit.
 */
#define CONST_TOLEL(x)       ((le_uint32_t) CONST_BSWAP32(x))

/**
 * @brief          Constant to little endian, 64 bit.
 */
#define CONST_TOLELL(x)      ((le_uint64_t) CONST_BSWAP64(x))

#endif

#ifdef HAVE_NO_BUILTIN_BSWAP16
static inline unsigned short __builtin_bswap16(unsigned short a)
{
    return (a<<8)|(a>>8);
}
#endif

static inline uint16_t byteorder_swaps(uint16_t v)
{
#ifndef MODULE_MSP430_COMMON
    return __builtin_bswap16(v);
#else
    network_uint16_t result = { v };
    uint8_t tmp = result.u8[0];
    result.u8[0] = result.u8[1];
    result.u8[1] = tmp;
    return result.u16;
#endif
}

static inline uint32_t byteorder_swapl(uint32_t v)
{
    return __builtin_bswap32(v);
}

static inline uint64_t byteorder_swapll(uint64_t v)
{
    return __builtin_bswap64(v);
}

static inline be_uint16_t byteorder_ltobs(le_uint16_t v)
{
    be_uint16_t result = { byteorder_swaps(v.u16) };
    return result;
}

static inline be_uint32_t byteorder_ltobl(le_uint32_t v)
{
    be_uint32_t result = { byteorder_swapl(v.u32) };
    return result;
}

static inline be_uint64_t byteorder_ltobll(le_uint64_t v)
{
    be_uint64_t result = { byteorder_swapll(v.u64) };
    return result;
}

static inline le_uint16_t byteorder_btols(be_uint16_t v)
{
    le_uint16_t result = { byteorder_swaps(v.u16) };
    return result;
}

static inline le_uint32_t byteorder_btoll(be_uint32_t v)
{
    le_uint32_t result = { byteorder_swapl(v.u32) };
    return result;
}

static inline le_uint64_t byteorder_btolll(be_uint64_t v)
{
    le_uint64_t result = { byteorder_swapll(v.u64) };
    return result;
}

/**
 * @brief Swaps the byteorder according to the endianess
 */
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#   define _byteorder_swap(V, T) (byteorder_swap##T((V)))
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#   define _byteorder_swap(V, T) (V)
#else
#   error "Byte order is neither little nor big!"
#endif

static inline network_uint16_t byteorder_htons(uint16_t v)
{
    network_uint16_t result = { _byteorder_swap(v, s) };
    return result;
}

static inline network_uint32_t byteorder_htonl(uint32_t v)
{
    network_uint32_t result = { _byteorder_swap(v, l) };
    return result;
}

static inline network_uint64_t byteorder_htonll(uint64_t v)
{
    network_uint64_t result = { _byteorder_swap(v, ll) };
    return result;
}

static inline uint16_t byteorder_ntohs(network_uint16_t v)
{
    return _byteorder_swap(v.u16, s);
}

static inline uint32_t byteorder_ntohl(network_uint32_t v)
{
    return _byteorder_swap(v.u32, l);
}

static inline uint64_t byteorder_ntohll(network_uint64_t v)
{
    return _byteorder_swap(v.u64, ll);
}

/**
 * @brief          Convert from host byte order to network byte order, 16 bit.
 */
#define HTONS(x)             CONST_HTONS(x)

/**
 * @brief          Convert from host byte order to network byte order, 32 bit.
 */
#define HTONL(x)             CONST_HTONL(x)

/**
 * @brief          Convert from host byte order to network byte order, 64 bit.
 */
#define HTONLL(x)            CONST_HTONLL(x)

/**
 * @brief          Convert from network byte order to host byte order, 16 bit.
 */
#define NTOHS(x)             CONST_NTOHS(x)

/**
 * @brief          Convert from network byte order to host byte order, 32 bit.
 */
#define NTOHL(x)             CONST_NTOHL(x)

/**
 * @brief          Convert from network byte order to host byte order, 64 bit.
 */
#define NTOHLL(x)            CONST_NTOHLL(x)

#ifdef __cplusplus
}
#endif

#endif /* BYTEORDER_H */
/** @} */
