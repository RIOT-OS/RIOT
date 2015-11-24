/*
 * Copyright (C) 2015 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef GNRC_TCP_INTERNAL_HELPER_H_
#define GNRC_TCP_INTERNAL_HELPER_H_

#include "net/gnrc/netapi.h"
#include "net/gnrc/tcp/tcb.h"

/* Control Bit Masks */
#define MSK_FIN         0x0001
#define MSK_SYN         0x0002
#define MSK_RST         0x0004
#define MSK_PSH         0x0008
#define MSK_ACK         0x0010
#define MSK_URG         0x0020
#define MSK_FIN_ACK     0x0011
#define MSK_SYN_ACK     0x0012
#define MSK_RST_ACK     0x0014
#define MSK_SYN_FIN_ACK 0x0013
#define MSK_FIN_ACK_PSH 0x0019
#define MSK_CTL         0x003F
#define MSK_OFFSET      0xF000

#define MSG_TYPE_USER_TIMEOUT   (GNRC_NETAPI_MSG_TYPE_ACK + 1)
#define MSG_TYPE_RETRANSMISSION (GNRC_NETAPI_MSG_TYPE_ACK + 2)
#define MSG_TYPE_TIMEWAIT       (GNRC_NETAPI_MSG_TYPE_ACK + 3)
#define MSG_TYPE_NOTIFY_USER    (GNRC_NETAPI_MSG_TYPE_ACK + 4)

/**
 * @brief Write data into a connections receive buffer
 *
 * @param[in]  tcb        tcb that contains receive buffer
 * @param[out] src        data source that should be written
 * @param[in]  nBytes     number of bytes that should be written
 *
 * @return                number of actually written bytes.
 */
size_t _helper_rcv_buf_write(gnrc_tcp_tcb_t* tcb, void* src, size_t nBytes);

/**
 * @brief Read data from a connections receive buffer
 *
 * @param[in]  tcb        receive buffer container tcb
 * @param[out] dst        buffer where data should be read into
 * @param[in]  nBytes     number of bytes that should be read
 *
 * @return                number of actually read bytes.
 */
size_t _helper_rcv_buf_read(gnrc_tcp_tcb_t* tcb, void* dst, size_t nBytes);

/**
 * TODO:
 */
int8_t _helper_rcv_buf_empty(gnrc_tcp_tcb_t* tcb);

/**
 * @brief Clears retransmit mechanism and stop possibly startet timers for a connection.
 *
 * @param[in] tcb     transmission control block, to specify a connection
 */
void _clear_retransmit(gnrc_tcp_tcb_t* tcb);

/**
 * @brief Prints Transmission control block
 *
 * @param[in] tcb     tcb to print
 */
void _gnrc_tcp_tcb_print (const gnrc_tcp_tcb_t *tcb);

/**
 * @brief Prints TCP Header
 *
 * @param[in] hdr     tcp header to print
 */
void _gnrc_tcp_hdr_print (const tcp_hdr_t *hdr);

#endif
