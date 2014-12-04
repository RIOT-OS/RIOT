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

#include <stdbool.h>

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
 * @brief   Static initializer for the unspecified IPv6 address (::)
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.5.2">
 *          RFC 4291, section 2.5.2
 *      </a>
 */
#define IPV6_ADDR_UNSPECIFIED {{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }}

/**
 * @brief   Static initializer for the loopback IPv6 address (::1)
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.5.3">
 *          RFC 4291, section 2.5.3
 *      </a>
 */
#define IPV6_ADDR_LOOPBACK {{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }}

/**
 * @brief   Static initializer for the interface-local all nodes multicast IPv6
 *          address (ff01::1)
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.7">
 *          RFC 4291, section 2.7
 *      </a>
 */
#define IPV6_ADDR_ALL_NODES_IF_LOCAL {{ 0xff, 0x01, 0, 0, 0, 0, 0, 0, \
                                        0, 0, 0, 0, 0, 0, 0, 1 }}

/**
 * @brief   Static initializer for the link-local all nodes multicast IPv6
 *          address (ff02::1)
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.7">
 *          RFC 4291, section 2.7
 *      </a>
 */
#define IPV6_ADDR_ALL_NODES_LINK_LOCAL {{ 0xff, 0x02, 0, 0, 0, 0, 0, 0, \
                                          0, 0, 0, 0, 0, 0, 0, 1 }}

/**
 * @brief   Static initializer for the interface-local all routers multicast IPv6
 *          address (ff01::2)
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.7">
 *          RFC 4291, section 2.7
 *      </a>
 */
#define IPV6_ADDR_ALL_ROUTERS_IF_LOCAL {{ 0xff, 0x01, 0, 0, 0, 0, 0, 0, \
                                          0, 0, 0, 0, 0, 0, 0, 2 }}

/**
 * @brief   Static initializer for the link-local all routers multicast IPv6
 *          address (ff02::2)
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.7">
 *          RFC 4291, section 2.7
 *      </a>
 */
#define IPV6_ADDR_ALL_ROUTERS_LINK_LOCAL {{ 0xff, 0x02, 0, 0, 0, 0, 0, 0, \
                                            0, 0, 0, 0, 0, 0, 0, 2 }}


/**
 * @brief   Static initializer for the site-local all routers multicast IPv6
 *          address (ff05::2)
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.7">
 *          RFC 4291, section 2.7
 *      </a>
 */
#define IPV6_ADDR_ALL_ROUTERS_SITE_LOCAL {{ 0xff, 0x05, 0, 0, 0, 0, 0, 0, \
                                            0, 0, 0, 0, 0, 0, 0, 2 }}

/**
 * @brief   Values for the flag field in multicast addresses.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.7">
 *          RFC 4291, section 2.7
 *      </a>
 */
typedef enum __attribute__((packed)) {
    /**
     * @brief   The address is transient, i.e. not well-known, permanantly
     *          assigned address by IANA.
     */
    IPV6_ADDR_MCAST_FLAG_TRANSIENT = 0x01,

    /**
     * @brief   The address is based on a network prefix
     *
     * @see <a href="http://tools.ietf.org/html/rfc3306#section-4">
     *          RFC 3306, section 4
     *      </a>
     */
    IPV6_ADDR_MCAST_FLAG_PREFIX_BASED = 0x02,

    /**
     * @brief   The address embeds the address on the rendezvous point
     *
     * @see <a href="http://tools.ietf.org/html/rfc3956#section-3">
     *          RFC 3956, section 3
     *      </a>
     */
    IPV6_ADDR_MCAST_FLAG_EMBED_ON_RP = 0x04,
} ipv6_addr_mcast_flag_t;

/**
 * @brief   Values for the scope field in multicast addresses.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.7">
 *          RFC 4291, section 2.7
 *      </a>
 */
typedef enum __attribute__((packed)) {
    IPV6_ADDR_MCAST_SCP_IF_LOCAL    = 0x1,      /**< interface-local scope */
    IPV6_ADDR_MCAST_SCP_LINK_LOCAL  = 0x2,      /**< link-local scope */
    /**
     * @brief realm-local scope
     *
     * @see <a href="http://tools.ietf.org/html/rfc7346#section-3">
     *          RFC 7346, section 3
     *      </a> and
     *      <a href="http://tools.ietf.org/html/rfc7346#section-5">
     *          5
     *      </a> and
     */
    IPV6_ADDR_MCAST_SCP_REALM_LOCAL = 0x3,
    IPV6_ADDR_MCAST_SCP_ADMIN_LOCAL = 0x4,      /**< admin-local scope */
    IPV6_ADDR_MCAST_SCP_SITE_LOCAL  = 0x5,      /**< site-local scope */
    IPV6_ADDR_MCAST_SCP_ORG_LOCAL   = 0x8,      /**< organization-local scope */
    IPV6_ADDR_MCAST_SCP_GLOBAL      = 0xe,      /**< global scope */
} ipv6_addr_mcast_scp_t;

/**
 * @brief   Checks if two IPv6 addresses are equal.
 *
 * @param[in] a     An IPv6 address.
 * @param[in] b     Another IPv6 address.
 *
 * @return  true, if *a* and *b* are equal
 * @return  false, otherwise.
 */
static inline bool ipv6_addr_equal(const ipv6_addr_t *a, const ipv6_addr_t *b)
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
 * @return  true, if *ipv6_addr* is unspecified address
 * @return  false, otherwise.
 */
static inline bool ipv6_addr_is_unspecified(const ipv6_addr_t *ipv6_addr)
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
 * @return  true, if *ipv6_addr* is multicast address,
 * @returne false, otherwise.
 */
static inline bool ipv6_addr_is_multicast(const ipv6_addr_t *ipv6_addr)
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
 * @return  true, if *ipv6_addr* is loopback address,
 * @return  false, otherwise.
 */
static inline bool ipv6_addr_is_loopback(const ipv6_addr_t *ipv6_addr)
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
 * @return  true, if *ipv6_addr* is link-local address,
 * @return  false, otherwise.
 */
static inline bool ipv6_addr_is_link_local(const ipv6_addr_t *ipv6_addr)
{
    return (byteorder_ntohll(ipv6_addr->u64[0]) == 0xfe80000000000000) ||
           (ipv6_addr_is_multicast(ipv6_addr) &&
            (ipv6_addr->u8[1] & 0x0f) == IPV6_ADDR_MCAST_SCP_LINK_LOCAL);
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
 * @return  true, if *ipv6_addr* is unique local unicast address,
 * @return  false, otherwise.
 */
static inline bool ipv6_addr_is_unique_local_unicast(const ipv6_addr_t *ipv6_addr)
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
 * @return  true, if *ipv6_addr* is solicited-node multicast address,
 * @return  false, otherwise.
 */
static inline bool ipv6_addr_is_solicited_node(const ipv6_addr_t *ipv6_addr)
{
    return (byteorder_ntohll(ipv6_addr->u64[0]) == 0xff02000000000000) &&
           (byteorder_ntohl(ipv6_addr->u32[2]) == 1) &&
           (ipv6_addr->u8[12] == 0xff);
}

/**
 * @brief   Checks up to which bit-count two IPv6 addresses match in their
 *          prefix.
 *
 * @param[in] a An IPv6 address.
 * @param[in] b Another IPv6 address.
 *
 * @return  The number of bits *a* and *b* match in there prefix
 */
uint8_t ipv6_addr_match_prefix(const ipv6_addr_t *a, const ipv6_addr_t *b);

/**
 * @brief   Sets IPv6 address *out* with the first *bits* bit taken
 *          from *prefix* and the remaining bits to 0.
 *
 * @param[out]  out     Prefix to be set.
 * @param[in]   prefix  Address to take prefix from.
 * @param[in]   bits    Bits to be copied from *prefix* to *out*
 *                      (set to 128 when greater than 128).
 */
void ipv6_addr_init_prefix(ipv6_addr_t *out, const ipv6_addr_t *prefix,
                           uint8_t bits);

/**
 * @brief   Sets *addr* dynamically to the unspecified IPv6 address (::).
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.5.2">
 *          RFC 4291, section 2.5.2
 *      </a>
 *
 * @param[in,out] ipv6_addr The address to set.
 */
static inline void ipv6_addr_set_unspecified(ipv6_addr_t *ipv6_addr)
{
    ipv6_addr->u64[0].u64 = 0;
    ipv6_addr->u64[1].u64 = 0;
}

/**
 * @brief   Sets *addr* dynamically to the loopback IPv6 address (::1).
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.5.3">
 *          RFC 4291, section 2.5.3
 *      </a>
 *
 * @param[in,out] ipv6_addr The address to set.
 */
static inline void ipv6_addr_set_loopback(ipv6_addr_t *ipv6_addr)
{
    ipv6_addr->u64[0].u64 = 0;
    ipv6_addr->u64[1] = byteorder_htonll(1);
}

/**
 * @brief   Sets the first 64 bit of *ipv6_addr* to link local prefix (fe08::/64).
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.5.6">
 *          RFC 4291, section 2.5.6
 *      </a>
 *
 * @param[in,out] ipv6_addr The address to set.
 */
static inline void ipv6_addr_set_link_local_prefix(ipv6_addr_t *ipv6_addr)
{
    ipv6_addr->u64[0] = byteorder_htonll(0xfe80000000000000);
}

/**
 * @brief   Sets the 64-bit interface ID of a unicast or anycast IPv6 address.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.5.4">
 *          RFC 4291, section 2.5.4
 *      </a>
 *
 * @param[in,out] ipv6_addr The address to set.
 * @param[in] iid           The interface ID to set.
 */
static inline void ipv6_addr_set_iid(ipv6_addr_t *ipv6_addr, uint64_t iid)
{
    ipv6_addr->u64[1] = byteorder_htonll(iid);
}

/**
 * @brief   Sets the bits for an address required to be a multicast address.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.7">
 *          RFC 4291, section 2.7
 *      </a>
 *
 * @param[in,out] ipv6_addr The address to set.
 * @param[in] flags         The multicast address' flags.
 * @param[in] scope         The multicast address' scope.
 */
static inline void ipv6_addr_set_multicast(ipv6_addr_t *ipv6_addr,
        ipv6_addr_mcast_flag_t flags,
        ipv6_addr_mcast_scp_t scope)
{
    ipv6_addr->u8[0] = 0xff;
    ipv6_addr->u8[1] = (((uint8_t)flags) << 4) | (((uint8_t) scope) & 0x0f);
}

/**
 * @brief   Sets *addr* dynamically to an all nodes multicast IPv6 address (ff0S::1,
 *          where S is the scope).
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.7.1">
 *          RFC 4291, section 2.7.1
 *      </a>
 *
 * @param[in,out] ipv6_addr The address to set.
 * @param[in] scope         The multicast address' scope.
 */
static inline void ipv6_addr_set_all_nodes_multicast(ipv6_addr_t *ipv6_addr,
        ipv6_addr_mcast_scp_t scope)
{
    ipv6_addr->u64[0] = byteorder_htonll(0xff00000000000000);
    ipv6_addr->u8[1] = (uint8_t)scope;
    ipv6_addr->u64[1] = byteorder_htonll(1);
}

/**
 * @brief   Sets *addr* dynamically to an all routers multicast IPv6 address (ff0S::2,
 *          where S is the scope).
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.7.1">
 *          RFC 4291, section 2.7.1
 *      </a>
 *
 * @param[in,out] ipv6_addr The address to set.
 * @param[in] scope         The multicast address' scope.
 */
static inline void ipv6_addr_set_all_routers_multicast(ipv6_addr_t *ipv6_addr,
        ipv6_addr_mcast_scp_t scope)
{
    ipv6_addr->u64[0] = byteorder_htonll(0xff00000000000000);
    ipv6_addr->u8[1] = (uint8_t)scope;
    ipv6_addr->u64[1] = byteorder_htonll(2);
}

/**
 * @brief   Set *ipv6_addr_out* to the solicited-node multicast address
 *          computed from *ipv6_addr_in*.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291">
 *          RFC 4291
 *      </a>
 *
 * @param[out]  ipv6_addr_out   Is set to solicited-node address of
 *                              this node.
 * @param[in]   ipv6_addr_in    The IPv6 address the solicited-node
 *                              address.
 */
static inline void ipv6_addr_set_solicited_nodes(ipv6_addr_t *ipv6_addr_out,
        const ipv6_addr_t *ipv6_addr_in)
{
    ipv6_addr_out->u64[0] = byteorder_htonll(0xff02000000000000);
    ipv6_addr_out->u32[2] = byteorder_htonl(1);
    ipv6_addr_out->u8[12] = 0xff;
    ipv6_addr_out->u8[13] = ipv6_addr_in->u8[13];
    ipv6_addr_out->u16[7] = ipv6_addr_in->u16[7];
}

#ifdef __cplusplus
}
#endif

#endif /* __IPV6_ADDR_H_ */
/**
 * @}
 */
