/*
 * Copyright (C) 2015-2017 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_tcp TCP
 * @ingroup     net_gnrc
 * @brief       RIOT's tcp implementation for the gnrc stack
 *
 * @{
 *
 * @file
 * @brief       GNRC TCP transmission control block
 *
 * @author      Simon Brummer <simon.brummer@posteo.de>
 */

#ifndef GNRC_TCP_TCB_H
#define GNRC_TCP_TCB_H

#include <stdint.h>
#include "kernel_types.h"
#include "ringbuffer.h"
#include "xtimer.h"
#include "mutex.h"
#include "msg.h"
#include "net/gnrc/pkt.h"
#include "config.h"

#ifdef MODULE_GNRC_IPV6
#include "net/gnrc/ipv6.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Size of the tcbs internal message queue
 */
#define GNRC_TCP_TCB_MSG_QUEUE_SIZE (8U)

/**
 * @brief transmission control block of gnrc_tcp
 */
typedef struct _transmission_control_block {
    uint8_t address_family;   /**< Address Family of local_addr and peer_addr */
#ifdef MODULE_GNRC_IPV6
    uint8_t local_addr[sizeof(ipv6_addr_t)];  /**< local IP address */
    uint8_t peer_addr[sizeof(ipv6_addr_t)];   /**< peer IP address */
#endif
    uint16_t local_port;   /**< local connections port number */
    uint16_t peer_port;    /**< port connections port number */
    uint8_t state;         /**< Connections state */
    uint8_t status;        /**< A connections status flags */
    uint32_t snd_una;      /**< Send Unacknowledged */
    uint32_t snd_nxt;      /**< Send Next */
    uint16_t snd_wnd;      /**< Send Window */
    uint32_t snd_wl1;      /**< SeqNo. Last Windowupdate */
    uint32_t snd_wl2;      /**< AckNo. Last Windowupdate */
    uint32_t rcv_nxt;      /**< Receive Next */
    uint16_t rcv_wnd;      /**< Receive Window */
    uint32_t iss;          /**< Initial Sequence Number */
    uint32_t irs;          /**< Initial Received Sequence Number */
    uint16_t mss;          /**< The peers MSS */
    uint32_t rtt_start;    /**< Timer value for rtt estimation */
    int32_t rtt_var;       /**< Round Trip Time variance */
    int32_t srtt;          /**< Smoothed Round Trip Time */
    int32_t rto;           /**< Retransmission Timeout Duration */
    uint8_t retries;       /**< Number of Retransmissions */
    xtimer_t tim_tout;     /**< Timer struct for timeouts */
    msg_t msg_tout;        /**< Message, sent on timeouts */
    gnrc_pktsnip_t *pkt_retransmit;   /**< Pointer to Packet in "retransmit queue" */
    kernel_pid_t owner;               /**< PID of this connection handling thread */
    msg_t msg_queue[GNRC_TCP_TCB_MSG_QUEUE_SIZE];   /**< Tcb's message queue */
    uint8_t *rcv_buf_raw;    /**< Pointer to the receive buffer */
    ringbuffer_t rcv_buf;    /**< Receive Buffer data structure */
    mutex_t fsm_lock;        /**< Mutex for FSM access synchronization */
    mutex_t function_lock;   /**< Mutex for Function call synchronization */
    struct _transmission_control_block *next;   /**< Pointer next TCP connection */
} gnrc_tcp_tcb_t;

#ifdef __cplusplus
}
#endif
#endif /* GNRC_TCP_TCB_H */
/** @} */
