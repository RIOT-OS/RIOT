/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup pkg_openthread_sokc
 * @{
 *
 * @file
 * @brief openthread-specific types
 *
 */
#ifndef SOCK_TYPES_H
#define SOCK_TYPES_H

#include "net/af.h"
#include <openthread.h>
#include "openthread/udp.h"
#include "thread.h"
#include "mbox.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SOCK_MBOX_SIZE
#define SOCK_MBOX_SIZE      (8)         /**< Size for gnrc_sock_reg_t::mbox_queue */
#endif

typedef struct 
{
    ipv6_addr_t ip_addr;
    uint16_t port;
    void *payload;
    size_t len;
} udp_pkt_t;

/**
 * @brief   sock @ref net_gnrc_netreg info
 * @internal
 */
typedef struct openthread_sock_reg {
    mbox_t mbox;                        /**< @ref core_mbox target for the sock */
    msg_t mbox_queue[SOCK_MBOX_SIZE];   /**< queue for gnrc_sock_reg_t::mbox */
} openthread_sock_reg_t;

/**
 * @brief   Raw IP sock type
 * @internal
 */
struct sock_ip {
};

/**
 * @brief   TCP sock type
 * @internal
 */
struct sock_tcp {
};

/**
 * @brief   TCP queue type
 */
struct sock_tcp_queue {
};

/**
 * @brief   UDP sock type
 * @internal
 */
struct sock_udp {
	openthread_sock_reg_t openthread_sock_reg;
	udp_pkt_t pkt;
	otUdpSocket ot_udp_socket;
	void *data;
	size_t len;
	size_t max_len;
    sock_udp_ep_t local;                /**< local end-point */
    sock_udp_ep_t remote;               /**< remote end-point */
    mbox_t mbox;                        /**< @ref core_mbox target for the sock */
    msg_t mbox_queue[SOCK_MBOX_SIZE];   /**< queue for gnrc_sock_reg_t::mbox */
    uint16_t flags;                     /**< option flags */
};

#ifdef __cplusplus
}
#endif

#endif /* SOCK_TYPES_H */
/** @} */
