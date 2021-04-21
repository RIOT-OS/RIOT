/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_rfc5444_addr RFC 5444 address blocks
 * @ingroup     net_rfc5444
 * @brief       RFC 54444 address block structures.
 *
 * @see <a href="https://tools.ietf.org/html/rfc5444#section-5.3">
 *          RFC 5444, Section 5.3
 *      </a>
 *
 * @{
 *
 * @file
 * @brief       Definitions for RFC 5444 address block structures
 *
 * @author      Jean Pierre Dudey <jeandudey@hotmail.com>
 */

#ifndef NET_RFC5444_ADDR_H
#define NET_RFC5444_ADDR_H

#include <stdint.h>

#include "bitarithm.h"
#include "byteorder.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Message flags
 * @{
 */
#define RFC5444_ADDR_FLAG_HAS_HEAD           (BIT7)
#define RFC5444_ADDR_FLAG_HAS_FULL_TAIL      (BIT6)
#define RFC5444_ADDR_FLAG_HAS_ZERO_TAIL      (BIT5)
#define RFC5444_ADDR_FLAG_HAS_SINGLE_PRE_LEN (BIT4)
#define RFC5444_ADDR_FLAG_HAS_MULTI_PRE_LEN  (BIT3)
/** @} */

/**
 * @brief   Address block
 *
 * @see [RFC 5444, Section 5.3]
 *      (https://tools.ietf.org/html/rfc5444#section-5.3)
 */
typedef struct __attribute__((packed)) {
    uint8_t num_addr;   /**< Number of addresses represented by this address
                             block */
    uint8_t addr_flags; /**< Address block flags */
} rfc5444_addr_block_t;

/**
 * @brief   An RFC 5444 address.
 *
 * Addresses can be either IPv4 or IPv6 addresses, this container can
 * hold both type of addresses and the type is identified by the
 * rfc5444_addr_t::len field.
 *
 * @note The RFC 5444 standard doesn't specify which types of addresses are
 * allowed, but it's assumed that they're IPv4 and IPv6 so any other address
 * length is invalid.
 */
typedef struct {
    uint8_t len;                    /**< Address length in bytes */
    union {
        uint8_t u8[16];             /**< Divided by 16 8-bit words. */
        network_uint16_t u16[8];    /**< Divided by 8 16-bit words. */
        network_uint32_t u32[4];    /**< Divided by 4 32-bit words. */
        network_uint64_t u64[2];    /**< Divided by 2 64-bit words. */
    };
} rfc5444_addr_t;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /**< NET_RFC5444_ADDR_H */
/** @} */
