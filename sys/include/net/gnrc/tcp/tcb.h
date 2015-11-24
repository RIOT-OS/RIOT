/*
 * Copyright (C) 2015 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef GNRC_TCP_TCB_H_
#define GNRC_TCP_TCB_H_

#include "msg.h"
#include "mutex.h"
#include "xtimer.h"
#include "ringbuffer.h"
#include "net/gnrc/netreg.h"
#include "net/gnrc/tcp/config.h"
#include "net/gnrc/tcp/hdr.h"
#include "net/gnrc/tcp/fsm.h"

/**
 * @brief Transmission Control Block.
 *
 * Stores all Information needed for a single connection.
 */
typedef struct __attribute__((packed)) tcb {
    gnrc_tcp_fsm_state_t state; /**< Current State of this connection */

    /* Multiplexing relevant information */
    uint8_t* peer_addr;      /**< Pointer to Peer Address, this connection sends to */
    size_t   peer_addr_len;  /**< Peer Address Len */
    uint16_t peer_port;      /**< Peer Port, peer is listening on */
    uint16_t local_port;     /**< Local Port, this connection is listening on */
    uint8_t  options;        /**< Options used to configure this connection */

    /* Sliding Window Mechanism Variables*/
    /* Send Pointers */
    uint32_t snd_una;  /**< Send Unackknowledged: Bytes sent and not acknowledged */
    uint32_t snd_nxt;  /**< Send Next: Begin Bytes to send to the Peer.*/
    uint16_t snd_wnd;  /**< Send Window: Size of Send Window. Usable size = SND_UNA + SND_WND - SND_NXT */
    uint16_t snd_ur;            /**< Send Urgent Pointer */
    uint32_t snd_wl1;           /**< Send seqno for last window update */
    uint32_t snd_wl2;           /**< Send ackno for last window update */
    uint32_t iss;               /**< Initial Send Sequence Number */

    /* Receive Pointers */
    uint32_t rcv_nxt;           /**< Receive Next: Begin Bytes expected to receive from peer */
    uint16_t rcv_wnd;           /**< Receive Window: Number of Bytes willing to receive */
    uint16_t rcv_up;            /**< Receive Urgent Pointer */
    uint32_t irs;               /**< Intial Receive Sequence Number */

    /* Receive Buffer (Ringbuffer) */
    uint8_t buf[GNRC_TCP_RCV_BUF_SIZE]; /**< Receive Buffer */
    ringbuffer_t rcv_buf;               /**< Receive Buffer struct */

    /* Max Window Size */
    uint16_t mss;               /**< Peer Maximum Window Size */

    /* Retransmission */
    gnrc_pktsnip_t* pkt_retransmit;

    /* Linked List Stuff */
    kernel_pid_t owner;                  /**< Owner of this TCB */
    struct tcb * next;                   /**< TCB next Pointer */

    /* Misc */
    gnrc_netreg_entry_t netreg_entry;    /**< Netreg entry for this connection */
    mutex_t mtx;                         /**< Mutex used for fsm locking. */
    xtimer_t timer_timeout;              /**< Timer used for various timeouts */
    msg_t msg_timeout;                   /**< Message to send in case of retransmit */
    msg_t msg_queue[TCB_MSG_QUEUE_SIZE]; /**< Message Queue, stores messageing for tcb owner */
} gnrc_tcp_tcb_t;

/**
 * @brief Head of tcb linked list. The eventloop needs 
 *        to look up the affected tcbs in case of an incomming paket.
 *        ng_tcp_tcb_init() adds tcb to this list.
 */
extern gnrc_tcp_tcb_t* _head_list_tcb;

#endif
