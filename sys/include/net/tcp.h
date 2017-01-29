/*
 * Copyright (C) 2017 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_tcp TCP
 * @ingroup     net
 * @brief       Provides TCP Header and helper functions
 * @see         <a href="https://tools.ietf.org/html/rfc793">
 *                  RFC 793
 *              </a>
 * @{
 *
 * @file
 * @brief      TCP Header
 *
 * @author     Simon Brummer <simon.brummer@posteo.de>
 */

#ifndef TCP_H
#define TCP_H

#include "byteorder.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief TCP header definition
 */
typedef struct __attribute__((packed)) {
    network_uint16_t src_port;    /**< source port, in network byte order */
    network_uint16_t dst_port;    /**< destination port, in network byte order */
    network_uint32_t seq_num;     /**< sequence number, in network byte order */
    network_uint32_t ack_num;     /**< Acknowledgement number, in network byte order */
    network_uint16_t off_ctl;     /**< Data Offset and control Bits in network byte order */
    network_uint16_t window;      /**< window, in network byte order */
    network_uint16_t checksum;    /**< checksum, in network byte order */
    network_uint16_t urgent_ptr;  /**< urgent pointer, in network byte order */
} tcp_hdr_t;

/**
 * @brief TCP Option definition
 */
typedef struct __attribute__((packed)) {
    uint8_t kind;     /**< kind field */
    uint8_t length;   /**< options Length in Byte */
    uint8_t *data;    /**< options value */
} tcp_hdr_opt_t;

#ifdef __cplusplus
}
#endif

#endif /* TCP_H */
/** @} */
