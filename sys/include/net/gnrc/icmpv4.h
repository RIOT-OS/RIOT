/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    net_gnrc_icmpv4  ICMPv4
 * @ingroup     net_gnrc_ipv4
 * @brief       GNRC's implementation of ICMPv4
 *
 * @see <a href="https://tools.ietf.org/html/rfc4443">
 *          RFC 4443
 *      </a>
 * @{
 *
 * @file
 * @brief       Definitions for GNRC's ICMPv4 implementation
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * @todo build error messages
 */

#ifndef NET_GNRC_ICMPV4_H
#define NET_GNRC_ICMPV4_H

#include "net/icmp.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/pkt.h"

#include "net/gnrc/icmpv4/echo.h"
#include "net/gnrc/icmpv4/error.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Demultiplexes a received ICMPv4 packet according to its type field.
 *
 * @param[in] netif     The receiving interface
 * @param[in] pkt       The packet to demultiplex.
 */
void gnrc_icmpv4_demux(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt);

/**
 * @brief   Builds an ICMPv4 message for sending.
 *
 * @param[in] next  Next packet snip in the new packet.
 * @param[in] type  Type for the ICMPv4 message.
 * @param[in] code  Code for the ICMPv4 message.
 * @param[in] size  Size of the ICMPv4 message (needs do be >
 *                  `sizeof(icmpv4_hdr_t)`).
 *
 * @return  The ICMPv4 message on success
 * @return  NULL, on failure
 */
gnrc_pktsnip_t *gnrc_icmpv4_build(gnrc_pktsnip_t *next, uint8_t type, uint8_t code, size_t size);

/**
 * @brief   Calculates the checksum for an ICMPv4 packet.
 *
 * @param[in] hdr           The header the checksum should be calculated
 *                          for.
 * @param[in] pseudo_hdr    The header the pseudo header shall be generated
 *                          from. NULL if none is needed.
 *
 * @return  0, on success.
 * @return  -EINVAL, if gnrc_pktsnip_t::type of @p pkt was not GNRC_NETTYPE_ICMPv4
 * @return  -ENOENT, if gnrc_pktsnip_t::type of @p pseudo_hdr was not
 *          GNRC_NETTYPE_IPv4
 */
int gnrc_icmpv4_calc_csum(gnrc_pktsnip_t *hdr, gnrc_pktsnip_t *pseudo_hdr);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_ICMPv4_H */
/**
 * @}
 */
