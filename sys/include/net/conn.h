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
 * @brief       Provides a minimal common API for applications to connect to the
 *              different network stacks.
 *
 * This module provides a minimal common API for applications to connect over different network
 * stacks. For each network stack there is supposed to be at least one connection type
 * implementation. Note that this definition gives no restriction on how a connection type should be
 * structured for simplicity and modularity. As a result, it can't give any guarantee that an
 * implementation keeps them compatible to each other. For example, an implementation might allow,
 * that a UDP receive function is called on a raw IPv6 connection object and even choose to do so
 * for valid reasons (e.g. code size), but this definition does not impose this on the
 * implementation. Currently there are the following option types defined:
 *
 * * @ref conn_ip_t (net/conn/ip.h): raw IP connections
 * * @ref conn_tcp_t (net/conn/tcp.h): TCP connections
 * * @ref conn_udp_t (net/conn/udp.h): UDP connections
 *
 * @{
 *
 * @file
 * @brief   Application connection API definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 */
#ifndef NET_CONN_H_
#define NET_CONN_H_

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

#endif /* NET_CONN_H_ */
/** @} */
