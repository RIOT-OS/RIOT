/**
 * Transport Layer types header
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup transport_layer
 * @{
 * @file
 * @brief   Transport Layer types
 * @author  Oliver Gesch <oliver.gesch@googlemail.com>
 * @author  Martin Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef SOCKET_BASE_TYPES_H_
#define SOCKET_BASE_TYPES_H_
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * UDP packet header length
 */
#define UDP_HDR_LEN 8

/**
 * TCP packet header length
 */
#define TCP_HDR_LEN 20

/**
 * UDP packet header
 *
 * @see <a href="http://tools.ietf.org/html/rfc768">RFC 768</a>
 */
typedef struct __attribute__((packed)) {
    uint16_t src_port;                  ///< source port
    uint16_t dst_port;                  ///< destination port
    uint16_t length;                    ///< payload length
    /**
     * internet checksum
     *
     * @see <a href="http://tools.ietf.org/html/rfc1071">RFC 1071</a>
     */
    uint16_t checksum;
} udp_hdr_t;

/**
 * TCP packet header
 *
 * @see <a href="http://tools.ietf.org/html/rfc793">RFC 793</a>
 */
typedef struct __attribute__((packed)) {
    uint16_t    src_port;               ///< source port
    uint16_t    dst_port;               ///< destination port
    uint32_t    seq_nr;                 ///< sequence number
    uint32_t    ack_nr;                 ///< acknowledgement number
    unsigned    flag_ns         :1;     ///< ECN-nonce concealment protection (since RFC 3540).
    unsigned    reserved        :3;     ///< for future use - set to zero
    unsigned    data_offset     :4;
    uint8_t     reserved_flags;         ///< TODO: break this down into another bitfield: flag_fin, flag_syn, etc
    uint16_t    window;                 ///< receiver window
    /**
     * internet checksum
     *
     * @see <a href="http://tools.ietf.org/html/rfc1071">RFC 1071</a>
     */
    uint16_t    checksum;
    uint16_t    urg_pointer;            ///< urgent pointer
} tcp_hdr_t;

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* SOCKET_BASE_TYPES_H_ */
