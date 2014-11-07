/*
 * Copyright (C) 2014 Martin Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    ipv6_addr
 * @addtogroup  net_ipv6
 * @{
 *
 * @file        ipv6/addr.h
 * @brief       Definitions and functions for IPv6 addresses.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291">
 *          RFC 4291
 *      </a>
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */


#ifndef __IPV6_ADDR_H_
#define __IPV6_ADDR_H_

#include "byteorder.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Length of an IPv6 address in bit.
 */
#define IPV6_ADDR_BIT_LEN           (128)

/**
 * @brief Data type to represent an IPv6 address.
 */
typedef union __attribute__((packed)) {
    uint8_t u8[16];             /**< devided by 16 8-bit words. */
    network_uint16_t u16[8];    /**< devided by 8 16-bit words. */
    network_uint32_t u32[4];    /**< devided by 4 32-bit words. */
    network_uint64_t u64[2];    /**< devided by 2 64-bit words. */
} ipv6_addr_t;

/**
 * @brief   Checks if two IPv6 addresses are equal.
 *
 * @param[in] a     An IPv6 address.
 * @param[in] b     Another IPv6 address.
 *
 * @return  1 if *a* and *b* are equal, 0 otherwise.
 */
static inline int ipv6_addr_is_equal(const ipv6_addr_t *a, const ipv6_addr_t *b)
{
    return (a->u64[0].u64 == b->u64[0].u64) &&
           (a->u64[1].u64 == b->u64[1].u64);
}

/**
 * @brief   Checks if *ipv6_addr* is unspecified (all zero).
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.5.2">
 *          RFC 4291, section 2.5.2
 *      </a>
 *
 * @param[in] ipv6_addr An IPv6 address.
 *
 * @return  1 if *ipv6_addr* is unspecified address, 0 otherwise.
 */
static inline int ipv6_addr_is_unspecified(const ipv6_addr_t *ipv6_addr)
{
    return (ipv6_addr->u64[0].u64 == 0) &&
           (ipv6_addr->u64[1].u64 == 0);
}

/**
 * @brief   Check if *ipv6_addr* is a multicast address.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.7">
 *          RFC 4291, section 2.7
 *      </a>
 *
 * @param[in] ipv6_addr     An IPv6 address.
 *
 * @return  1 if *ipv6_addr* is multicast address, 0 otherwise.
 */
static inline int ipv6_addr_is_multicast(const ipv6_addr_t *ipv6_addr)
{
    return (ipv6_addr->u8[0] == 0xff);
}

/**
 * @brief   Checks if *ipv6_addr* is a loopback address.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.5.3">
 *          RFC 4291, section 2.5.3
 *      </a>
 *
 * @param[in] ipv6_addr An IPv6 address.
 *
 * @return  1 if *ipv6_addr* is loopback address, 0 otherwise.
 */
static inline int ipv6_addr_is_loopback(const ipv6_addr_t *ipv6_addr)
{
    return ipv6_addr->u64[0].u64 == 0 &&
           byteorder_ntohll(ipv6_addr->u64[1]) == 1;
}

/**
 * @brief   Check if *ipv6_addr* is a link-local address.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.5.6">
 *          RFC 4291, section 2.5.6
 *      </a>
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.7">
 *          RFC 4291, section 2.7
 *      </a>
 *
 * @param[in] ipv6_addr     An IPv6 address.
 *
 * @return  1 if *ipv6_addr* is link-local address, 0 otherwise.
 */
static inline int ipv6_addr_is_link_local(const ipv6_addr_t *ipv6_addr)
{
    return (byteorder_ntohll(ipv6_addr->u64[0]) == 0xfe80000000000000) ||
           (ipv6_addr_is_multicast(ipv6_addr) && (ipv6_addr->u8[1] & 0x0f) == 2);
}

/**
 * @brief   Check if *ipv6_addr* is unique local unicast address.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4193">
 *          RFC 4193
 *      </a>
 *
 * @param[in] ipv6_addr     An IPv6 address.
 *
 * @return  1 if *ipv6_addr* is unique local unicast address,
 *          0 otherwise.
 */
static inline int ipv6_addr_is_unique_local_unicast(const ipv6_addr_t *ipv6_addr)
{
    return ((ipv6_addr->u8[0] == 0xfc) || (ipv6_addr->u8[0] == 0xfd));
}

/**
 * @brief   Check if *ipv6_addr* is solicited-node multicast address.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.7.1">
 *          RFC 4291, section 2.7.1
 *      </a>
 *
 * @param[in] ipv6_addr     An IPv6 address.
 *
 * @return  1 if *ipv6_addr* is solicited-node multicast address,
 *          0 otherwise.
 */
static inline int ipv6_addr_is_solicited_node(const ipv6_addr_t *ipv6_addr)
{
    return (byteorder_ntohll(ipv6_addr->u64[0]) == 0xff02000000000000) &&
           (byteorder_ntohl(ipv6_addr->u32[2]) == 1) &&
           (ipv6_addr->u8[12] == 0xff);
}


#ifdef __cplusplus
}
#endif

#endif /* __IPV6_ADDR_H_ */
/**
 * @}
 */
