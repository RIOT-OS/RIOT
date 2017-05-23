/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup pkg_emb6_sock  emb6-specific implementation of the sock API
 * @ingroup  pkg_emb6
 * @brief
 * @{
 *
 * @file
 * @brief
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef SOCK_TYPES_H
#define SOCK_TYPES_H

#include <stdatomic.h>

#include "mbox.h"
#include "mutex.h"
#include "net/ipv6/addr.h"

#include "uip.h"
#include "udp-socket.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SOCK_MBOX_SIZE
#define SOCK_MBOX_SIZE  (2)
#endif

#ifndef SOCK_HAS_IPV6
#error "emb6 only runs with IPv6 support"
#endif

/**
 * @brief @ref net_sock_udp definition for emb6
 */
struct sock_udp {
    struct udp_socket sock;             /**< emb6 internal socket */
    mutex_t mutex;                      /**< mutex for the connection */
    mbox_t mbox;                        /**< mbox for receiving */
    msg_t mbox_queue[SOCK_MBOX_SIZE];   /**< queue for mbox */
    atomic_int receivers;               /**< current number of recv calls */
    struct {
        const ipv6_addr_t *src;         /**< source address */
        const void *data;               /**< data of received packet */
        size_t datalen;                 /**< length of received packet data */
        uint16_t src_port;              /**< source port */
    } recv_info;                        /**< info on received packet */
};

#ifdef __cplusplus
}
#endif

#endif /* SOCK_TYPES_H */
/** @} */
