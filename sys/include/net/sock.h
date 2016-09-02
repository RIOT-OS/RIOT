/*
 * Copyright (C) 2016 Alexander Aring <aar@pengutronix.de>
 *                    Freie Universität Berlin
 *                    HAW Hamburg
 *                    Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_sock    Sock API
 * @ingroup     net
 * @brief       Provides a minimal common API for applications to connect to the
 *              different network stacks
 *
 * About
 * =====
 *
 * ~~~~~~~~~~~~~~~~~~~~
 *    +---------------+
 *    |  Application  |
 *    +---------------+
 *            ^
 *            |
 *            v
 *          sock
 *            ^
 *            |
 *            v
 *    +---------------+
 *    | Network Stack |
 *    +---------------+
 * ~~~~~~~~~~~~~~~~~~~~
 *
 * This module provides a minimal set of functions to establish connections or
 * send and receives datagrams using different types of communication. Together,
 * they serve as a common API that connects application- and network stack code.
 *
 * Currently the following sock types are defined:
 *
 * * @ref sock_ip_t (net/sock/ip.h): raw IP sock
 * * @ref sock_tcp_t (net/sock/tcp.h): TCP sock
 * * @ref sock_udp_t (net/sock/udp.h): UDP sock
 *
 * Each network stack must implement at least one sock type.
 *
 * Note that there might be no relation between the different sock types.
 * For simplicity and modularity this API doesn't put any restriction of the
 * actual implementation of the type. For example, one implementation might
 * choose to have all sock types have a common base class or use the raw IP
 * sock type to send e.g. UDP packets, while others will keep them
 * completely separate from each other.
 *
 * How To Use
 * ==========
 *
 * A RIOT application uses the functions provided by one or more of the
 * sock type headers (for example @ref sock_udp), regardless of the
 * network stack it uses.
 * The network stack used under the bonnet is specified by including the
 * appropriate module (for example USEMODULE += gnrc_sock_udp)
 *
 * This allows for network stack agnostic code on the application layer.
 * The application code to establish a connection is always the same, allowing
 * the network stack underneath to be switched simply by changing the
 * `USEMODULE` definitions in the application's Makefile.
 *
 * @author  Alexander Aring <aar@pengutronix.de>
 * @author  Simon Brummer <simon.brummer@haw-hamburg.de>
 * @author  Cenk Gündoğan <mail@cgundogan.de>
 * @author  Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @{
 *
 * @file
 * @brief   Common sock API definitions
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef NET_SOCK_H_
#define NET_SOCK_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(DOXYGEN)
/**
 * @brief compile flag to activate IPv6 support for sock
 */
#define SOCK_HAS_IPV6
#endif

/**
 * @brief       Common flags for @ref net_conn
 * @name        net_sock_flags
 * @{
 */
#define SOCK_FLAGS_REUSE_EP     (0x0001)    /**< allow to reuse end point on bind */
/** @} */


/**
 * @brief   Special netif ID for "any interface"
 * @todo    Use an equivalent defintion from PR #5511
 */
#define SOCK_ADDR_ANY_NETIF (0)

/**
 * @brief   Address to bind to any IPv4 address
 */
#define SOCK_IPV4_EP_ANY        { .family = AF_INET, \
                                  .netif = SOCK_ADDR_ANY_NETIF }

#if defined(SOCK_HAS_IPV6) || defined(DOXYGEN)
/**
 * @brief   Address to bind to any IPv6 address
 */
#define SOCK_IPV6_EP_ANY        { .family = AF_INET6, \
                                  .netif = SOCK_ADDR_ANY_NETIF }
#endif

/**
 * @brief   Abstract IP end point and end point for a raw IP sock object
 */
typedef struct {
    /**
     * @brief family of sock_ip_ep_t::addr
     *
     * @see @ref net_af
     */
    int family;

    union {
#if defined(SOCK_HAS_IPV6) || defined(DOXYGEN)
        /**
         * @brief IPv6 address mode
         *
         * @note only available if @ref SOCK_HAS_IPV6 is defined.
         */
        uint8_t ipv6[16];
#endif
        uint32_t ipv4;      /**< IPv4 address mode */
    } addr;                 /**< address */

    /**
     * @brief   stack-specific network interface ID
     *
     * @todo    port to common network interface identifiers in PR #5511.
     *
     * Use @ref SOCK_ADDR_ANY_NETIF for any interface.
     * For reception this is the local interface the message came over,
     * for transmission, this is the local interface the message should be send
     * over
     */
    uint16_t netif;
} sock_ip_ep_t;

/**
 * @brief   Common IP-based transport layer end point
 */
struct _sock_tl_ep {
    /**
     * @brief family of sock_ip_ep_t::addr
     *
     * @see @ref net_af
     */
    int family;

    union {
#if defined(SOCK_HAS_IPV6) || defined(DOXYGEN)
        /**
         * @brief IPv6 address mode
         *
         * @note only available if @ref SOCK_HAS_IPV6 is defined.
         */
        uint8_t ipv6[16];
#endif
        uint32_t ipv4;      /**< IPv4 address mode */
    } addr;                 /**< address */

    /**
     * @brief   stack-specific network interface ID
     *
     * @todo    port to common network interface identifiers in PR #5511.
     *
     * Use @ref SOCK_ADDR_ANY_NETIF for any interface.
     * For reception this is the local interface the message came over,
     * for transmission, this is the local interface the message should be send
     * over
     */
    uint16_t netif;
    uint16_t port;          /**< transport layer port */
};

#ifdef __cplusplus
}
#endif

#endif /* NET_SOCK_H_ */
/** @} */
