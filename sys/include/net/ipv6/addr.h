/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    net_ipv6_addr    IPv6 addresses
 * @ingroup     net_ipv6
 * @brief       IPv6 address architecture
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291">
 *          RFC 4291
 *      </a>
 *
 * @{
 *
 * @file
 * @brief       Definitions for IPv6 addresses
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#ifndef NET_IPV6_ADDR_H
#define NET_IPV6_ADDR_H

#include <stdbool.h>
#include <string.h>

#include "byteorder.h"
#include "net/ipv4/addr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Length of an IPv6 address in bit.
 */
#define IPV6_ADDR_BIT_LEN           (128)

#ifdef MODULE_IPV4_ADDR
/**
 * @brief   Maximum length of an IPv6 address as string.
 */
#define IPV6_ADDR_MAX_STR_LEN       (sizeof("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255"))
#else
/**
 * @brief   Maximum length of an IPv6 address as string.
 */
#define IPV6_ADDR_MAX_STR_LEN       (sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff"))
#endif

/**
 * @brief The first 10 bits of a site-local IPv6 unicast address
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.5.7">
 *          RFC 4291, section 2.5.7
 *      </a>
 *
 * @note Site-local addresses are now deprecated as defined in <a
 *       href="http://tools.ietf.org/html/rfc3879">SLDEP</a>. They are only
 *       defined here for the distinction of global unicast addresses.
 */
#define IPV6_ADDR_SITE_LOCAL_PREFIX (0xfec0)

/**
 * @brief Data type to represent an IPv6 address.
 */
typedef union {
    uint8_t u8[16];             /**< divided by 16 8-bit words. */
    network_uint16_t u16[8];    /**< divided by 8 16-bit words. */
    network_uint32_t u32[4];    /**< divided by 4 32-bit words. */
    network_uint64_t u64[2];    /**< divided by 2 64-bit words. */
} ipv6_addr_t;

/**
 * @brief   Static initializer for the unspecified IPv6 address (::)
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.5.2">
 *          RFC 4291, section 2.5.2
 *      </a>
 */
#define IPV6_ADDR_UNSPECIFIED               {{ 0x00, 0x00, 0x00, 0x00, \
                                               0x00, 0x00, 0x00, 0x00, \
                                               0x00, 0x00, 0x00, 0x00, \
                                               0x00, 0x00, 0x00, 0x00 }}

/**
 * @brief   Static initializer for the loopback IPv6 address (::1)
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.5.3">
 *          RFC 4291, section 2.5.3
 *      </a>
 */
#define IPV6_ADDR_LOOPBACK                  {{ 0x00, 0x00, 0x00, 0x00, \
                                               0x00, 0x00, 0x00, 0x00, \
                                               0x00, 0x00, 0x00, 0x00, \
                                               0x00, 0x00, 0x00, 0x01 }}
/**
 * @brief   Static initializer for the link-local prefix (fe80::/64)
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.5.6">
 *          RFC 4291, section 2.5.6
 *      </a>
 */
#define IPV6_ADDR_LINK_LOCAL_PREFIX         {{ 0xfe, 0x80, 0x00, 0x00, \
                                               0x00, 0x00, 0x00, 0x00, \
                                               0x00, 0x00, 0x00, 0x00, \
                                               0x00, 0x00, 0x00, 0x00 }}

/**
 * @brief   Static initializer for the interface-local all nodes multicast IPv6
 *          address (ff01::1)
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.7">
 *          RFC 4291, section 2.7
 *      </a>
 */
#define IPV6_ADDR_ALL_NODES_IF_LOCAL        {{ 0xff, 0x01, 0x00, 0x00, \
                                               0x00, 0x00, 0x00, 0x00, \
                                               0x00, 0x00, 0x00, 0x00, \
                                               0x00, 0x00, 0x00, 0x01 }}

/**
 * @brief   Static initializer for the link-local all nodes multicast IPv6
 *          address (ff02::1)
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.7">
 *          RFC 4291, section 2.7
 *      </a>
 */
#define IPV6_ADDR_ALL_NODES_LINK_LOCAL      {{ 0xff, 0x02, 0x00, 0x00, \
                                               0x00, 0x00, 0x00, 0x00, \
                                               0x00, 0x00, 0x00, 0x00, \
                                               0x00, 0x00, 0x00, 0x01 }}

/**
 * @brief   Static initializer for the interface-local all routers multicast IPv6
 *          address (ff01::2)
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.7">
 *          RFC 4291, section 2.7
 *      </a>
 */
#define IPV6_ADDR_ALL_ROUTERS_IF_LOCAL      {{ 0xff, 0x01, 0x00, 0x00, \
                                               0x00, 0x00, 0x00, 0x00, \
                                               0x00, 0x00, 0x00, 0x00, \
                                               0x00, 0x00, 0x00, 0x02 }}

/**
 * @brief   Static initializer for the link-local all routers multicast IPv6
 *          address (ff02::2)
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.7">
 *          RFC 4291, section 2.7
 *      </a>
 */
#define IPV6_ADDR_ALL_ROUTERS_LINK_LOCAL    {{ 0xff, 0x02, 0x00, 0x00, \
                                               0x00, 0x00, 0x00, 0x00, \
                                               0x00, 0x00, 0x00, 0x00, \
                                               0x00, 0x00, 0x00, 0x02 }}

/**
 * @brief   Static initializer for the site-local all routers multicast IPv6
 *          address (ff05::2)
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.7">
 *          RFC 4291, section 2.7
 *      </a>
 */
#define IPV6_ADDR_ALL_ROUTERS_SITE_LOCAL    {{ 0xff, 0x05, 0x00, 0x00, \
                                               0x00, 0x00, 0x00, 0x00, \
                                               0x00, 0x00, 0x00, 0x00, \
                                               0x00, 0x00, 0x00, 0x02 }}

/**
 * @brief   Static initializer for the solicited node multicast prefix
 *          (ff02:0:0:0:0:1:ff00::/104)
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.7">
 *          RFC 4291, section 2.7
 *      </a>
 */
#define IPV6_ADDR_SOLICITED_NODE_PREFIX     {{ 0xff, 0x02, 0x00, 0x00, \
                                               0x00, 0x00, 0x00, 0x00, \
                                               0x00, 0x00, 0x00, 0x01, \
                                               0xff, 0x00, 0x00, 0x00 }}

/**
 * @name    Multicast address flags
 * @brief   Values for the flag field in multicast addresses.
 * @{
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.7">
 *          RFC 4291, section 2.7
 *      </a>
 */
/**
 * @brief   The address is transient, i.e. not well-known, permanently
 *          assigned address by IANA.
 */
#define IPV6_ADDR_MCAST_FLAG_TRANSIENT      (0x01)

/**
 * @brief   The address is based on a network prefix
 *
 * @see <a href="http://tools.ietf.org/html/rfc3306#section-4">
 *          RFC 3306, section 4
 *      </a>
 */
#define IPV6_ADDR_MCAST_FLAG_PREFIX_BASED   (0x02)

/**
 * @brief   The address embeds the address on the rendezvous point
 *
 * @see <a href="http://tools.ietf.org/html/rfc3956#section-3">
 *          RFC 3956, section 3
 *      </a>
 */
#define IPV6_ADDR_MCAST_FLAG_EMBED_ON_RP    (0x04)
/** @} */

/**
 * @name    Multicast address scopes
 * @brief   Values for the scope field in multicast addresses.
 * @{
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.7">
 *          RFC 4291, section 2.7
 *      </a>
 */
#define IPV6_ADDR_MCAST_SCP_IF_LOCAL        (0x1)   /**< interface-local scope */
#define IPV6_ADDR_MCAST_SCP_LINK_LOCAL      (0x2)   /**< link-local scope */

/**
 * @brief realm-local scope
 *
 * @see <a href="http://tools.ietf.org/html/rfc7346#section-3">
 *          RFC 7346, section 3
 *      </a> and
 *      <a href="http://tools.ietf.org/html/rfc7346#section-5">
 *          RFC 7346, section 5
 *      </a> and
 */
#define IPV6_ADDR_MCAST_SCP_REALM_LOCAL (0x3)
#define IPV6_ADDR_MCAST_SCP_ADMIN_LOCAL (0x4)      /**< admin-local scope */
#define IPV6_ADDR_MCAST_SCP_SITE_LOCAL  (0x5)      /**< site-local scope */
#define IPV6_ADDR_MCAST_SCP_ORG_LOCAL   (0x8)      /**< organization-local scope */
#define IPV6_ADDR_MCAST_SCP_GLOBAL      (0xe)      /**< global scope */
/** @} */

/**
 * @brief In-memory constants of defined addresses and prefixes
 * @{
 */
/**
 * @see @ref IPV6_ADDR_UNSPECIFIED
 */
extern const ipv6_addr_t ipv6_addr_unspecified;

/**
 * @see @ref IPV6_ADDR_LOOPBACK
 */
extern const ipv6_addr_t ipv6_addr_loopback;

/**
 * @see @ref IPV6_ADDR_LINK_LOCAL_PREFIX
 */
extern const ipv6_addr_t ipv6_addr_link_local_prefix;

/**
 * @see @ref IPV6_ADDR_ALL_NODES_IF_LOCAL
 */
extern const ipv6_addr_t ipv6_addr_all_nodes_if_local;

/**
 * @see @ref IPV6_ADDR_ALL_NODES_LINK_LOCAL
 */
extern const ipv6_addr_t ipv6_addr_all_nodes_link_local;

/**
 * @see @ref IPV6_ADDR_ALL_ROUTERS_IF_LOCAL
 */
extern const ipv6_addr_t ipv6_addr_all_routers_if_local;

/**
 * @see @ref IPV6_ADDR_ALL_ROUTERS_LINK_LOCAL
 */
extern const ipv6_addr_t ipv6_addr_all_routers_link_local;

/**
 * @see @ref IPV6_ADDR_ALL_ROUTERS_SITE_LOCAL
 */
extern const ipv6_addr_t ipv6_addr_all_routers_site_local;

/**
 * @see @ref IPV6_ADDR_SOLICITED_NODE_PREFIX
 */
extern const ipv6_addr_t ipv6_addr_solicited_node_prefix;
/**
 * @}
 */

/**
 * @brief   Checks if @p addr is unspecified (all zero).
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.5.2">
 *          RFC 4291, section 2.5.2
 *      </a>
 *
 * @param[in] addr  An IPv6 address.
 *
 * @return  true, if @p addr is unspecified address
 * @return  false, otherwise.
 */
static inline bool ipv6_addr_is_unspecified(const ipv6_addr_t *addr)
{
    return (memcmp(addr, &ipv6_addr_unspecified, sizeof(ipv6_addr_t)) == 0);
}

/**
 * @brief   Checks if @p addr is a loopback address.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.5.3">
 *          RFC 4291, section 2.5.3
 *      </a>
 *
 * @param[in] addr  An IPv6 address.
 *
 * @return  true, if @p addr is loopback address,
 * @return  false, otherwise.
 */
static inline bool ipv6_addr_is_loopback(const ipv6_addr_t *addr)
{
    return (memcmp(addr, &ipv6_addr_loopback, sizeof(ipv6_addr_t)) == 0);
}

/**
 * @brief   Checks if @p addr is a IPv4-compatible IPv6 address.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.5.5.1">
 *          RFC 4291, section 2.5.5.1
 *      </a>
 *
 * @param[in] addr  An IPv6 address.
 *
 * @return  true, if @p addr is an IPv4-compatible IPv6 address,
 * @return  false, otherwise.
 */
static inline bool ipv6_addr_is_ipv4_compat(const ipv6_addr_t *addr)
{
    return (memcmp(addr, &ipv6_addr_unspecified,
                   sizeof(ipv6_addr_t) - sizeof(ipv4_addr_t)) == 0);
}

/**
 * @brief   Checks if @p addr is a IPv4-mapped IPv6 address.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.5.5.2">
 *          RFC 4291, section 2.5.5.2
 *      </a>
 *
 * @param[in] addr  An IPv6 address.
 *
 * @return  true, if @p addr is an IPv4-compatible IPv6 address,
 * @return  false, otherwise.
 */
static inline bool ipv6_addr_is_ipv4_mapped(const ipv6_addr_t *addr)
{
    return ((memcmp(addr, &ipv6_addr_unspecified,
                    sizeof(ipv6_addr_t) - sizeof(ipv4_addr_t) - 2) == 0) &&
            (addr->u16[5].u16 == 0xffff));
}

/**
 * @brief   Check if @p addr is a multicast address.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.7">
 *          RFC 4291, section 2.7
 *      </a>
 *
 * @param[in] addr  An IPv6 address.
 *
 * @return  true, if @p addr is multicast address,
 * @return  false, otherwise.
 */
static inline bool ipv6_addr_is_multicast(const ipv6_addr_t *addr)
{
    return (addr->u8[0] == 0xff);
}

/**
 * @brief   Check if @p addr is a link-local address.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.5.6">
 *          RFC 4291, section 2.5.6
 *      </a>
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.7">
 *          RFC 4291, section 2.7
 *      </a>
 *
 * @param[in] addr  An IPv6 address.
 *
 * @return  true, if @p addr is link-local address,
 * @return  false, otherwise.
 */
static inline bool ipv6_addr_is_link_local(const ipv6_addr_t *addr)
{
    return (memcmp(addr, &ipv6_addr_link_local_prefix, sizeof(addr->u64[0])) == 0) ||
           (ipv6_addr_is_multicast(addr) &&
            (addr->u8[1] & 0x0f) == IPV6_ADDR_MCAST_SCP_LINK_LOCAL);
}

/**
 * @brief   Checks if @p addr is a site-local address.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.5.7">
 *          RFC 4291, section 2.5.7
 *      </a>
 *
 * @note Site-local addresses are now deprecated as defined in <a
 *       href="http://tools.ietf.org/html/rfc3879">SLDEP</a>. They are only
 *       defined here for the distinction of global unicast addresses.
 *
 * @param[in] addr  An IPv6 address.
 *
 * @return  true, if @p addr is a site-local unicast address,
 * @return  false, otherwise.
 */
static inline bool ipv6_addr_is_site_local(const ipv6_addr_t *addr)
{
    return (((byteorder_ntohs(addr->u16[0]) & 0xffc0) ==
             IPV6_ADDR_SITE_LOCAL_PREFIX) ||
            (ipv6_addr_is_multicast(addr) &&
             (addr->u8[1] & 0x0f) == IPV6_ADDR_MCAST_SCP_SITE_LOCAL));
}

/**
 * @brief   Check if @p addr is unique local unicast address.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4193">
 *          RFC 4193
 *      </a>
 *
 * @param[in] addr  An IPv6 address.
 *
 * @return  true, if @p addr is unique local unicast address,
 * @return  false, otherwise.
 */
static inline bool ipv6_addr_is_unique_local_unicast(const ipv6_addr_t *addr)
{
    return ((addr->u8[0] == 0xfc) || (addr->u8[0] == 0xfd));
}

/**
 * @brief   Check if @p addr is global unicast address.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.5.4">
 *          RFC 4291, section 2.5.4
 *      </a>
 *
 * @param[in] addr  An IPv6 address.
 *
 * @return  true, if @p addr is global unicast address,
 * @return  false, otherwise.
 */
static inline bool ipv6_addr_is_global(const ipv6_addr_t *addr)
{
    /* first check for multicast with global scope */
    if (ipv6_addr_is_multicast(addr)) {
        return ((addr->u8[1] & 0x0f) == IPV6_ADDR_MCAST_SCP_GLOBAL);
    }
    else {
        return !(ipv6_addr_is_link_local(addr) ||
                 ipv6_addr_is_unspecified(addr) ||
                 ipv6_addr_is_loopback(addr));
    }
}

/**
 * @brief   Check if @p addr is solicited-node multicast address.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.7.1">
 *          RFC 4291, section 2.7.1
 *      </a>
 *
 * @param[in] addr  An IPv6 address.
 *
 * @return  true, if @p addr is solicited-node multicast address,
 * @return  false, otherwise.
 */
static inline bool ipv6_addr_is_solicited_node(const ipv6_addr_t *addr)
{
    return (memcmp(addr, &ipv6_addr_solicited_node_prefix,
                   sizeof(ipv6_addr_t) - 3) == 0);
}

/**
 * @brief   Checks if two IPv6 addresses are equal.
 *
 * @param[in] a     An IPv6 address.
 * @param[in] b     Another IPv6 address.
 *
 * @return  true, if @p a and @p b are equal
 * @return  false, otherwise.
 */
bool ipv6_addr_equal(const ipv6_addr_t *a, const ipv6_addr_t *b);

/**
 * @brief   Checks up to which bit-count two IPv6 addresses match in their
 *          prefix.
 *
 * @param[in] a An IPv6 address.
 * @param[in] b Another IPv6 address.
 *
 * @return  The number of bits @p a and @p b match in their prefix
 */
uint8_t ipv6_addr_match_prefix(const ipv6_addr_t *a, const ipv6_addr_t *b);

/**
 * @brief   Sets IPv6 address @p out with the first @p bits taken
 *          from @p prefix and leaves the remaining bits untouched.
 *
 * @param[out]  out     Prefix to be set.
 * @param[in]   prefix  Address to take prefix from.
 * @param[in]   bits    Bits to be copied from @p prefix to @p out
 *                      (set to 128 when greater than 128).
 */
void ipv6_addr_init_prefix(ipv6_addr_t *out, const ipv6_addr_t *prefix, uint8_t bits);

/**
 * @brief   Sets the last @p bits of IPv6 address @p out to @p iid.
 *          Leading bits of @p out stay untouched.
 *
 * @param[out]  out     IPv6 address to be set.
 * @param[in]   iid     buffer representing the iid.
 * @param[in]   bits    Bits to be copied from @p iid to @p out
 *                      (set to 128 when greater than 128).
 */
void ipv6_addr_init_iid(ipv6_addr_t *out, const uint8_t *iid, uint8_t bits);

/**
 * @brief   Sets @p addr dynamically to the unspecified IPv6 address (::).
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.5.2">
 *          RFC 4291, section 2.5.2
 *      </a>
 *
 * @param[in,out] addr  The address to set.
 */
static inline void ipv6_addr_set_unspecified(ipv6_addr_t *addr)
{
    memset(addr, 0, sizeof(ipv6_addr_t));
}

/**
 * @brief   Sets @p addr dynamically to the loopback IPv6 address (::1).
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.5.3">
 *          RFC 4291, section 2.5.3
 *      </a>
 *
 * @param[in,out] addr  The address to set.
 */
static inline void ipv6_addr_set_loopback(ipv6_addr_t *addr)
{
    memset(addr, 0, sizeof(ipv6_addr_t));
    addr->u8[15] = 1;
}

/**
 * @brief   Sets the first 64 bit of @p addr to link local prefix (fe08::/64).
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.5.6">
 *          RFC 4291, section 2.5.6
 *      </a>
 *
 * @param[in,out] addr  The address to set.
 */
static inline void ipv6_addr_set_link_local_prefix(ipv6_addr_t *addr)
{
    memcpy(addr, &ipv6_addr_link_local_prefix, sizeof(addr->u64[0]));
}

/**
 * @brief   Sets the 64-bit interface ID (as integer) of a unicast or anycast
 *          IPv6 address.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.5.4">
 *          RFC 4291, section 2.5.4
 *      </a>
 *
 * @param[in,out] addr  The address to set.
 * @param[in] iid       The interface ID as integer to set.
 */
static inline void ipv6_addr_set_iid(ipv6_addr_t *addr, uint64_t iid)
{
    addr->u64[1] = byteorder_htonll(iid);
}

/**
 * @brief   Sets the 64-bit interface ID (as array) of a unicast or anycast
 *          IPv6 address.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.5.4">
 *          RFC 4291, section 2.5.4
 *      </a>
 *
 * @param[in,out] addr  The address to set.
 * @param[in] iid       The interface ID as array of at least length 8 to set.
 */
static inline void ipv6_addr_set_aiid(ipv6_addr_t *addr, uint8_t *iid)
{
    memcpy(&addr->u64[1], iid, sizeof(addr->u64[1]));
}

/**
 * @brief   Sets the bits for an address required to be a multicast address.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.7">
 *          RFC 4291, section 2.7
 *      </a>
 *
 * @param[in,out] addr  The address to set.
 * @param[in] flags     The multicast address' flags.
 * @param[in] scope     The multicast address' scope.
 */
static inline void ipv6_addr_set_multicast(ipv6_addr_t *addr, unsigned int flags,
                                           unsigned int scope)
{
    addr->u8[0] = 0xff;
    addr->u8[1] = (((uint8_t)flags) << 4) | (((uint8_t) scope) & 0x0f);
}

/**
 * @brief   Sets @p addr dynamically to an all nodes multicast IPv6 address (ff0S::1,
 *          where S is the scope).
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.7.1">
 *          RFC 4291, section 2.7.1
 *      </a>
 *
 * @param[in,out] addr  The address to set.
 * @param[in] scope     The multicast address' scope.
 */
static inline void ipv6_addr_set_all_nodes_multicast(ipv6_addr_t *addr, unsigned int scope)
{
    memcpy(addr, &ipv6_addr_all_nodes_if_local, sizeof(ipv6_addr_t));
    addr->u8[1] = (uint8_t)scope;
}

/**
 * @brief   Sets @p addr dynamically to an all routers multicast IPv6 address (ff0S::2,
 *          where S is the scope).
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.7.1">
 *          RFC 4291, section 2.7.1
 *      </a>
 *
 * @param[in,out] addr  The address to set.
 * @param[in] scope     The multicast address' scope.
 */
static inline void ipv6_addr_set_all_routers_multicast(ipv6_addr_t *addr, unsigned int scope)
{
    memcpy(addr, &ipv6_addr_all_routers_if_local, sizeof(ipv6_addr_t));
    addr->u8[1] = (uint8_t)scope;
}

/**
 * @brief   Set @p out to the solicited-node multicast address
 *          computed from @p in.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291">
 *          RFC 4291
 *      </a>
 *
 * @param[out]  out   Is set to solicited-node address of this node.
 * @param[in]   in    The IPv6 address the solicited-node address.
 */
static inline void ipv6_addr_set_solicited_nodes(ipv6_addr_t *out, const ipv6_addr_t *in)
{
    out->u64[0] = byteorder_htonll(0xff02000000000000ull);
    out->u32[2] = byteorder_htonl(1);
    out->u8[12] = 0xff;
    out->u8[13] = in->u8[13];
    out->u16[7] = in->u16[7];
}

/**
 * @brief   Converts an IPv6 address to its string representation
 *
 * @see <a href="https://tools.ietf.org/html/rfc5952">
 *          RFC 5952
 *      </a>
 *
 * @param[out] result       The resulting string representation of at least
 *                          @ref IPV6_ADDR_MAX_STR_LEN
 * @param[in] addr          An IPv6 address
 * @param[in] result_len    Length of @p result_len
 *
 * @return  @p result, on success
 * @return  NULL, if @p result_len was lesser than IPV6_ADDR_MAX_STR_LEN
 * @return  NULL, if @p result or @p addr was NULL
 */
char *ipv6_addr_to_str(char *result, const ipv6_addr_t *addr, uint8_t result_len);

/**
 * @brief   Converts an IPv6 address string representation to a byte-represented
 *          IPv6 address
 *
 * @see <a href="https://tools.ietf.org/html/rfc5952">
 *          RFC 5952
 *      </a>
 *
 * @param[out] result    The resulting byte representation
 * @param[in] addr       An IPv6 address string representation
 *
 * @return  @p result, on success
 * @return  NULL, if @p addr was malformed
 * @return  NULL, if @p result or @p addr was NULL
 */
ipv6_addr_t *ipv6_addr_from_str(ipv6_addr_t *result, const char *addr);

/**
 * @brief   Converts an IPv6 address from a buffer of characters to a
 *          byte-represented IPv6 address
 *
 * @see <a href="https://tools.ietf.org/html/rfc5952">
 *          RFC 5952
 *      </a>
 *
 * @note    @p addr_len should be between 0 and IPV6_ADDR_MAX_STR_LEN
 *
 * @param[out] result    The resulting byte representation
 * @param[in] addr       An IPv6 address string representation
 * @param[in] addr_len   The amount of characters to parse
 *
 * @return  @p result, on success
 * @return  NULL, if @p addr was malformed
 * @return  NULL, if @p result or @p addr was NULL
 */
ipv6_addr_t *ipv6_addr_from_buf(ipv6_addr_t *result, const char *addr,
                                size_t addr_len);

/**
 * @brief split IPv6 address string representation and return remaining string
 *
 * Will change @p separator position in @p addr_str to '\0'
 *
 * @param[in,out]   addr_str    Address to split
 * @param[in]       separator   Separator char to use
 *
 * @return      string following the first occurrence of @p separator in
 *              @p addr_str.
 * @return      NULL if @p separator was not found.
 */
char *ipv6_addr_split_str(char *addr_str, char separator);

/**
 * @brief split IPv6 address string representation
 *
 * @note Will change @p separator position in @p addr_str to '\0'
 *
 * @param[in,out]   addr_str    Address to split
 * @param[in]       separator   Separator char to use
 * @param[in]       _default    Default value
 *
 * @return      atoi(string after split)
 * @return      @p _default if no string after @p separator
 */
int ipv6_addr_split_int(char *addr_str, char separator, int _default);

/**
 * @brief split IPv6 prefix string representation
 *
 * E.g., "2001:db8::1/64" returns "64", changes @p addr_str to "2001:db8::1"
 *
 * @param[in,out]   addr_str    Address to split
 * @return          prefix length or 128 if none specified
 */
static inline int ipv6_addr_split_prefix(char *addr_str)
{
    return ipv6_addr_split_int(addr_str, '/', 128);
}

/**
 * @brief split IPv6 address + interface specifier
 *
 * E.g., "fe80::1%5" returns "5", changes @p addr_str to "fe80::1"
 *
 * @param[in,out]   addr_str Address to split
 * @return          string containing the interface specifier.
 * @return          NULL if no interface was specified.
 */
static inline char *ipv6_addr_split_iface(char *addr_str)
{
    return ipv6_addr_split_str(addr_str, '%');
}

/**
 * @brief Print IPv6 address to stdout
 *
 * @param[in]   addr  Pointer to ipv6_addr_t to print
 */
void ipv6_addr_print(const ipv6_addr_t *addr);

#ifdef __cplusplus
}
#endif

#endif /* NET_IPV6_ADDR_H */
/**
 * @}
 */
