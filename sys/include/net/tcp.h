/*
 * Copyright (C) 2015-2017 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_tcp TCP
 * @ingroup     net
 * @brief       Provides TCP header and helper functions
 *
 * @{
 *
 * @file
 * @brief       TCP header and helper functions
 *
 * @author      Simon Brummer <simon.brummer@posteo.de>
 */

#ifndef TCP_H
#define TCP_H

#include "byteorder.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief TCP offset value boundries
 * @{
 */
#define TCP_HDR_OFFSET_MIN (0x05)
#define TCP_HDR_OFFSET_MAX (0x0F)
/** @} */

/**
 * @brief TCP Option "Kind" field defines
 * @{
 */
#define TCP_OPTION_KIND_EOL (0x00)  /**< "End of List"-Option */
#define TCP_OPTION_KIND_NOP (0x01)  /**< "No Operatrion"-Option */
#define TCP_OPTION_KIND_MSS (0x02)  /**< "Maximum Segment Size"-Option */
/** @} */

/**
 * @brief TCP Option Length Field Values
 * @{
 */
#define TCP_OPTION_LENGTH_MSS (0x04)  /**< MSS Option Size always 4 */
/** @} */

/**
 * @brief TCP header definition
 */
typedef struct __attribute__((packed)) {
    network_uint16_t src_port;     /**< source port, in network byte order */
    network_uint16_t dst_port;     /**< destination port, in network byte order */
    network_uint32_t seq_num;      /**< sequence number, in network byte order */
    network_uint32_t ack_num;      /**< Acknowledgement number, in network byte order */
    network_uint16_t off_ctl;      /**< Data Offset and control Bits in network byte order */
    network_uint16_t window;       /**< window, in network byte order */
    network_uint16_t checksum;     /**< checksum, in network byte order */
    network_uint16_t urgent_ptr;   /**< urgent pointer, in network byte order */
} tcp_hdr_t;

/**
 * @brief TCP Option Field Helper Structure
 */
typedef struct __attribute__((packed)) {
    uint8_t kind;     /**< TCP options Kind field */
    uint8_t length;   /**< TCP options Length field */
    uint8_t value[];  /**< Pointer to TCP options Value field */
} tcp_hdr_opt_t;

/**
 * @brief   Print the given TCP header to STDOUT
 *
 * @param[in] hdr           TCP header to print
 */
void tcp_hdr_print(tcp_hdr_t *hdr);

#ifdef __cplusplus
}
#endif

#endif /* TCP_H */
/** @} */
