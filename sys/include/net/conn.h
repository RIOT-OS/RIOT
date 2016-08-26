/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_conn    Application connectivity API
 * @ingroup     net
 * @brief       Provides a minimal common API for applications to connect to the
 *              different network stacks.
 *
 * About
 * =====
 *
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
 *
 * This module provides a minimal set of functions to establish connectivity or
 * send and receives datagrams using different types of connectivity. Together,
 * they serve as a common API that connects application- and network stack code.
 *
 * Currently the following connectivity types are defined:
 *
 * * @ref conn_ip_t (net/conn/ip.h): raw IP connectivity
 * * @ref conn_tcp_t (net/conn/tcp.h): TCP connectivity
 * * @ref conn_udp_t (net/conn/udp.h): UDP connectivity
 *
 * Each network stack must implement at least one connectivity type.
 *
 * Note that there might be no relation between the different connectivity
 * types.
 * For simplicity and modularity this API doesn't put any restriction of the
 * actual implementation of the type. For example, one implementation might
 * choose to have all connectivity types have a common base class or use the raw
 * IPv6 connectivity type to send e.g. UDP packets, while others will keep them
 * completely separate from each other.
 *
 * How To Use
 * ==========
 *
 * A RIOT application uses the functions provided by one or more of the
 * connectivity type headers (for example @ref conn_udp), regardless of the
 * network stack it uses.
 * The network stack used under the bonnet is specified by including the
 * appropriate module (for example USEMODULE += gnrc_conn_udp)
 *
 * This allows for network stack agnostic code on the application layer.
 * The application code to establish connectivity is always the same, allowing
 * the network stack underneath to be switched simply by changing the
 * `USEMODULE` definitions in the application's Makefile.
 *
 * @{
 *
 * @file
 * @brief   Application connectivity API definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 */

#ifndef NET_CONN_H_
#define NET_CONN_H_

#include <stdbool.h>

#include "net/conn/ip.h"
#include "net/conn/tcp.h"
#include "net/conn/udp.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* NET_CONN_H_ */
/** @} */
