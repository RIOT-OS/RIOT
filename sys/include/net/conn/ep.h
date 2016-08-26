/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_conn_ep End points
 * @ingroup     net_conn
 * @brief       Protocol specific end points for connectivity objects.
 * @{
 *
 * @file
 * @brief   Type definitions of protocol specific end points for connectivity
 *          objects.
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef CONN_EP_H_
#define CONN_EP_H_

#include <stdint.h>

#include "net/conn/addr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Special @ref net_netif "netif" ID for "any interface".
 */
#define CONN_EP_ANY_NETIF   (0)

/**
 * @brief   An end point for a raw IPv4/IPv6 connectivity object.
 */
typedef struct {
    conn_addr_ip_t addr;    /**< IP address */
    int family;             /**< family of conn_ep_ip_t::addr */

    /**
     * @brief   network interface ID as defined in @ref net_netif
     *
     * Use @ref CONN_EP_ANY_NETIF for any interface.
     * For reception this is the local interface the message came over,
     * for transmission, this is the local interface the message should be send
     * over
     */
    uint16_t netif;
} conn_ep_ip_t;

/**
 * @brief   An end point for a UDP connectivity object.
 */
typedef struct {
    conn_addr_ip_t addr;    /**< IP address */
    int family;             /**< family of conn_ep_udp_t::addr */

    /**
     * @brief   network interface ID as defined in @ref net_netif
     *
     * Use @ref CONN_EP_ANY_NETIF for any interface.
     * For reception this is the local interface the message came over,
     * for transmission, this is the local interface the message should be send
     * over
     */
    uint16_t netif;
    uint16_t port;          /**< port for the UDP end point */
} conn_ep_udp_t;

/**
 * @brief   An end point for a TCP connectivity object.
 */
typedef struct {
    conn_addr_ip_t addr;    /**< IP address */
    int family;             /**< family of conn_ep_tcp_t::addr */

    /**
     * @brief   network interface ID as defined in @ref net_netif
     *
     * Use @ref CONN_EP_ANY_NETIF for any interface.
     * For reception this is the local interface the message came over,
     * for transmission, this is the local interface the message should be send
     * over
     */
    uint16_t netif;
    uint16_t port;          /**< port for the TCP end point */
} conn_ep_tcp_t;

#ifdef __cplusplus
}
#endif

#endif /* CONN_EP_H_ */
/** @} */
