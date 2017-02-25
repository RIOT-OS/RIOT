/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    emb6_conn_udp   udp_conn wrapper for emb6
 * @ingroup     emb6
 * @brief       UDP conn for emb6
 *
 * For this implementation to receive with an open connection only with one
 * thread at once. If you use @ref conn_udp_recvfrom() with more than one thread
 * simultaneously, it will return `-EALREADY`.
 *
 * @{
 *
 * @file
 * @brief   UDP conn definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef EMB6_CONN_UDP_H
#define EMB6_CONN_UDP_H

#include <stdint.h>

#include "kernel_types.h"
#include "mutex.h"
#include "net/ipv6/addr.h"

#include "uip.h"
#include "udp-socket.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief @ref net_conn_udp definition for emb6
 */
struct conn_udp {
    struct udp_socket sock;         /**< emb6 internal socket */
    mutex_t mutex;                  /**< mutex for the connection */
    kernel_pid_t waiting_thread;    /**< thread waiting for an incoming packet
                                     *   on this connection */
    struct {
        uint16_t src_port;          /**< source port */
        const ipv6_addr_t *src;     /**< source address */
        const void *data;           /**< data of received packet */
        size_t datalen;             /**< length of received packet data */
    } recv_info;                    /**< info on received packet */
};

#ifdef __cplusplus
}
#endif

#endif /* EMB6_CONN_UDP_H */
/** @} */
