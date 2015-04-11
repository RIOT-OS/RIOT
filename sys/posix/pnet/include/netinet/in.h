/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  pnet
 * @{
 */

/**
 * @file    netinet/in.h
 * @brief   Main socket header
 * @see     <a href="http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/netinet_in.h.html">
 *              The Open Group Base Specifications Issue 7, <netinet/in.h>
 *          </a>
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef _NETINET_IN_H
#define _NETINET_IN_H

#include <inttypes.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "ipv6.h"
#include "socket_base/socket.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * IPv4 socket address type.
 */
struct sockaddr_in {
    sa_family_t     sin_family; ///< Protocol family, always AF_INET.
    in_port_t       sin_port;   ///< Port number
    struct in_addr  sin_addr;   ///< IPv4 address
};

/**
 * IPv6 address structure type.
 */
struct in6_addr {
    /**
     * Private RIOT-internal data, needs not to be touched by the user.
     */
    ipv6_addr_t     __in6_u;

    /**
     * IPv6 Address represented as sequence of 8-bit numbers. Member of
     * struct in6_addr.
     */
#define s6_addr     __in6_u.uint8

    /**
     * IPv6 Address represented as sequence of 16-bit numbers. Member of
     * struct in6_addr.
     */
#define s6_addr16   __in6_u.uint16

    /**
     * IPv6 Address represented as sequence of 32-bit numbers. Member of
     * struct in6_addr.
     */
#define s6_addr32   __in6_u.uint32
};

/**
 * IPv6 socket address type.
 */
struct sockaddr_in6 {
    /**
     * Private RIOT-internal data, needs not to be touched by the user.
     */
    sockaddr6_t __in6_a;

    /**
     * Protocol family, always AF_INET6. Member of struct sockaddr_in6
     */
#define         sin6_family     __in6_a.sin6_family

    /**
     * Port number. Member of struct sockaddr_in6
     */
#define         sin6_port       __in6_a.sin6_port

    /**
     * IPv6 traffic class and flow information. Member of struct sockaddr_in6
     */
#define         sin6_flowinfo   __in6_a.sin6_flowinfo

    /**
     * IPv6 address. Member of struct sockaddr_in6
     */
#define         sin6_addr       __in6_a.sin6_addr

    /**
     * Set of interfaces for a scope.
     */
    uint32_t    sin6_scope_id;
};

/**
 * IPv6 wildcard address.
 */
#define IN6ADDR_ANY_INIT        {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}}

/**
 * IPv6 loopback address.
 */
#define IN6ADDR_LOOPBACK_INIT   {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}}

/**
 * IPv6 socket address for the wildcard address.
 */
extern const struct sockaddr_in6 in6addr_any;

/**
 * IPv6 socket address for the loopback address.
 */
extern const struct sockaddr_in6 in6addr_loopback;

/**
 * IPv4 local host address.
 */
#define INADDR_ANY          ((in_addr_t)0x00000000)

/**
 * IPv4 broadcast address.
 */
#define INADDR_BROADCAST    ((in_addr_t)0xffffffff)

/**
 * Multicast hop limit option name for getsockopt() or setsockopt()
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/getsockopt.html">
 *          The Open Group Base Specification Issue 7, getsockopt
 *      </a>
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/setsockopt.html">
 *          The Open Group Base Specification Issue 7, setsockopt
 *      </a>
 */
#define IPV6_MULTICAST_HOPS 0x12

/**
 * Test for IPv6 multicast address.
 *
 * @param[in] a An IPv6 address.
 * @return  1 if *a* is an multicast address, 0 if not.
 */
#define IN6_IS_ADDR_MULTICAST(a) (((const uint8_t *) (a))[0] == 0xff)

#ifdef __cplusplus
}
#endif

/**
 * @}
 */
#endif /* _NETINET_IN_H */
