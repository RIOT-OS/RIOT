/*
 * Copyright (C) 2015 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef GNRC_TCP_HDR_H_
#define GNRC_TCP_HDR_H_

#include "byteorder.h"

/**
 * @brief TCP Options could contain up to 10 32-Bit values of Information.
 */
#define TCP_MAX_HDR_OPTIONS 10

/**
 * @brief TCP header definition
 */
typedef struct __attribute__((packed)) {
    network_uint16_t src_port;                     /**< source port, in network byte order */
    network_uint16_t dst_port;                     /**< destination port, in network byte order */
    network_uint32_t seq_num;                      /**< sequence number, in network byte order */
    network_uint32_t ack_num;                      /**< Acknowledgement number, in network byte order */
    network_uint16_t off_ctl;                      /**< Data Offset and control Bits in network byte order */
    network_uint16_t window;                       /**< window, in network byte order */
    network_uint16_t checksum;                     /**< checksum, in network byte order */
    network_uint16_t urgent_ptr;                   /**< urgent pointer, in network byte order */
    network_uint32_t options[TCP_MAX_HDR_OPTIONS]; /**< Option Fields (Optional) */
} tcp_hdr_t;

void gnrc_tcp_hdr_print(const tcp_hdr_t *hdr);

#endif
