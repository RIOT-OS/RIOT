/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    net_ng_icmpv6  Internet Control Message Protocol for IPv6
 * @ingroup     net_ng_ipv6
 * @brief       Basic implementation of ICMPv6
 *
 * @see <a href="https://tools.ietf.org/html/rfc4443">
 *          RFC 4443
 *      </a>
 * @{
 *
 * @file
 * @brief       Definitions for ICMPv6
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#ifndef NG_ICMPV6_H_
#define NG_ICMPV6_H_

#include <inttypes.h>

#include "byteorder.h"
#include "kernel_types.h"
#include "net/ng_ipv6/hdr.h"
#include "net/ng_nettype.h"
#include "net/ng_nettype.h"
#include "net/ng_pkt.h"

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
#define NG_ICMPV6_DEST_UNR      (1)     /**< Destination unreachable message */
#define NG_ICMPV6_PKT_TOO_BIG   (2)     /**< Packet Too Big message */
#define NG_ICMPV6_TIME_EXC      (3)     /**< Time Exceeded message */
#define NG_ICMPV6_PARAM_PROB    (4)     /**< Parameter Problem message */
#define NG_ICMPV6_ERR_EXP1      (100)   /**< message type for private experimentation */
#define NG_ICMPV6_ERR_EXP2      (101)   /**< message type for private experimentation */
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
#define NG_ICMPV6_ECHO_REQ      (128)   /**< Echo request message (ping) */
#define NG_ICMPV6_ECHO_REP      (129)   /**< Echo reply message (pong) */
#define NG_ICMPV6_RTR_SOL       (133)   /**< NDP router solicitation message */
#define NG_ICMPV6_RTR_ADV       (134)   /**< NDP router advertisement message */
#define NG_ICMPV6_NBR_SOL       (135)   /**< NDP neighbor solicitation message */
#define NG_ICMPV6_NBR_ADV       (136)   /**< NDP neighbor advertisement message */
#define NG_ICMPV6_REDIRECT      (137)   /**< NDP redirect message */
#define NG_ICMPV6_RPL_CTRL      (155)   /**< RPL control message */
#define NG_ICMPV6_INF_EXP1      (200)   /**< message type for private experimentation */
#define NG_ICMPV6_INF_EXP2      (201)   /**< message type for private experimentation */
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
} ng_icmpv6_hdr_t;

/**
 * @brief   Demultiplexes a received ICMPv6 packet according to its type field.
 *
 * @param[in] iface     The receiving interface
 * @param[in] pkt       The packet to demultiplex.
 */
void ng_icmpv6_demux(kernel_pid_t iface, ng_pktsnip_t *pkt);

/* TODO: build error messages */

/**
 * @brief   Calculates the checksum for an ICMPv6 packet.
 *
 * @param[in] hdr           The header the checksum should be calculated
 *                          for.
 * @param[in] pseudo_hdr    The header the pseudo header shall be generated
 *                          from. NULL if none is needed.
 *
 * @return  0, on success.
 * @return  -EINVAL, if ng_pktsnip_t::type of @p pkt was not NG_NETTYPE_ICMPV6
 * @return  -ENOENT, if ng_pktsnip_t::type of @p pseudo_hdr was not
 *          NG_NETTYPE_IPV6
 */
int ng_icmpv6_calc_csum(ng_pktsnip_t *hdr, ng_pktsnip_t *pseudo_hdr);

#ifdef __cplusplus
}
#endif

#endif /* NG_ICMPV6_H_ */
/**
 * @}
 */
