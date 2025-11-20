/*
 * Copyright (C) 2015-2017 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

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

#include "byteorder.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief TCP offset value boundaries.
 * @{
 */
#define TCP_HDR_OFFSET_MIN (0x05) /**< Header offset minimum value */
#define TCP_HDR_OFFSET_MAX (0x0F) /**< Header offset maximum value */
/** @} */

/**
 * @brief TCP Option "Kind"-field defines.
 * @{
 */
#define TCP_OPTION_KIND_EOL (0x00)  /**< "End of List"-Option */
#define TCP_OPTION_KIND_NOP (0x01)  /**< "No Operation"-Option */
#define TCP_OPTION_KIND_MSS (0x02)  /**< "Maximum Segment Size"-Option */
/** @} */

/**
 * @brief TCP option "length"-field values.
 * @{
 */
#define TCP_OPTION_LENGTH_MIN (2U)    /**< Minimum option field size in bytes */
#define TCP_OPTION_LENGTH_MSS (0x04)  /**< MSS Option Size always 4 */
/** @} */

/**
 * @brief TCP header definition
 */
typedef struct __attribute__((packed)) {
    network_uint16_t src_port;     /**< Source port, in network byte order */
    network_uint16_t dst_port;     /**< Destination port, in network byte order */
    network_uint32_t seq_num;      /**< Sequence number, in network byte order */
    network_uint32_t ack_num;      /**< Acknowledgement number, in network byte order */
    network_uint16_t off_ctl;      /**< Data Offset and control Bits in network byte order */
    network_uint16_t window;       /**< Window, in network byte order */
    network_uint16_t checksum;     /**< Checksum, in network byte order */
    network_uint16_t urgent_ptr;   /**< Urgent pointer, in network byte order */
} tcp_hdr_t;

/**
 * @brief TCP option field helper structure
 */
typedef struct __attribute__((packed)) {
    uint8_t kind;     /**< TCP options "Kind" field */
    uint8_t length;   /**< TCP options "Length" field */
    uint8_t value[];  /**< TCP options "Value" field */
} tcp_hdr_opt_t;

/**
 * @brief   Print the given TCP header to STDOUT
 *
 * @param[in] hdr   TCP header to print
 */
void tcp_hdr_print(tcp_hdr_t *hdr);

#ifdef __cplusplus
}
#endif

/** @} */
