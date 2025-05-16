/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    net_udp UDP
 * @ingroup     net
 * @brief       Provides UDP header and helper functions
 * @see         <a href="https://tools.ietf.org/html/rfc768">
 *                  RFC 768
 *              </a>
 * @{
 *
 * @file
 * @brief   UDP header and helper functions definition
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include "byteorder.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   UDP header
 */
typedef struct __attribute__((packed)) {
    network_uint16_t src_port;      /**< source port */
    network_uint16_t dst_port;      /**< destination port */
    network_uint16_t length;        /**< payload length (including the header) */
    network_uint16_t checksum;      /**< checksum */
} udp_hdr_t;

/**
 * @brief   Print the given UDP header to STDOUT
 *
 * @param[in] hdr           UDP header to print
 */
void udp_hdr_print(udp_hdr_t *hdr);

#ifdef __cplusplus
}
#endif

/** @} */
