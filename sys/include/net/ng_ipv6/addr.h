/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    net_ng_ipv6_addr    IPv6 addresses
 * @ingroup     net_ng_ipv6
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


#ifndef NG_IPV6_ADDR_H_
#define NG_IPV6_ADDR_H_

#include <stdbool.h>

#include "byteorder.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Length of an IPv6 address in bit.
 */
#define NG_IPV6_ADDR_BIT_LEN        (128)

/**
 * @brief   Maximum length of an IPv6 address as string.
 */
#define NG_IPV6_ADDR_MAX_STR_LEN    (sizeof("ffff:ffff:ffff:ffff:" \
                                            "ffff:ffff:255.255.255.255"))

/**
 * @brief Data type to represent an IPv6 address.
 */
typedef union __attribute__((packed)) {
    uint8_t u8[16];             /**< devided by 16 8-bit words. */
    network_uint16_t u16[8];    /**< devided by 8 16-bit words. */
    network_uint32_t u32[4];    /**< devided by 4 32-bit words. */
    network_uint64_t u64[2];    /**< devided by 2 64-bit words. */
} ng_ipv6_addr_t;

/**
 * @brief   Static initializer for the unspecified IPv6 address (::)
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.5.2">
 *          RFC 4291, section 2.5.2
 *      </a>
 */
#define NG_IPV6_ADDR_UNSPECIFIED {{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }}

/**
 * @brief   Static initializer for the loopback IPv6 address (::1)
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.5.3">
 *          RFC 4291, section 2.5.3
 *      </a>
 */
#define NG_IPV6_ADDR_LOOPBACK {{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }}

/**
 * @brief   Static initializer for the interface-local all nodes multicast IPv6
 *          address (ff01::1)
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.7">
 *          RFC 4291, section 2.7
 *      </a>
 */
#define NG_IPV6_ADDR_ALL_NODES_IF_LOCAL {{ 0xff, 0x01, 0, 0, 0, 0, 0, 0, \
                                           0, 0, 0, 0, 0, 0, 0, 1 }}

/**
 * @brief   Static initializer for the link-local all nodes multicast IPv6
 *          address (ff02::1)
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.7">
 *          RFC 4291, section 2.7
 *      </a>
 */
#define NG_IPV6_ADDR_ALL_NODES_LINK_LOCAL {{ 0xff, 0x02, 0, 0, 0, 0, 0, 0, \
                                             0, 0, 0, 0, 0, 0, 0, 1 }}

/**
 * @brief   Static initializer for the interface-local all routers multicast IPv6
 *          address (ff01::2)
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.7">
 *          RFC 4291, section 2.7
 *      </a>
 */
#define NG_IPV6_ADDR_ALL_ROUTERS_IF_LOCAL {{ 0xff, 0x01, 0, 0, 0, 0, 0, 0, \
                                             0, 0, 0, 0, 0, 0, 0, 2 }}

/**
 * @brief   Static initializer for the link-local all routers multicast IPv6
 *          address (ff02::2)
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.7">
 *          RFC 4291, section 2.7
 *      </a>
 */
#define NG_IPV6_ADDR_ALL_ROUTERS_LINK_LOCAL {{ 0xff, 0x02, 0, 0, 0, 0, 0, 0, \
                                               0, 0, 0, 0, 0, 0, 0, 2 }}


/**
 * @brief   Static initializer for the site-local all routers multicast IPv6
 *          address (ff05::2)
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.7">
 *          RFC 4291, section 2.7
 *      </a>
 */
#define NG_IPV6_ADDR_ALL_ROUTERS_SITE_LOCAL {{ 0xff, 0x05, 0, 0, 0, 0, 0, 0, \
                                               0, 0, 0, 0, 0, 0, 0, 2 }}

/**
 * @brief   Values for the flag field in multicast addresses.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.7">
 *          RFC 4291, section 2.7
 *      </a>
 */
typedef enum {
    /**
     * @brief   The address is transient, i.e. not well-known, permanantly
     *          assigned address by IANA.
     */
    NG_IPV6_ADDR_MCAST_FLAG_TRANSIENT = 0x01,

    /**
     * @brief   The address is based on a network prefix
     *
     * @see <a href="http://tools.ietf.org/html/rfc3306#section-4">
     *          RFC 3306, section 4
     *      </a>
     */
    NG_IPV6_ADDR_MCAST_FLAG_PREFIX_BASED = 0x02,

    /**
     * @brief   The address embeds the address on the rendezvous point
     *
     * @see <a href="http://tools.ietf.org/html/rfc3956#section-3">
     *          RFC 3956, section 3
     *      </a>
     */
    NG_IPV6_ADDR_MCAST_FLAG_EMBED_ON_RP = 0x04,
} ng_ipv6_addr_mcast_flag_t;

/**
 * @brief   Values for the scope field in multicast addresses.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.7">
 *          RFC 4291, section 2.7
 *      </a>
 */
typedef enum {
    NG_IPV6_ADDR_MCAST_SCP_IF_LOCAL    = 0x1,      /**< interface-local scope */
    NG_IPV6_ADDR_MCAST_SCP_LINK_LOCAL  = 0x2,      /**< link-local scope */
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
    NG_IPV6_ADDR_MCAST_SCP_REALM_LOCAL = 0x3,
    NG_IPV6_ADDR_MCAST_SCP_ADMIN_LOCAL = 0x4,      /**< admin-local scope */
    NG_IPV6_ADDR_MCAST_SCP_SITE_LOCAL  = 0x5,      /**< site-local scope */
    NG_IPV6_ADDR_MCAST_SCP_ORG_LOCAL   = 0x8,      /**< organization-local scope */
    NG_IPV6_ADDR_MCAST_SCP_GLOBAL      = 0xe,      /**< global scope */
} ng_ipv6_addr_mcast_scp_t;

/**
 * @brief   Checks if two IPv6 addresses are equal.
 *
 * @param[in] a     An IPv6 address.
 * @param[in] b     Another IPv6 address.
 *
 * @return  true, if @p a and @p b are equal
 * @return  false, otherwise.
 */
static inline bool ng_ipv6_addr_equal(const ng_ipv6_addr_t *a,
                                      const ng_ipv6_addr_t *b)
{
    return (a->u64[0].u64 == b->u64[0].u64) &&
           (a->u64[1].u64 == b->u64[1].u64);
}

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
static inline bool ng_ipv6_addr_is_unspecified(const ng_ipv6_addr_t *addr)
{
    return (addr->u64[0].u64 == 0) &&
           (addr->u64[1].u64 == 0);
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
static inline bool ng_ipv6_addr_is_multicast(const ng_ipv6_addr_t *addr)
{
    return (addr->u8[0] == 0xff);
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
static inline bool ng_ipv6_addr_is_loopback(const ng_ipv6_addr_t *addr)
{
    return (addr->u64[0].u64 == 0) &&
           (byteorder_ntohll(addr->u64[1]) == 1);
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
static inline bool ng_ipv6_addr_is_link_local(const ng_ipv6_addr_t *addr)
{
    return (byteorder_ntohll(addr->u64[0]) == 0xfe80000000000000) ||
           (ng_ipv6_addr_is_multicast(addr) &&
            (addr->u8[1] & 0x0f) == NG_IPV6_ADDR_MCAST_SCP_LINK_LOCAL);
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
static inline bool ng_ipv6_addr_is_unique_local_unicast(const ng_ipv6_addr_t *addr)
{
    return ((addr->u8[0] == 0xfc) || (addr->u8[0] == 0xfd));
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
static inline bool ng_ipv6_addr_is_solicited_node(const ng_ipv6_addr_t *addr)
{
    return (byteorder_ntohll(addr->u64[0]) == 0xff02000000000000) &&
           (byteorder_ntohl(addr->u32[2]) == 1) &&
           (addr->u8[12] == 0xff);
}

/**
 * @brief   Checks up to which bit-count two IPv6 addresses match in their
 *          prefix.
 *
 * @param[in] a An IPv6 address.
 * @param[in] b Another IPv6 address.
 *
 * @return  The number of bits @p a and @p b match in there prefix
 */
uint8_t ng_ipv6_addr_match_prefix(const ng_ipv6_addr_t *a, const ng_ipv6_addr_t *b);

/**
 * @brief   Sets IPv6 address @p out with the first @p bits bit taken
 *          from @p prefix and leaves the remaining bits untouched.
 *
 * @param[out]  out     Prefix to be set.
 * @param[in]   prefix  Address to take prefix from.
 * @param[in]   bits    Bits to be copied from @p prefix to @p out
 *                      (set to 128 when greater than 128).
 */
void ng_ipv6_addr_init_prefix(ng_ipv6_addr_t *out, const ng_ipv6_addr_t *prefix,
                              uint8_t bits);

/**
 * @brief   Sets @p addr dynamically to the unspecified IPv6 address (::).
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291#section-2.5.2">
 *          RFC 4291, section 2.5.2
 *      </a>
 *
 * @param[in,out] addr  The address to set.
 */
static inline void ng_ipv6_addr_set_unspecified(ng_ipv6_addr_t *addr)
{
    addr->u64[0].u64 = 0;
    addr->u64[1].u64 = 0;
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
static inline void ng_ipv6_addr_set_loopback(ng_ipv6_addr_t *addr)
{
    addr->u64[0].u64 = 0;
    addr->u64[1] = byteorder_htonll(1);
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
static inline void ng_ipv6_addr_set_link_local_prefix(ng_ipv6_addr_t *addr)
{
    addr->u64[0] = byteorder_htonll(0xfe80000000000000);
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
static inline void ng_ipv6_addr_set_iid(ng_ipv6_addr_t *addr, uint64_t iid)
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
static inline void ng_ipv6_addr_set_aiid(ng_ipv6_addr_t *addr, uint8_t *iid)
{
    addr->u8[8]  = iid[0];
    addr->u8[9]  = iid[1];
    addr->u8[10] = iid[2];
    addr->u8[11] = iid[3];
    addr->u8[12] = iid[4];
    addr->u8[13] = iid[5];
    addr->u8[14] = iid[6];
    addr->u8[15] = iid[7];
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
static inline void ng_ipv6_addr_set_multicast(ng_ipv6_addr_t *addr,
        ng_ipv6_addr_mcast_flag_t flags,
        ng_ipv6_addr_mcast_scp_t scope)
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
static inline void ng_ipv6_addr_set_all_nodes_multicast(ng_ipv6_addr_t *addr,
        ng_ipv6_addr_mcast_scp_t scope)
{
    addr->u64[0] = byteorder_htonll(0xff00000000000000);
    addr->u8[1] = (uint8_t)scope;
    addr->u64[1] = byteorder_htonll(1);
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
static inline void ng_ipv6_addr_set_all_routers_multicast(ng_ipv6_addr_t *addr,
        ng_ipv6_addr_mcast_scp_t scope)
{
    addr->u64[0] = byteorder_htonll(0xff00000000000000);
    addr->u8[1] = (uint8_t)scope;
    addr->u64[1] = byteorder_htonll(2);
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
static inline void ng_ipv6_addr_set_solicited_nodes(ng_ipv6_addr_t *out,
        const ng_ipv6_addr_t *in)
{
    out->u64[0] = byteorder_htonll(0xff02000000000000);
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
 * @param[out] result       The resulting string representation
 * @param[in] addr          An IPv6 address
 * @param[in] result_len    Length of @p result_len
 *
 * @return  @p result, on success
 * @return  NULL, if @p result_len was smaller than needed
 * @return  NULL, if @p result or @p addr was NULL
 */
char *ng_ipv6_addr_to_str(char *result, const ng_ipv6_addr_t *addr,
                          uint8_t result_len);

/**
 * @brief   Converts an IPv6 address string representation to a byte-represented
 *          IPv6 address
 *
 * @see <a href="https://tools.ietf.org/html/rfc5952">
 *          RFC 5952
 *      </a>
 *
 * @param[in] result    The resulting byte representation
 * @param[in] addr      An IPv6 address string representation
 *
 * @return  @p result, on success
 * @return  NULL, if @p addr was malformed
 * @return  NULL, if @p result or @p addr was NULL
 */
ng_ipv6_addr_t *ng_ipv6_addr_from_str(ng_ipv6_addr_t *result, const char *addr);

#ifdef __cplusplus
}
#endif

#endif /* NG_IPV6_ADDR_H_ */
/**
 * @}
 */
