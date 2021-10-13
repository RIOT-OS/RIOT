/*
 * Copyright (C) 2014 René Kijewski
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys
 * @{
 *
 * @file
 * @brief       Functions to work with different byte orders.
 *
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 */

#ifndef BYTEORDER_H
#define BYTEORDER_H

#include <string.h>
#include <stdint.h>
#include "unaligned.h"

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
    uint16_t u16;       /**< 16 bit representation */
    uint8_t u8[2];      /**< 8 bit representation */
} le_uint16_t;

/**
 * @brief          A 32 bit integer in little endian.
 * @details        This is a wrapper around an uint32_t to catch missing conversions
 *                 between different byte orders at compile time.
 */
typedef union __attribute__((packed)) {
    uint32_t u32;       /**< 32 bit representation */
    uint8_t u8[4];      /**< 8 bit representation */
    uint16_t u16[2];    /**< 16 bit representation */
    le_uint16_t l16[2]; /**< little endian 16 bit representation */
} le_uint32_t;

/**
 * @brief          A 64 bit integer in little endian.
 * @details        This is a wrapper around an uint64_t to catch missing conversions
 *                 between different byte orders at compile time.
 */
typedef union __attribute__((packed)) {
    uint64_t u64;       /**< 64 bit representation */
    uint8_t u8[8];      /**< 8 bit representation */
    uint16_t u16[4];    /**< 16 bit representation */
    uint32_t u32[2];    /**< 32 bit representation */
    le_uint16_t l16[4]; /**< little endian 16 bit representation */
    le_uint32_t l32[2]; /**< little endian 32 bit representation */
} le_uint64_t;

/**
 * @brief          A 16 bit integer in big endian aka network byte order.
 * @details        This is a wrapper around an uint16_t to catch missing conversions
 *                 between different byte orders at compile time.
 */
typedef union __attribute__((packed)) {
    uint16_t u16;       /**< 16 bit representation */
    uint8_t u8[2];      /**< 8 bit representation */
} be_uint16_t;

/**
 * @brief          A 32 bit integer in big endian aka network byte order.
 * @details        This is a wrapper around an uint32_t to catch missing conversions
 *                 between different byte orders at compile time.
 */
typedef union __attribute__((packed)) {
    uint32_t u32;       /**< 32 bit representation */
    uint8_t u8[4];      /**< 8 bit representation */
    uint16_t u16[2];    /**< 16 bit representation */
    be_uint16_t b16[2]; /**< big endian 16 bit representation */
} be_uint32_t;

/**
 * @brief          A 64 bit integer in big endian aka network byte order.
 * @details        This is a wrapper around an uint64_t to catch missing conversions
 *                 between different byte orders at compile time.
 */
typedef union __attribute__((packed)) {
    uint64_t u64;       /**< 64 bit representation */
    uint8_t u8[8];      /**< 8 bit representation */
    uint16_t u16[4];    /**< 16 bit representation */
    uint32_t u32[2];    /**< 32 bit representation */
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
 * @brief          Convert from little endian to host byte order, 16 bit.
 * @param[in]      v   The integer in little endian.
 * @returns        `v` converted to host byte order.
 */
static inline uint16_t byteorder_ltohs(le_uint16_t v);

/**
 * @brief          Convert from little endian to host byte order, 32 bit.
 * @param[in]      v   The integer in little endian.
 * @returns        `v` converted to host byte order.
 */
static inline uint32_t byteorder_ltohl(le_uint32_t v);

/**
 * @brief          Convert from little endian to host byte order, 64 bit.
 * @param[in]      v   The integer in little endian.
 * @returns        `v` converted to host byte order.
 */
static inline uint64_t byteorder_ltohll(le_uint64_t v);

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
 * @brief          Convert from host byte order to little endian, 16 bit.
 * @param[in]      v   The integer in host byte order.
 * @returns        `v` converted to little endian.
 */
static inline le_uint16_t byteorder_htols(uint16_t v);

/**
 * @brief          Convert from host byte order to little endian, 32 bit.
 * @param[in]      v   The integer in host byte order.
 * @returns        `v` converted to little endian.
 */
static inline le_uint32_t byteorder_htoll(uint32_t v);

/**
 * @brief          Convert from host byte order to little endian, 64 bit.
 * @param[in]      v   The integer in host byte order.
 * @returns        `v` converted to little endian.
 */
static inline le_uint64_t byteorder_htolll(uint64_t v);

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

/**
 * @brief           Read a big endian encoded unsigned integer from a buffer
 *                  into host byte order encoded variable, 16-bit
 *
 * @note            This function is agnostic to the alignment of the target
 *                  value in the given buffer
 *
 * @param[in] buf   position in a buffer holding the target value
 *
 * @return          16-bit unsigned integer in host byte order
 */
static inline uint16_t byteorder_bebuftohs(const uint8_t *buf);

/**
 * @brief           Read a big endian encoded unsigned integer from a buffer
 *                  into host byte order encoded variable, 32-bit
 *
 * @note            This function is agnostic to the alignment of the target
 *                  value in the given buffer
 *
 * @param[in] buf   position in a buffer holding the target value
 *
 * @return          32-bit unsigned integer in host byte order
 */
static inline uint32_t byteorder_bebuftohl(const uint8_t *buf);

/**
 * @brief           Read a big endian encoded unsigned integer from a buffer
 *                  into host byte order encoded variable, 64-bit
 *
 * @note            This function is agnostic to the alignment of the target
 *                  value in the given buffer
 *
 * @param[in] buf   position in a buffer holding the target value
 *
 * @return          64-bit unsigned integer in host byte order
 */
static inline uint64_t byteorder_bebuftohll(const uint8_t *buf);

/**
 * @brief           Write a host byte order encoded unsigned integer as big
 *                  endian encoded value into a buffer, 16-bit
 *
 * @note            This function is alignment agnostic and works with any given
 *                  memory location of the buffer
 *
 * @param[out] buf  target buffer, must be able to accept 2 bytes
 * @param[in]  val  value written to the buffer, in host byte order
 */
static inline void byteorder_htobebufs(uint8_t *buf, uint16_t val);

/**
 * @brief           Write a host byte order encoded unsigned integer as big
 *                  endian encoded value into a buffer, 32-bit
 *
 * @note            This function is alignment agnostic and works with any given
 *                  memory location of the buffer
 *
 * @param[out] buf  target buffer, must be able to accept 4 bytes
 * @param[in]  val  value written to the buffer, in host byte order
 */
static inline void byteorder_htobebufl(uint8_t *buf, uint32_t val);

/**
 * @brief           Write a host byte order encoded unsigned integer as big
 *                  endian encoded value into a buffer, 64-bit
 *
 * @note            This function is alignment agnostic and works with any given
 *                  memory location of the buffer
 *
 * @param[out] buf  target buffer, must be able to accept 8 bytes
 * @param[in]  val  value written to the buffer, in host byte order
 */
static inline void byteorder_htobebufll(uint8_t *buf, uint64_t val);

/**
 * @brief          Convert from host byte order to network byte order, 16 bit.
 * @see            byteorder_htons()
 * @param[in]      v   The integer to convert.
 * @returns        Converted integer.
 */
static inline uint16_t htons(uint16_t v);

/**
 * @brief          Convert from host byte order to network byte order, 32 bit.
 * @see            byteorder_htonl()
 * @param[in]      v   The integer to convert.
 * @returns        Converted integer.
 */
static inline uint32_t htonl(uint32_t v);

/**
 * @brief          Convert from host byte order to network byte order, 64 bit.
 * @see            byteorder_htonll()
 * @param[in]      v   The integer to convert.
 * @returns        Converted integer.
 */
static inline uint64_t htonll(uint64_t v);

/**
 * @brief          Convert from network byte order to host byte order, 16 bit.
 * @see            byteorder_ntohs()
 * @param[in]      v   The integer to convert.
 * @returns        Converted integer.
 */
static inline uint16_t ntohs(uint16_t v);

/**
 * @brief          Convert from network byte order to host byte order, 32 bit.
 * @see            byteorder_ntohl()
 * @param[in]      v   The integer to convert.
 * @returns        Converted integer.
 */
static inline uint32_t ntohl(uint32_t v);

/**
 * @brief          Convert from network byte order to host byte order, 64 bit.
 * @see            byteorder_ntohll()
 * @param[in]      v   The integer to convert.
 * @returns        Converted integer.
 */
static inline uint64_t ntohll(uint64_t v);

/* **************************** IMPLEMENTATION ***************************** */

#ifdef HAVE_NO_BUILTIN_BSWAP16
static inline unsigned short __builtin_bswap16(unsigned short a)
{
    return (a << 8) | (a >> 8);
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

/**
 * @brief Swaps the byteorder according to the endianness (host -> le)
 */
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#   define _byteorder_swap_le(V, T) (V)
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#   define _byteorder_swap_le(V, T) (byteorder_swap ## T((V)))
#else
#   error "Byte order is neither little nor big!"
#endif

static inline uint16_t byteorder_ltohs(le_uint16_t v)
{
    return _byteorder_swap_le(v.u16, s);
}

static inline uint32_t byteorder_ltohl(le_uint32_t v)
{
    return _byteorder_swap_le(v.u32, l);
}

static inline uint64_t byteorder_ltohll(le_uint64_t v)
{
    return _byteorder_swap_le(v.u64, ll);
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

static inline le_uint16_t byteorder_htols(uint16_t v)
{
    le_uint16_t result = { _byteorder_swap_le(v, s) };

    return result;
}

static inline le_uint32_t byteorder_htoll(uint32_t v)
{
    le_uint32_t result = { _byteorder_swap_le(v, l) };

    return result;
}

static inline le_uint64_t byteorder_htolll(uint64_t v)
{
    le_uint64_t result = { _byteorder_swap_le(v, ll) };

    return result;
}

/**
 * @brief Swaps the byteorder according to the endianness (host -> BE)
 */
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#   define _byteorder_swap(V, T) (byteorder_swap ## T((V)))
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

static inline uint16_t htons(uint16_t v)
{
    return byteorder_htons(v).u16;
}

static inline uint32_t htonl(uint32_t v)
{
    return byteorder_htonl(v).u32;
}

static inline uint64_t htonll(uint64_t v)
{
    return byteorder_htonll(v).u64;
}

static inline uint16_t ntohs(uint16_t v)
{
    network_uint16_t input = { v };

    return byteorder_ntohs(input);
}

static inline uint32_t ntohl(uint32_t v)
{
    network_uint32_t input = { v };

    return byteorder_ntohl(input);
}

static inline uint64_t ntohll(uint64_t v)
{
    network_uint64_t input = { v };

    return byteorder_ntohll(input);
}

static inline uint16_t byteorder_bebuftohs(const uint8_t *buf)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return (uint16_t)((buf[0] << 8) | (buf[1] << 0));
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    /* big endian to big endian conversion is easy, but buffer might be
     * unaligned */
    return unaligned_get_u16(buf);
#endif
}

static inline uint32_t byteorder_bebuftohl(const uint8_t *buf)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return (((uint32_t) buf[0] << 24)
          | ((uint32_t) buf[1] << 16)
          | ((uint32_t) buf[2] << 8)
          | ((uint32_t) buf[3] << 0));
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    /* big endian to big endian conversion is easy, but buffer might be
     * unaligned */
    return unaligned_get_u32(buf);
#endif
}

static inline uint64_t byteorder_bebuftohll(const uint8_t *buf)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return (((uint64_t) buf[0] << 56)
          | ((uint64_t) buf[1] << 48)
          | ((uint64_t) buf[2] << 40)
          | ((uint64_t) buf[3] << 32)
          | ((uint64_t) buf[4] << 24)
          | ((uint64_t) buf[5] << 16)
          | ((uint64_t) buf[6] <<  8)
          | ((uint64_t) buf[7] <<  0));
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    /* big endian to big endian conversion is easy, but buffer might be
     * unaligned */
    return unaligned_get_u64(buf);
#endif
}

static inline void byteorder_htobebufs(uint8_t *buf, uint16_t val)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    buf[0] = (uint8_t)(val >> 8);
    buf[1] = (uint8_t)(val >> 0);
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    /* big endian to big endian conversion is easy, but buffer might be
     * unaligned */
    memcpy(buf, &val, sizeof(val));
#endif
}

static inline void byteorder_htobebufl(uint8_t *buf, uint32_t val)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    buf[0] = (uint8_t)(val >> 24);
    buf[1] = (uint8_t)(val >> 16);
    buf[2] = (uint8_t)(val >> 8);
    buf[3] = (uint8_t)(val >> 0);
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    /* big endian to big endian conversion is easy, but buffer might be
     * unaligned */
    memcpy(buf, &val, sizeof(val));
#endif
}

static inline void byteorder_htobebufll(uint8_t *buf, uint64_t val)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    buf[0] = (uint8_t)(val >> 56);
    buf[1] = (uint8_t)(val >> 48);
    buf[2] = (uint8_t)(val >> 40);
    buf[3] = (uint8_t)(val >> 32);
    buf[4] = (uint8_t)(val >> 24);
    buf[5] = (uint8_t)(val >> 16);
    buf[6] = (uint8_t)(val >> 8);
    buf[7] = (uint8_t)(val >> 0);
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    /* big endian to big endian conversion is easy, but buffer might be
     * unaligned */
    memcpy(buf, &val, sizeof(val));
#endif
}

static inline uint16_t byteorder_lebuftohs(const uint8_t *buf)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    /* little endian to little endian conversion is easy, but buffer might be
     * unaligned */
    return unaligned_get_u16(buf);
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    return (uint16_t)((buf[0] << 8) | (buf[1] << 0));
#endif
}

static inline uint32_t byteorder_lebuftohl(const uint8_t *buf)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    /* little endian to little endian conversion is easy, but buffer might be
     * unaligned */
    return unaligned_get_u32(buf);
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    return (((uint32_t) buf[0] << 24)
          | ((uint32_t) buf[1] << 16)
          | ((uint32_t) buf[2] << 8)
          | ((uint32_t) buf[3] << 0));
#endif
}

static inline uint64_t byteorder_lebuftohll(const uint8_t *buf)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    /* little endian to little endian conversion is easy, but buffer might be
     * unaligned */
    return unaligned_get_u64(buf);
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    return (((uint64_t) buf[0] << 56)
          | ((uint64_t) buf[1] << 48)
          | ((uint64_t) buf[2] << 40)
          | ((uint64_t) buf[3] << 32)
          | ((uint64_t) buf[4] << 24)
          | ((uint64_t) buf[5] << 16)
          | ((uint64_t) buf[6] <<  8)
          | ((uint64_t) buf[7] <<  0));
#endif
}

static inline void byteorder_htolebufs(uint8_t *buf, uint16_t val)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    /* little endian to little endian conversion is easy, but buffer might be
     * unaligned */
    memcpy(buf, &val, sizeof(val));
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    buf[0] = (uint8_t)(val >> 8);
    buf[1] = (uint8_t)(val >> 0);
#endif
}

static inline void byteorder_htolebufl(uint8_t *buf, uint32_t val)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    /* little endian to little endian conversion is easy, but buffer might be
     * unaligned */
    memcpy(buf, &val, sizeof(val));
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    buf[0] = (uint8_t)(val >> 24);
    buf[1] = (uint8_t)(val >> 16);
    buf[2] = (uint8_t)(val >> 8);
    buf[3] = (uint8_t)(val >> 0);
#endif
}

static inline void byteorder_htolebufll(uint8_t *buf, uint64_t val)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    /* little endian to little endian conversion is easy, but buffer might be
     * unaligned */
    memcpy(buf, &val, sizeof(val));
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    buf[0] = (uint8_t)(val >> 56);
    buf[1] = (uint8_t)(val >> 48);
    buf[2] = (uint8_t)(val >> 40);
    buf[3] = (uint8_t)(val >> 32);
    buf[4] = (uint8_t)(val >> 24);
    buf[5] = (uint8_t)(val >> 16);
    buf[6] = (uint8_t)(val >> 8);
    buf[7] = (uint8_t)(val >> 0);
#endif
}

#ifdef __cplusplus
}
#endif

#endif /* BYTEORDER_H */
/** @} */
