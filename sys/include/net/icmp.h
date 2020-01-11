/*
 * Copyright (C) 2015 José Ignacio Alamos <jialamos@uc.cl>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_icmp  ICMPV4
 * @ingroup     net_ipv4
 * @brief       Provides types related to ICMPv4
 * @see         <a href="https://tools.ietf.org/html/rfc792">
 *                  RFC 792
 *              </a>
 * @{
 *
 * @file
 * @brief   ICMPv4 type and function definitions
 *
 * @author  José Ignacio Alamos <jialamos@uc.cl>
 */
#ifndef NET_ICMP_H
#define NET_ICMP_H

#include "byteorder.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief   Echo request and response message format.
 *
 * @see <a href="https://tools.ietf.org/html/rfc792#page-14">
 *          RFC 792, page 14
 *      </a>
 */
typedef struct __attribute__((packed)){
    uint8_t type;           /**< message type */
    uint8_t code;           /**< message code */
    network_uint16_t csum;  /**< checksum */
    network_uint16_t id;    /**< identifier */
    network_uint16_t sn;    /**< sequence number */
} icmp_echo_t;
typedef icmp_echo_t icmpv4_echo_t;

/**
 * @brief   Packet too big message format.
 * @extends icmpv4_hdr_t
 *
 * @see <a href="https://tools.ietf.org/html/rfc4443#section-3.2">
 *          RFC 4443, section 3.2
 *      </a>
 */
typedef struct __attribute__((packed)){
    uint8_t type;           /**< message type */
    uint8_t code;           /**< message code */
    network_uint16_t csum;  /**< checksum */
} icmp_hdr_t;
typedef icmp_hdr_t icmpv4_hdr_t;

/**
 * @brief   Destination unreachable message format.
 * @extends icmpv4_hdr_t
 */
typedef struct __attribute__((packed)) {
    uint8_t type;               /**< message type */
    uint8_t code;               /**< message code */
    network_uint16_t csum;      /**< checksum */
    network_uint32_t unused;    /**< unused field */
} icmp_error_dst_unr_t;
typedef icmp_error_dst_unr_t icmpv4_error_dst_unr_t;

/**
 * @brief   Packet too big message format.
 * @extends icmpv4_hdr_t
 */
typedef struct __attribute__((packed)) {
    uint8_t type;           /**< message type */
    uint8_t code;           /**< message code */
    network_uint16_t csum;  /**< checksum */
    network_uint32_t mtu;   /**< MTU */
} icmp_error_pkt_too_big_t;
typedef icmp_error_pkt_too_big_t icmpv4_error_pkt_too_big_t;

/**
 * @brief   Parameter problem message format.
 * @extends icmpv4_hdr_t
 */
typedef struct __attribute__((packed)) {
    uint8_t type;           /**< message type */
    uint8_t code;           /**< message code */
    network_uint16_t csum;  /**< checksum */
    network_uint32_t ptr;   /**< pointer */
} icmp_error_param_prob_t;
typedef icmp_error_param_prob_t icmpv4_error_param_prob_t;

#define ICMPV4_ECHO_REP     (0)   /**< Echo reply message (pong) */
#define ICMPV4_DST_UNR      (3)   /**< Destination unreachable */
#define ICMPV4_ECHO_REQ     (8)   /**< Echo request message (ping) */
#define ICMPV4_TIME_EXC     (11)  /**< Time exceeded */
#define ICMPV4_PARAM_PROB   (12)  /**< Perameter problem */
#define ICMPV4_PKT_TOO_BIG  ICMPV4_PARAM_PROB

/**
 * @{
 * @name Codes for destination unreachable messages
 *
 * @anchor net_ICMPV4_error_dst_unr_codes
 * @see <a href="https://tools.ietf.org/html/rfc4443#section-3.1">
 *          RFC 4443, section 3.1
 *      </a>
 */
#define ICMPV4_ERROR_DST_UNR_NO_ROUTE   (0) /**< no route to destination */
#define ICMPV4_ERROR_DST_UNR_PROHIB     (1) /**< communication with
                                             *   destination administratively
                                             *   prohibited */
#define ICMPV4_ERROR_DST_UNR_SCOPE      (2) /**< beyond scope of source address */
#define ICMPV4_ERROR_DST_UNR_ADDR       (3) /**< address unreachable */
#define ICMPV4_ERROR_DST_UNR_PORT       (4) /**< port unreachable */
#define ICMPV4_ERROR_DST_UNR_POLICY     (5) /**< source address failed ingress/egress
                                             *   policy */
#define ICMPV4_ERROR_DST_UNR_REJECT     (6) /**< reject route to destination */

#ifdef __cplusplus
}
#endif

#endif /* NET_ICMP_H */
/** @} */
