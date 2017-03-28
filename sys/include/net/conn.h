/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_conn    Application connection API
 * @ingroup     net
 * @deprecated  Please use @ref net_sock instead
 * @brief       Provides a minimal common API for applications to connect to the
 *              different network stacks.
 *
 * About
 * =====
 *
 * ~~~~~~~~~~~~~~~~~~~~~
 *    +---------------+
 *    |  Application  |
 *    +---------------+
 *            ^
 *            |
 *            v
 *          conn
 *            ^
 *            |
 *            v
 *    +---------------+
 *    | Network Stack |
 *    +---------------+
 * ~~~~~~~~~~~~~~~~~~~~~
 *
 * This module provides a minimal set of functions to establish a connection using
 * different types of connections. Together, they serve as a common API
 * that connects application- and network stack code.
 *
 * Currently the following connection types are defined:
 *
 * * @ref conn_ip_t (net/conn/ip.h): raw IP connections
 * * @ref conn_tcp_t (net/conn/tcp.h): TCP connections
 * * @ref conn_udp_t (net/conn/udp.h): UDP connections
 *
 * Each network stack must implement at least one connection type.
 *
 * Note that there might be no relation between the different connection types.
 * For simplicity and modularity this API doesn't put any restriction of the actual
 * implementation of the type. For example, one implementation might choose
 * to have all connection types have a common base class or use the raw IPv6
 * connection type to send e.g. UDP packets, while others will keep them
 * completely separate from each other.
 *
 * How To Use
 * ==========
 *
 * A RIOT application uses the functions provided by one or more of the connection types
 * headers (for example @ref conn_udp_t), regardless of the network stack it uses.
 * The network stack used under the bonnet is specified by including the appropriate
 * module (for example USEMODULE += gnrc_conn_udp)
 *
 * This allows for network stack agnostic code on the application layer.
 * The application code to establish a connection is always the same, allowing
 * the network stack underneath to be switched simply by changing the USEMODULE
 * definition in the application's Makefile.
 *
 * @{
 *
 * @file
 * @brief   Application connection API definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 */

#ifndef NET_CONN_H
#define NET_CONN_H

#include "net/conn/ip.h"
#include "net/conn/tcp.h"
#include "net/conn/udp.h"
#include "net/ipv6/addr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Find the best matching source address for a given prefix
 *
 * @param[in] dst   Pointer to the IPv6 address to find a match for
 *                  Must not be NULL
 *
 * @return NULL if no matching address on any interface could be found
 * @return pointer to an IPv6 address configured on an interface with the best
 *         match to @p dst
 */
ipv6_addr_t *conn_find_best_source(const ipv6_addr_t *dst);

#ifdef __cplusplus
}
#endif

#endif /* NET_CONN_H */
/** @} */
