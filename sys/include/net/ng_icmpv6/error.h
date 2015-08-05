/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_ng_icmpv6_error ICMPv6 error messages
 * @ingroup     net_ng_icmpv6
 * @brief       ICMPv6 error message handling and creation
 * @{
 *
 * @file
 * @brief   ICMPv6 error message definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * @todo implement build and handle functions
 */
#ifndef NG_ICMPV6_ERROR_H_
#define NG_ICMPV6_ERROR_H_

#include <inttypes.h>

#include "byteorder.h"
#include "kernel_types.h"
#include "net/ng_icmpv6/types.h"
#include "net/ng_ipv6/hdr.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @{
 * @name Codes for destination unreachable messages
 *
 * @see <a href="https://tools.ietf.org/html/rfc4443#section-3.1">
 *          RFC 4443, section 3.1
 *      </a>
 */
#define NG_ICMPV6_ERROR_DEST_UNR_NO_ROUTE   (0) /**< no route to destination */
#define NG_ICMPV6_ERROR_DEST_UNR_PROHIB     (1) /**< communictation with
                                                 *   destination administratively
                                                 *   prohibited */
#define NG_ICMPV6_ERROR_DEST_UNR_SCOPE      (2) /**< beyond scope of source address */
#define NG_ICMPV6_ERROR_DEST_UNR_ADDR       (3) /**< address unreachable */
#define NG_ICMPV6_ERROR_DEST_UNR_PORT       (4) /**< port unreachable */
#define NG_ICMPV6_ERROR_DEST_UNR_POLICY     (5) /**< source address failed ingress/egress
                                                 *   policy */
#define NG_ICMPV6_ERROR_DEST_UNR_REJECT     (6) /**< reject route to destination */
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
#define NG_ICMPV6_ERROR_TIME_EXC_HL     (0) /**< hop limit exceeded in transit */
#define NG_ICMPV6_ERROR_TIME_EXC_FRAG   (1) /**< fragment reassembly time exceeded */
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
#define NG_ICMPV6_ERROR_PARAM_PROB_HDR_FIELD    (0) /**< errorneous header field
                                                     *   encountered */
#define NG_ICMPV6_ERROR_PARAM_PROB_NH           (1) /**< unrecognized next header
                                                     *   field encountered */
#define NG_ICMPV6_ERROR_PARAM_PROB_OPT          (2) /**< unrecognized IPv6 option
                                                     *   field encountered */
/**
 * @}
 */

/**
 * @brief   Destination unreachable message format.
 * @extends ng_icmpv6_hdr_t
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
} ng_icmpv6_error_dst_unr_t;

/**
 * @brief   Packet too big message format.
 * @extends ng_icmpv6_hdr_t
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
} ng_icmpv6_error_pkt_too_big_t;

/**
 * @brief   Time exceeded message format.
 * @extends ng_icmpv6_hdr_t
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
} ng_icmpv6_error_time_exc_t;

/**
 * @brief   Parameter problem message format.
 * @extends ng_icmpv6_hdr_t
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
} ng_icmpv6_error_param_prob_t;

#ifdef __cplusplus
}
#endif

#endif /* NG_ICMPV6_ERROR_H_ */
/** @} */
