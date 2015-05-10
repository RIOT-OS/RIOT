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

#include "net/ng_icmpv6/echo.h"
#include "net/ng_icmpv6/error.h"
#include "net/ng_icmpv6/types.h"

#ifdef __cplusplus
extern "C" {
#endif

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

/**
 * @brief   Builds an ICMPv6 message for sending.
 *
 * @param[in] next  Next packet snip in the new packet.
 * @param[in] type  Type for the ICMPv6 message.
 * @param[in] code  Code for the ICMPv6 message.
 * @param[in] size  Size of the ICMPv6 message (needs do be >
 *                  `sizeof(ng_icmpv6_hdr_t)`).
 *
 * @return  The ICMPv6 message on success
 * @return  NULL, on failure
 */
ng_pktsnip_t *ng_icmpv6_build(ng_pktsnip_t *next, uint8_t type, uint8_t code, size_t size);

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
