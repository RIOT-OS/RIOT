/*
 * Copyright (C) 2015 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_net_tcp TCP
 * @ingroup     net
 * @breif       RIOT's implementation of the TCP protocol
 *
 * @{
 *
 * @file
 * @brief       TCP interface definition
 *
 * @author      Simon Brummer <simon.brummer@haw-hamburg.de>
 * @}
 */

#ifndef GNRC_TCP_H_
#define GNRC_TCP_H_

#include "byteorder.h"
#include "timex.h"
#include "net/gnrc/netapi.h"
#include "net/gnrc/netreg.h"
#include "net/gnrc/nettype.h"

#ifdef MODULE_GNRC_IPV6
#include "net/gnrc/ipv6.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    Default stack size for TCP thread
 */
#ifndef GNRC_TCP_STACK_SIZE
#define GNRC_TCP_STACK_SIZE   THREAD_STACKSIZE_DEFAULT
#endif

/**
 * @brief    Default name for TCP thread
 */
#ifndef GNRC_TCP_THREAD_NAME
#define GNRC_TCP_THREAD_NAME  "_NGTCP"
#endif

/**
 * @brief    TODO: Description
 */
#ifndef GNRC_TCP_PRIO
#define GNRC_TCP_PRIO   (THREAD_PRIORITY_MAIN - 2)
#endif

/**
 * @brief Size of TCP threads message queue
 */
#ifndef GNRC_TCP_MSG_QUEUE_SIZE
#define GNRC_TCP_MSG_QUEUE_SIZE 4
#endif

/**
 * @brief TCP Options could contain up to 10 32-Bit values of Information.
 */
#define MAX_OPTIONS 10

/**
 * @brief TCP Header size without options(20 Byte)
 */
#define GNRC_TCP_HDR_SIZE 20

/**
 * @brief TCP Header size with maximum possible options(60 Byte)
 */
#define GNRC_TCP_HDR_SIZE_WITH_OPTIONS 60

/* If IPv6 is used. Get MSS from MTU*/
#ifdef MODULE_GNRC_IPV6
#ifndef GNRC_TCP_DEFAULT_MSS
#define GNRC_TCP_DEFAULT_MSS (GNRC_IPV6_NETIF_DEFAULT_MTU - GNRC_TCP_HDR_SIZE)
#endif
#endif

/* Maximum Segment Size. Maximum amount of payload in bytes */
#ifndef GNRC_TCP_DEFAULT_MSS
#define GNRC_TCP_DEFAULT_MSS 576
#endif

#define AI_PASSIVE (1<<0)

/* Retransmit Queue Size */
#ifndef GNRC_TCP_RETRANSMIT_QUEUE_SIZE
#define GNRC_TCP_RETRANSMIT_QUEUE_SIZE 5
#endif

/* Maximum Number of Retransmits per Packet */
#ifndef GNRC_TCP_MAX_RETRANSMITS
#define GNRC_TCP_MAX_RETRANSMITS 5
#endif

/**
 *  @brief States for the TCP State Maschine
 */
typedef enum {CLOSED,
              LISTEN,
              SYN_SENT,
              SYN_RCVD,
              ESTABLISHED,
              CLOSE_WAIT,
              LAST_ACK,
              FIN_WAIT_1,
              FIN_WAIT_2,
              CLOSING,
              TIME_WAIT
} gnrc_tcp_states_t;

/**
 * @brief TCP header definition
 */
typedef struct __attribute__((packed)) {
    network_uint16_t src_port;              /**< source port, in network byte order */
    network_uint16_t dst_port;              /**< destination port, in network byte order */
    network_uint32_t seq_num;               /**< sequence number, in network byte order */
    network_uint32_t ack_num;               /**< acknowledgement number, in network byte order */
    network_uint16_t off_ctl;               /**< Data Offset and control Bits in network byte order */
    network_uint16_t window;                /**< window, in network byte order */
    network_uint16_t checksum;              /**< checksum, in network byte order */
    network_uint16_t urgent_ptr;            /**< urgent pointer, in network byte order */
    network_uint32_t options[MAX_OPTIONS];  /**< Option Fields (Optional) */
} gnrc_tcp_hdr_t;

typedef struct __attribute__((packed)) {
  uint8_t no_of_retries;
  gnrc_pktsnip_t* pkt;
} gnrc_tcp_queue_entry_t;

/**
 * @brief Transmission Control Block.
 *
 * Stores all Information needed for a single connection.
 */
typedef struct __attribute__((packed)) {
    gnrc_tcp_states_t state;     /**< Current State of this connection */

    /* Multiplexing relevant information */
    uint8_t* peer_addr;        /**< Pointer to Peer Address, this connection sends to */
    size_t   peer_addr_len;    /**< Peer Address Len */
    uint16_t peer_port;        /**< Peer Port, peer is listening on */
    uint16_t local_port;       /**< Local Port, this connection is listening on */
    uint8_t  options;          /**< Options used to configure this connection */

    /* Sliding Window Mechanism Variables*/
    /* Send Pointers */
    uint32_t snd_una;          /**< Send Unackknowledged: Bytes sent and not acknowledged */
    uint32_t snd_nxt;          /**< Send Next: Begin Bytes to send to the Peer.*/
    uint16_t snd_wnd;          /**< Send Window: Size of Send Window. Usable size = SND_UNA + SND_WND - SND_NXT */
    uint16_t snd_ur;           /**< Send Urgent Pointer */
    uint32_t snd_wl1;          /**< Send seqno for last window update */
    uint32_t snd_wl2;          /**< Send ackno for last window update */
    uint32_t iss;              /**< Initial Send Sequence Number */

    /* Recveive Pointers */
    uint32_t rcv_nxt;          /**< Receive Next: Begin Bytes expected to receive from peer */
    uint16_t rcv_wnd;          /**< Receive Window: Number of Bytes willing to receive */
    uint16_t rcv_up;           /**< Receive Urgent Pointer */
    uint32_t irs;              /**< Intial Receive Sequence Number */

    /* Max Window Size */
    uint16_t mss;              /**< Peer Maximum Window Size */

    /* Next paket to send */
    gnrc_pktsnip_t *cur_pkt;
    gnrc_tcp_hdr_t *cur_tcp_hdr;
    size_t cur_seg_len;

    /* Send and Receive Buffers */
    uint8_t *snd_buf;
    size_t   snd_buf_size;
    uint8_t *rcv_buf;
    size_t   rcv_buf_size;

    /* Retransmit Queue */
    gnrc_tcp_queue_entry_t ret_queue[GNRC_TCP_RETRANSMIT_QUEUE_SIZE];
    uint8_t ret_size;
    timex_t ret_tout;

    /* Misc */
    kernel_pid_t owner;
    void* next;
    gnrc_netreg_entry_t netreg_entry;
} gnrc_tcp_tcb_t;

/**
 * @brief Initialize and start TCP
 *
 * @param[in] priority      priority to use for the TCP thread
 *
 * @return                  PID of TCP thread on success
 * @return                  -1 if thread is already running.
 * @return                  -EINVAL, if priority is greater than or equal SCHED_PRIO_LEVELS
 * @return                  -EOVERFLOW, if there are too many threads running.
 */
int gnrc_tcp_init(void);

void gnrc_tcp_hdr_print(const gnrc_tcp_hdr_t *hdr);

/**
 * @brief Set checksum calculated from tcp and network-layer header in tcp-header.
 *
 * @param[in] hdr          ng_pktsnip that contains tcp header.
 * @param[in] pseudo_hdr   ng_pktsnip that contains networklayer header.
 *
 * @return                 zero on succeed.
 * @return                 -EFAULT if hdr or pseudo_hdr were NULL
 * @return                 -EBADMSG if hdr is not of type GNRC_NETTYPE_TCP
 * @return                 -ENOENT if pseudo_hdr protocol is unsupported.
 */
int gnrc_tcp_calc_csum(const gnrc_pktsnip_t *hdr, const gnrc_pktsnip_t *pseudo_hdr);

/**
 * TODO: Header Stuff
 */
gnrc_pktsnip_t *gnrc_tcp_hdr_build(gnrc_pktsnip_t *payload, uint8_t *src, size_t src_len,
                               uint8_t *dst, size_t dst_len);

/**
 * @brief Initialize transmission control block. Should be called before tcb usage.
 *
 * @param[out] tcb   transmision control block to be initialized.
 *
 * @return           zero on success.
 */
int8_t gnrc_tcp_tcb_init(gnrc_tcp_tcb_t *tcb);

/**
 * @brief TODO
 */
int8_t gnrc_tcp_tcb_destroy(gnrc_tcp_tcb_t *tcb);

/**
 * @brief Opens a connection. Block until a connection is established, closed or an critical error occured.
 *
 * @param[in/out] tcb        transmission control block to handle this connection
 * @param[in/out] addr       address buffer to store peer information
 * @param[in]     addr_len   size of addr
 * @param[in]     port       portno. to connect to in active mode, portno. to listen on in passive mode
 * @param[in]     flags      configuration flags for this connection, prefix AI is used for them.
 *
 * @return         zero on success
 * @return         negative value on error
 * @return         -EFAULT if addr was NULL or addr_len was zero
 * @return         -EALREADY if connection is already in use
 * @return         -EDESTADDRREQ if destination is unspecified
 */

int8_t gnrc_tcp_open(gnrc_tcp_tcb_t *tcb, uint16_t local_port, uint8_t *peer_addr, size_t peer_addr_len, uint16_t peer_port, uint8_t options);

/**
 * @brief TODO
 */
int8_t gnrc_tcp_send(gnrc_tcp_tcb_t *tcb, uint8_t *buf, size_t byte_count, bool push, bool urgent);

/**
 * @brief TODO
 */
int8_t gnrc_tcp_recv(gnrc_tcp_tcb_t *tcb);

/**
 * @brief TODO
 */
int8_t gnrc_tcp_close(gnrc_tcp_tcb_t *tcb);

#ifdef __cpluscplus
}
#endif

#endif /* GNRC_TCP_H_ */
/** @} */
