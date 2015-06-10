/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_icmpv6  ICMPV6
 * @ingroup     net_ipv6
 * @brief       Provides types related to ICMPv6
 * @see         <a href="https://tools.ietf.org/html/rfc4443">
 *                  RFC 4443
 *              </a>
 * @{
 *
 * @file
 * @brief   ICMPv6 type and function definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef ICMPV6_H_
#define ICMPV6_H_

#include <stdint.h>

#include "byteorder.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @{
 * @name Error message types
 * @see <a href="http://www.iana.org/assignments/icmpv6-parameters/icmpv6-parameters.xhtml#icmpv6-parameters-2">
 *          IANA, ICMPv6 "type" Numbers
 *      </a>
 */
#define ICMPV6_DST_UNR      (1)     /**< Destination unreachable message */
#define ICMPV6_PKT_TOO_BIG  (2)     /**< Packet Too Big message */
#define ICMPV6_TIME_EXC     (3)     /**< Time Exceeded message */
#define ICMPV6_PARAM_PROB   (4)     /**< Parameter Problem message */
#define ICMPV6_ERR_EXP1     (100)   /**< message type for private experimentation */
#define ICMPV6_ERR_EXP2     (101)   /**< message type for private experimentation */
/**
 * @}
 */

/**
 * @{
 * @name Informational message types
 * @see <a href="http://www.iana.org/assignments/icmpv6-parameters/icmpv6-parameters.xhtml#icmpv6-parameters-2">
 *          IANA, ICMPv6 "type" Numbers
 *      </a>
 */
#define ICMPV6_ECHO_REQ     (128)   /**< Echo request message (ping) */
#define ICMPV6_ECHO_REP     (129)   /**< Echo reply message (pong) */
#define ICMPV6_RTR_SOL      (133)   /**< NDP router solicitation message */
#define ICMPV6_RTR_ADV      (134)   /**< NDP router advertisement message */
#define ICMPV6_NBR_SOL      (135)   /**< NDP neighbor solicitation message */
#define ICMPV6_NBR_ADV      (136)   /**< NDP neighbor advertisement message */
#define ICMPV6_REDIRECT     (137)   /**< NDP redirect message */
#define ICMPV6_RPL_CTRL     (155)   /**< RPL control message */
#define ICMPV6_DAR          (157)   /**< Duplicate address request */
#define ICMPV6_DAC          (158)   /**< Duplicate address confirmation */
#define ICMPV6_INF_EXP1     (200)   /**< message type for private experimentation */
#define ICMPV6_INF_EXP2     (201)   /**< message type for private experimentation */
/**
 * @}
 */

/**
 * @{
 * @name Codes for destination unreachable messages
 *
 * @see <a href="https://tools.ietf.org/html/rfc4443#section-3.1">
 *          RFC 4443, section 3.1
 *      </a>
 */
#define ICMPV6_ERROR_DST_UNR_NO_ROUTE   (0) /**< no route to destination */
#define ICMPV6_ERROR_DST_UNR_PROHIB     (1) /**< communictation with
                                             *   destination administratively
                                             *   prohibited */
#define ICMPV6_ERROR_DST_UNR_SCOPE      (2) /**< beyond scope of source address */
#define ICMPV6_ERROR_DST_UNR_ADDR       (3) /**< address unreachable */
#define ICMPV6_ERROR_DST_UNR_PORT       (4) /**< port unreachable */
#define ICMPV6_ERROR_DST_UNR_POLICY     (5) /**< source address failed ingress/egress
                                             *   policy */
#define ICMPV6_ERROR_DST_UNR_REJECT     (6) /**< reject route to destination */
/**
 * @}
 */

/**
 * @{
 * @name Codes for time exceeded messages
 *
 * @see <a href="https://tools.ietf.org/html/rfc4443#section-3.3">
 *          RFC 4443, section 3.3
 *      </a>
 */
#define ICMPV6_ERROR_TIME_EXC_HL     (0) /**< hop limit exceeded in transit */
#define ICMPV6_ERROR_TIME_EXC_FRAG   (1) /**< fragment reassembly time exceeded */
/**
 * @}
 */

/**
 * @{
 * @name Codes for parameter problem messages
 *
 * @see <a href="https://tools.ietf.org/html/rfc4443#section-3.4">
 *          RFC 4443, section 3.4
 *      </a>
 */
#define ICMPV6_ERROR_PARAM_PROB_HDR_FIELD   (0) /**< errorneous header field
                                                 *   encountered */
#define ICMPV6_ERROR_PARAM_PROB_NH          (1) /**< unrecognized next header
                                                 *   field encountered */
#define ICMPV6_ERROR_PARAM_PROB_OPT         (2) /**< unrecognized IPv6 option
                                                 *   field encountered */
/**
 * @}
 */

/**
 * @brief   General ICMPv6 message format.
 *
 * @see <a href="https://tools.ietf.org/html/rfc4443#section-2.1">
 *          RFC 4443, section 2.1
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;           /**< message type */
    uint8_t code;           /**< message code */
    network_uint16_t csum;  /**< checksum */
} icmpv6_hdr_t;

/**
 * @brief   Destination unreachable message format.
 * @extends icmpv6_hdr_t
 *
 * @see <a href="https://tools.ietf.org/html/rfc4443#section-3.1">
 *          RFC 4443, section 3.1
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;               /**< message type */
    uint8_t code;               /**< message code */
    network_uint16_t csum;      /**< checksum */
    network_uint32_t unused;    /**< unused field */
} icmpv6_error_dst_unr_t;

/**
 * @brief   Packet too big message format.
 * @extends icmpv6_hdr_t
 *
 * @see <a href="https://tools.ietf.org/html/rfc4443#section-3.2">
 *          RFC 4443, section 3.2
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;           /**< message type */
    uint8_t code;           /**< message code */
    network_uint16_t csum;  /**< checksum */
    network_uint32_t mtu;   /**< MTU */
} icmpv6_error_pkt_too_big_t;

/**
 * @brief   Time exceeded message format.
 * @extends icmpv6_hdr_t
 *
 * @see <a href="https://tools.ietf.org/html/rfc4443#section-3.3">
 *          RFC 4443, section 3.3
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;               /**< message type */
    uint8_t code;               /**< message code */
    network_uint16_t csum;      /**< checksum */
    network_uint32_t unused;    /**< unused field */
} icmpv6_error_time_exc_t;

/**
 * @brief   Parameter problem message format.
 * @extends icmpv6_hdr_t
 *
 * @see <a href="https://tools.ietf.org/html/rfc4443#section-3.4">
 *          RFC 4443, section 3.3
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;           /**< message type */
    uint8_t code;           /**< message code */
    network_uint16_t csum;  /**< checksum */
    network_uint32_t ptr;   /**< pointer */
} icmpv6_error_param_prob_t;

/**
 * @brief   Echo request and response message format.
 * @extends icmpv6_hdr_t
 *
 * @see <a href="https://tools.ietf.org/html/rfc4443#section-4.1">
 *          RFC 4443, section 4.1
 *      </a>
 * @see <a href="https://tools.ietf.org/html/rfc4443#section-4.2">
 *          RFC 4443, section 4.2
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;           /**< message type */
    uint8_t code;           /**< message code */
    network_uint16_t csum;  /**< checksum */
    network_uint16_t id;    /**< identifier */
    network_uint16_t seq;   /**< Sequence number */
} icmpv6_echo_t;

#ifdef __cplusplus
}
#endif

#endif /* ICMPV6_H_ */
/** @} */
