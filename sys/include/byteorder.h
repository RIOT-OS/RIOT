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
#include <endian.h>
#include "unaligned.h"

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

static inline uint16_t byteorder_swaps(uint16_t v)
{
    return __builtin_bswap16(v);
}

static inline uint32_t byteorder_swapl(uint32_t v)
{
    return __builtin_bswap32(v);
}

static inline uint64_t byteorder_swapll(uint64_t v)
{
    return __builtin_bswap64(v);
}

static inline uint16_t byteorder_ltohs(le_uint16_t v)
{
    return le16toh(v.u16);
}

static inline uint32_t byteorder_ltohl(le_uint32_t v)
{
    return le32toh(v.u32);
}

static inline uint64_t byteorder_ltohll(le_uint64_t v)
{
    return le64toh(v.u64);
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
    le_uint16_t result = { .u16 = htole16(v) };

    return result;
}

static inline le_uint32_t byteorder_htoll(uint32_t v)
{
    le_uint32_t result = { .u32 = htole32(v) };

    return result;
}

static inline le_uint64_t byteorder_htolll(uint64_t v)
{
    le_uint64_t result = { .u64 = htole64(v) };

    return result;
}

static inline network_uint16_t byteorder_htons(uint16_t v)
{
    network_uint16_t result = { .u16 = htobe16(v) };

    return result;
}

static inline network_uint32_t byteorder_htonl(uint32_t v)
{
    network_uint32_t result = { .u32 = htobe32(v) };

    return result;
}

static inline network_uint64_t byteorder_htonll(uint64_t v)
{
    network_uint64_t result = { .u64 = htobe64(v) };

    return result;
}

static inline uint16_t byteorder_ntohs(network_uint16_t v)
{
    return be16toh(v.u16);
}

static inline uint32_t byteorder_ntohl(network_uint32_t v)
{
    return be32toh(v.u32);
}

static inline uint64_t byteorder_ntohll(network_uint64_t v)
{
    return be64toh(v.u64);
}

static inline uint16_t htons(uint16_t v)
{
    return htobe16(v);
}

static inline uint32_t htonl(uint32_t v)
{
    return htobe32(v);
}

static inline uint64_t htonll(uint64_t v)
{
    return htobe64(v);
}

static inline uint16_t ntohs(uint16_t v)
{
    return be16toh(v);
}

static inline uint32_t ntohl(uint32_t v)
{
    return be32toh(v);
}

static inline uint64_t ntohll(uint64_t v)
{
    return be64toh(v);
}

static inline uint16_t byteorder_bebuftohs(const uint8_t *buf)
{
    return be16toh(unaligned_get_u16(buf));
}

static inline uint32_t byteorder_bebuftohl(const uint8_t *buf)
{
    return be32toh(unaligned_get_u32(buf));
}

static inline uint64_t byteorder_bebuftohll(const uint8_t *buf)
{
    return be64toh(unaligned_get_u64(buf));
}

static inline void byteorder_htobebufs(uint8_t *buf, uint16_t val)
{
    val = htobe16(val);
    memcpy(buf, &val, sizeof(val));
}

static inline void byteorder_htobebufl(uint8_t *buf, uint32_t val)
{
    val = htobe32(val);
    memcpy(buf, &val, sizeof(val));
}

static inline void byteorder_htobebufll(uint8_t *buf, uint64_t val)
{
    val = htobe64(val);
    memcpy(buf, &val, sizeof(val));
}

static inline uint16_t byteorder_lebuftohs(const uint8_t *buf)
{
    return le16toh(unaligned_get_u16(buf));
}

static inline uint32_t byteorder_lebuftohl(const uint8_t *buf)
{
    return le32toh(unaligned_get_u32(buf));
}

static inline uint64_t byteorder_lebuftohll(const uint8_t *buf)
{
    return le64toh(unaligned_get_u64(buf));
}

static inline void byteorder_htolebufs(uint8_t *buf, uint16_t val)
{
    val = htole16(val);
    memcpy(buf, &val, sizeof(val));
}

static inline void byteorder_htolebufl(uint8_t *buf, uint32_t val)
{
    val = htole32(val);
    memcpy(buf, &val, sizeof(val));
}

static inline void byteorder_htolebufll(uint8_t *buf, uint64_t val)
{
    val = htole64(val);
    memcpy(buf, &val, sizeof(val));
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* BYTEORDER_H */
