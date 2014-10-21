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
 * @file           byteorder.h
 * @brief          Functions to work with different byte orders.
 *
 * @author         René Kijewski <rene.kijewski@fu-berlin.de>
 */

#ifndef BYTEORDER_H_
#define BYTEORDER_H_

#include <stdint.h>


/* ******************************* INTERFACE ******************************* */


/**
 * @brief          A 16 bit integer in little endian.
 * @details        This is a wrapper around an uint16_t to catch missing conversions
 *                 between different byte orders at compile time.
 */
typedef union __attribute__((packed)) {
    uint8_t      u8[2];
    uint16_t    u16;
} le_uint16_t;

/**
 * @brief          A 32 bit integer in little endian.
 * @details        This is a wrapper around an uint32_t to catch missing conversions
 *                 between different byte orders at compile time.
 */
typedef union __attribute__((packed)) {
    uint8_t      u8[4];
    uint16_t    u16[2];
    uint32_t    u32;
    le_uint16_t l16[2];
} le_uint32_t;

/**
 * @brief          A 64 bit integer in little endian.
 * @details        This is a wrapper around an uint64_t to catch missing conversions
 *                 between different byte orders at compile time.
 */
typedef union __attribute__((packed)) {
    uint8_t      u8[8];
    uint16_t    u16[4];
    uint32_t    u32[2];
    uint64_t    u64;
    le_uint16_t l16[4];
    le_uint32_t l32[2];
} le_uint64_t;

/**
 * @brief          A 16 bit integer in big endian aka network byte order.
 * @details        This is a wrapper around an uint16_t to catch missing conversions
 *                 between different byte orders at compile time.
 */
typedef union __attribute__((packed)) {
    uint8_t      u8[2];
    uint16_t    u16;
} be_uint16_t;

/**
 * @brief          A 32 bit integer in big endian aka network byte order.
 * @details        This is a wrapper around an uint32_t to catch missing conversions
 *                 between different byte orders at compile time.
 */
typedef union __attribute__((packed)) {
    uint8_t      u8[4];
    uint16_t    u16[2];
    uint32_t    u32;
    be_uint16_t b16[2];
} be_uint32_t;

/**
 * @brief          A 64 bit integer in big endian aka network byte order.
 * @details        This is a wrapper around an uint64_t to catch missing conversions
 *                 between different byte orders at compile time.
 */
typedef union __attribute__((packed)) {
    uint8_t      u8[8];
    uint16_t    u16[4];
    uint32_t    u32[2];
    uint64_t    u64;
    be_uint16_t b16[4];
    be_uint32_t b32[2];
} be_uint64_t;

typedef be_uint16_t network_uint16_t;
typedef be_uint32_t network_uint32_t;
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


uint16_t byteorder_swaps(uint16_t v)
{
#ifndef MODULE_MSP430_COMMON
    return __builtin_bswap16(v);
#else
    network_uint16_t result = { .u16 = v };
    uint8_t tmp = result.u8[0];
    result.u8[0] = result.u8[1];
    result.u8[1] = tmp;
    return result.u16;
#endif
}

uint32_t byteorder_swapl(uint32_t v)
{
    return __builtin_bswap32(v);
}

uint64_t byteorder_swapll(uint64_t v)
{
    return __builtin_bswap64(v);
}

be_uint16_t byteorder_ltobs(le_uint16_t v)
{
    be_uint16_t result = { .u16 =  byteorder_swaps(v.u16) };
    return result;
}

be_uint32_t byteorder_ltobl(le_uint32_t v)
{
    be_uint32_t result = { .u32 =  byteorder_swapl(v.u32) };
    return result;
}

be_uint64_t byteorder_ltobll(le_uint64_t v)
{
    be_uint64_t result = { .u64 =  byteorder_swapll(v.u64) };
    return result;
}

le_uint16_t byteorder_btols(be_uint16_t v)
{
    le_uint16_t result = { .u16 =  byteorder_swaps(v.u16) };
    return result;
}

le_uint32_t byteorder_btoll(be_uint32_t v)
{
    le_uint32_t result = { .u32 =  byteorder_swapl(v.u32) };
    return result;
}

le_uint64_t byteorder_btolll(be_uint64_t v)
{
    le_uint64_t result = { .u64 =  byteorder_swapll(v.u64) };
    return result;
}

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#   define _byteorder_swap(V, T) (byteorder_swap##T((V)))
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#   define _byteorder_swap(V, T) (V)
#else
#   error "Byte order is neither little nor big!"
#endif

network_uint16_t byteorder_htons(uint16_t v)
{
    network_uint16_t result = { .u16 = _byteorder_swap(v, s) };
    return result;
}

network_uint32_t byteorder_htonl(uint32_t v)
{
    network_uint32_t result = { .u32 = _byteorder_swap(v, l) };
    return result;
}

network_uint64_t byteorder_htonll(uint64_t v)
{
    network_uint64_t result = { .u64 = _byteorder_swap(v, ll) };
    return result;
}

uint16_t byteorder_ntohs(network_uint16_t v)
{
    return _byteorder_swap(v.u16, s);
}

uint32_t byteorder_ntohl(network_uint32_t v)
{
    return _byteorder_swap(v.u32, l);
}

uint64_t byteorder_ntohll(network_uint64_t v)
{
    return _byteorder_swap(v.u64, ll);
}

#endif /* BYTEORDER_H_ */
/** @} */
