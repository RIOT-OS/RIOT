/*
 * Copyright (C) 2015-2017 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc_tcp
 *
 * @{
 *
 * @file
 * @brief       GNRC TCP transmission control block (TCB)
 *
 * @author      Simon Brummer <simon.brummer@posteo.de>
 */

#ifndef NET_GNRC_TCP_TCB_H
#define NET_GNRC_TCP_TCB_H

#include <stdint.h>
#include "ringbuffer.h"
#include "mutex.h"
#include "evtimer_msg.h"
#include "evtimer_mbox.h"
#include "msg.h"
#include "mbox.h"
#include "net/gnrc/pkt.h"
#include "config.h"

#ifdef MODULE_GNRC_IPV6
#include "net/gnrc/ipv6.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Transmission control block of GNRC TCP.
 */
typedef struct sock_tcp {
    uint8_t address_family;                   /**< Address Family of local_addr / peer_addr */
#ifdef MODULE_GNRC_IPV6
    uint8_t local_addr[sizeof(ipv6_addr_t)];  /**< Local IP address */
    uint8_t peer_addr[sizeof(ipv6_addr_t)];   /**< Peer IP address */
    int8_t  ll_iface;                         /**< Link layer interface id to use. */
#endif
    uint16_t local_port;   /**< Local connections port number */
    uint16_t peer_port;    /**< Peer connections port number */
    uint8_t state;         /**< Connections state */
    uint8_t status;        /**< A connections status flags */
    uint32_t snd_una;      /**< Send unacknowledged */
    uint32_t snd_nxt;      /**< Send next */
    uint16_t snd_wnd;      /**< Send window */
    uint32_t snd_wl1;      /**< SeqNo. from last window update */
    uint32_t snd_wl2;      /**< AckNo. from last window update */
    uint32_t rcv_nxt;      /**< Receive next */
    uint16_t rcv_wnd;      /**< Receive window */
    uint32_t iss;          /**< Initial sequence sumber */
    uint32_t irs;          /**< Initial received sequence number */
    uint16_t mss;          /**< The peers MSS */
    uint32_t rtt_start;    /**< Timer value for rtt estimation */
    int32_t rtt_var;       /**< Round trip time variance */
    int32_t srtt;          /**< Smoothed round trip time */
    int32_t rto;           /**< Retransmission timeout duration */
    uint8_t retries;       /**< Number of retransmissions */
    evtimer_msg_event_t event_retransmit; /**< Retransmission event */
    evtimer_msg_event_t event_timeout;    /**< Timeout event */
    evtimer_mbox_event_t event_misc;      /**< General purpose event */
    gnrc_pktsnip_t *pkt_retransmit;       /**< Pointer to packet in "retransmit queue" */
    mbox_t *mbox;            /**< TCB mbox for synchronization */
    uint8_t *rcv_buf_raw;    /**< Pointer to the receive buffer */
    ringbuffer_t rcv_buf;    /**< Receive buffer data structure */
    mutex_t fsm_lock;        /**< Mutex for FSM access synchronization */
    mutex_t function_lock;   /**< Mutex for function call synchronization */
    struct sock_tcp *next;   /**< Pointer next TCB */
} gnrc_tcp_tcb_t;

/**
 * @brief Transmission control block queue.
 */
typedef struct sock_tcp_queue {
    mutex_t lock;         /**< Mutex for access synchronization */
    gnrc_tcp_tcb_t *tcbs; /**< Pointer to TCB sequence */
    size_t tcbs_len;      /**< Number of TCBs behind member tcbs */
} gnrc_tcp_tcb_queue_t;

/**
 * @brief Static initializer for type gnrc_tcp_tcb_queue_t
 */
#define GNRC_TCP_TCB_QUEUE_INIT   { MUTEX_INIT, NULL, 0 }

#ifdef __cplusplus
}
#endif
#endif /* NET_GNRC_TCP_TCB_H */
/** @} */
