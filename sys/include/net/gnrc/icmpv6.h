/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    net_gnrc_icmpv6  ICMPv6
 * @ingroup     net_gnrc_ipv6
 * @brief       GNRC's implementation of ICMPv6
 *
 * @see <a href="https://tools.ietf.org/html/rfc4443">
 *          RFC 4443
 *      </a>
 * @{
 *
 * @file
 * @brief       Definitions for GNRC's ICMPv6 implementation
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * @todo build error messages
 */

#ifndef GNRC_ICMPV6_H_
#define GNRC_ICMPV6_H_

#include "kernel_types.h"
#include "net/icmpv6.h"
#include "net/gnrc/pkt.h"

#include "net/gnrc/icmpv6/echo.h"
#include "net/gnrc/icmpv6/error.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Demultiplexes a received ICMPv6 packet according to its type field.
 *
 * @param[in] iface     The receiving interface
 * @param[in] pkt       The packet to demultiplex.
 */
void gnrc_icmpv6_demux(kernel_pid_t iface, gnrc_pktsnip_t *pkt);

/**
 * @brief   Builds an ICMPv6 message for sending.
 *
 * @param[in] next  Next packet snip in the new packet.
 * @param[in] type  Type for the ICMPv6 message.
 * @param[in] code  Code for the ICMPv6 message.
 * @param[in] size  Size of the ICMPv6 message (needs do be >
 *                  `sizeof(icmpv6_hdr_t)`).
 *
 * @return  The ICMPv6 message on success
 * @return  NULL, on failure
 */
gnrc_pktsnip_t *gnrc_icmpv6_build(gnrc_pktsnip_t *next, uint8_t type, uint8_t code, size_t size);

/**
 * @brief   Calculates the checksum for an ICMPv6 packet.
 *
 * @param[in] hdr           The header the checksum should be calculated
 *                          for.
 * @param[in] pseudo_hdr    The header the pseudo header shall be generated
 *                          from. NULL if none is needed.
 *
 * @return  0, on success.
 * @return  -EINVAL, if gnrc_pktsnip_t::type of @p pkt was not GNRC_NETTYPE_ICMPV6
 * @return  -ENOENT, if gnrc_pktsnip_t::type of @p pseudo_hdr was not
 *          GNRC_NETTYPE_IPV6
 */
int gnrc_icmpv6_calc_csum(gnrc_pktsnip_t *hdr, gnrc_pktsnip_t *pseudo_hdr);

#ifdef __cplusplus
}
#endif

#endif /* GNRC_ICMPV6_H_ */
/**
 * @}
 */
