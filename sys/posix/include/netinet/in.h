/*
 * Copyright (C) 2013-15 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @addtogroup  posix_sockets
 * @{
 */

/**
 * @file
 * @brief   Main socket header
 * @see     <a href="http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/netinet_in.h.html">
 *              The Open Group Base Specifications Issue 7, <netinet/in.h>
 *          </a>
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <inttypes.h>
#include <sys/socket.h>

#include "net/protnum.h"
#include "net/ipv6/addr.h"
#include "sys/bytes.h"

#ifdef __cplusplus
extern "C" {
#endif

#define INET_ADDRSTRLEN     (16)    /**< Length of the string form for IPv4. */
#define INET6_ADDRSTRLEN    (46)    /**< Length of the string form for IPv6. */

/**
 * IPv4 local host address.
 */
#define INADDR_ANY          ((in_addr_t)0x00000000)

/**
 * IPv4 broadcast address.
 */
#define INADDR_BROADCAST    ((in_addr_t)0xffffffff)

/**
 * IPv6 wildcard address.
 */
#define IN6ADDR_ANY_INIT        IPV6_ADDR_UNSPECIFIED

/**
 * IPv6 loopback address.
 */
#define IN6ADDR_LOOPBACK_INIT   IPV6_ADDR_LOOPBACK

/**
 * @name    IPv6 address macros
 * @{
 */
/**
 * @brief Check if address is the unspecified address (`::`).
 *
 * @param[in] addr  address of type `const struct in6_addr *`
 *
 * @return 0, when an address is not the unspecified address.
 * @return any other value otherwise.
 */
#define IN6_IS_ADDR_UNSPECIFIED(addr)   ((int)(ipv6_addr_is_unspecified((const ipv6_addr_t *)(addr))))

/**
 * @brief Check if address is the loopback address (`::1`).
 *
 * @param[in] addr  address of type `const struct in6_addr *`
 *
 * @return 0, when an address is not the loopback address.
 * @return any other value otherwise.
 */
#define IN6_IS_ADDR_LOOPBACK(addr)      ((int)(ipv6_addr_is_loopback((const ipv6_addr_t *)(addr))))

/**
 * @brief Check if address is a multicast address.
 *
 * @param[in] addr  address of type `const struct in6_addr *`
 *
 * @return 0, when an address is not a multicast address.
 * @return any other value otherwise.
 */
#define IN6_IS_ADDR_MULTICAST(addr)     ((int)(ipv6_addr_is_multicast((const ipv6_addr_t *)(addr))))

/**
 * @brief Check if address is a link-local address.
 *
 * @param[in] addr  address of type `const struct in6_addr *`
 *
 * @return 0, when an address is not a link-local address.
 * @return any other value otherwise.
 */
#define IN6_IS_ADDR_LINKLOCAL(addr)     ((int)(ipv6_addr_is_link_local((const ipv6_addr_t *)addr)))

/**
 * @brief Check if address is a site-local address.
 *
 * @param[in] addr  address of type `const struct in6_addr *`
 *
 * @return 0, when an address is not a site-local address.
 * @return any other value otherwise.
 */
#define IN6_IS_ADDR_SITELOCAL(addr)     ((int)(ipv6_addr_is_site_local((const ipv6_addr_t *)addr)))

/**
 * @brief Check if address is an IPv4 mapped address.
 *
 * @param[in] addr  address of type `const struct in6_addr *`
 *
 * @return 0, when an address is not an IPv4 mapped address.
 * @return any other value otherwise.
 */
#define IN6_IS_ADDR_V4MAPPED(addr)      ((int)(ipv6_addr_is_ipv4_mapped((const ipv6_addr_t *)addr)))

/**
 * @brief Check if address is an IPv4-compatible address.
 *
 * @param[in] addr  address of type `const struct in6_addr *`
 *
 * @return 0, when an address is not an IPv4-compatible address.
 * @return any other value otherwise.
 */
#define IN6_IS_ADDR_V4COMPAT(addr)      ((int)(ipv6_addr_is_ipv4_compat((const ipv6_addr_t *)addr)))

/**
 * @brief Check if address is a multicast node-local address.
 *
 * @param[in] addr  address of type `const struct in6_addr *`
 *
 * @return 0, when an address is not a multicast node-local address.
 * @return any other value otherwise.
 */
#define IN6_IS_ADDR_MC_NODELOCAL(addr)  (IN6_IS_ADDR_MULTICAST(addr) && \
        (int)((addr->s6_addr[1] & 0x0f) == IPV6_ADDR_MCAST_SCP_IF_LOCAL))

/**
 * @brief Check if address is a multicast link-local address.
 *
 * @param[in] addr  address of type `const struct in6_addr *`
 *
 * @return 0, when an address is not a multicast link-local address.
 * @return any other value otherwise.
 */
#define IN6_IS_ADDR_MC_LINKLOCAL(addr)  (IN6_IS_ADDR_MULTICAST(addr) && \
        (int)((addr->s6_addr[1] & 0x0f) == IPV6_ADDR_MCAST_SCP_LINK_LOCAL))

/**
 * @brief Check if address is a multicast site-local address.
 *
 * @param[in] addr  address of type `const struct in6_addr *`
 *
 * @return 0, when an address is not a multicast site-local address.
 * @return any other value otherwise.
 */
#define IN6_IS_ADDR_MC_SITELOCAL(addr)  (IN6_IS_ADDR_MULTICAST(addr) && \
        (int)((addr->s6_addr[1] & 0x0f) == IPV6_ADDR_MCAST_SCP_SITE_LOCAL))

/**
 * @brief Check if address is a multicast organization-local address.
 *
 * @param[in] addr  address of type `const struct in6_addr *`
 *
 * @return 0, when an address is not a multicast organization-local address.
 * @return any other value otherwise.
 */
#define IN6_IS_ADDR_MC_ORGLOCAL(addr)   (IN6_IS_ADDR_MULTICAST(addr) && \
        (int)((addr->s6_addr[1] & 0x0f) == IPV6_ADDR_MCAST_SCP_ORG_LOCAL))

/**
 * @brief Check if address is a multicast global address.
 *
 * @param[in] addr  address of type `const struct in6_addr *`
 *
 * @return 0, when an address is not a multicast global address.
 * @return any other value otherwise.
 */
#define IN6_IS_ADDR_MC_GLOBAL(addr)     (IN6_IS_ADDR_MULTICAST(addr) && \
        (int)((addr->s6_addr[1] & 0x0f) == IPV6_ADDR_MCAST_SCP_GLOBAL))
/** @} */

/**
 * @name    Protocol numbers for option
 * @{
 */
#define IPPROTO_IP      (PROTNUM_IPV4)          /**< Internet Protocol version 4 */
#define IPPROTO_IPV6    (PROTNUM_IPV6)          /**< Internet Protocol version 6 */
#define IPPROTO_ICMP    (PROTNUM_ICMP)          /**< Internet Control Message Protocol */
#define IPPROTO_ICMPV6  (PROTNUM_ICMPV6)        /**< ICMP for IPv6 */
#define IPPROTO_RAW     (PROTNUM_RESERVED)      /**< Raw IP packets protocol */
#define IPPROTO_TCP     (PROTNUM_TCP)           /**< Transmission control protocol */
#define IPPROTO_UDP     (PROTNUM_UDP)           /**< User datagram protocol */
/** @} */

/**
 * @todo IPv6 option names
 */

typedef uint16_t in_port_t;         /**< Internet port type */
typedef uint32_t in_addr_t;         /**< IPv4 address type */

/**
 * IPv4 address structure type.
 */
struct in_addr {
    in_addr_t s_addr;           /**< an IPv4 address */
};

/**
 * @brief   IPv6 address structure type.
 */
struct in6_addr {
    uint8_t s6_addr[16];        /**< unsigned 8-bit integer array */
};

/**
 * @brief   IPv4 socket address type.
 * @extends struct sockaddr
 */
struct sockaddr_in {
    sa_family_t     sin_family; /**< Protocol family, always AF_INET */
    in_port_t       sin_port;   /**< Port number */
    struct in_addr  sin_addr;   /**< IPv4 address */
};

/**
 * IPv6 socket address type.
 * @extends struct sockaddr
 */
struct sockaddr_in6 {
    /**
     * Protocol family, always AF_INET6. Member of struct sockaddr_in6
     */
    sa_family_t     sin6_family;    /**< Protocol family, always AF_INET6 */
    in_port_t       sin6_port;      /**< Port number */
    uint32_t        sin6_flowinfo;  /**< IPv6 traffic class and flow information */
    struct in6_addr sin6_addr;      /**< IPv6 address */
    uint32_t        sin6_scope_id;  /**< Set of interfaces for a scope */
};

/**
 * @brief   IPv6 multicast request.
 */
struct ipv6_mreq {
    struct in6_addr ipv6mr_multiaddr;   /**< an IPv6 multicast address */
    unsigned        ipv6mr_interface;   /**< interface index, leave 0 for default */
};

/**
 * IPv6 socket address for the wildcard address.
 */
extern const struct in6_addr in6addr_any;

/**
 * IPv6 socket address for the loopback address.
 */
extern const struct in6_addr in6addr_loopback;

#ifdef __cplusplus
}
#endif

/**
 * @}
 */
