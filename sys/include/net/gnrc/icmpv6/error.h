/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_icmpv6_error ICMPv6 error messages
 * @ingroup     net_gnrc_icmpv6
 * @brief       ICMPv6 error message handling and creation
 * @{
 *
 * @file
 * @brief   ICMPv6 error message definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NET_GNRC_ICMPV6_ERROR_H
#define NET_GNRC_ICMPV6_ERROR_H

#include <errno.h>
#include <stdint.h>

#include "net/icmpv6.h"
#include "net/ipv6/hdr.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/netapi.h"
#include "net/gnrc/pkt.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Sends an ICMPv6 destination unreachable message for sending.
 *
 * @pre @p orig_pkt contains a packet snip of type @ref GNRC_NETTYPE_IPV6
 *
 * @param[in] code      The code for the message @see net/icmpv6.h.
 * @param[in] orig_pkt  The invoking packet.
 */
void gnrc_icmpv6_error_dst_unr_send(uint8_t code, const gnrc_pktsnip_t *orig_pkt);

/**
 * @brief   Sends an ICMPv6 packet too big message for sending.
 *
 * @pre @p orig_pkt contains a packet snip of type @ref GNRC_NETTYPE_IPV6
 *
 * @param[in] mtu       The maximum transission unit of the next-hop link.
 * @param[in] orig_pkt  The invoking packet.
 */
void gnrc_icmpv6_error_pkt_too_big_send(uint32_t mtu,
                                        const gnrc_pktsnip_t *orig_pkt);

/**
 * @brief   Sends an ICMPv6 time exceeded message for sending.
 *
 * @pre @p orig_pkt contains a packet snip of type @ref GNRC_NETTYPE_IPV6
 *
 * @param[in] code      The code for the message @see net/icmpv6.h.
 * @param[in] orig_pkt  The invoking packet.
 */
void gnrc_icmpv6_error_time_exc_send(uint8_t code,
                                     const gnrc_pktsnip_t *orig_pkt);

/**
 * @brief   Sends an ICMPv6 parameter problem message for sending.
 *
 * @pre @p orig_pkt is in receive order.
 * @pre @p orig_pkt contains a packet snip of type @ref GNRC_NETTYPE_IPV6
 *
 * @param[in] code      The code for the message @see net/icmpv6.h.
 * @param[in] ptr       Pointer to the errorneous octet in @p orig_pkt.
 * @param[in] orig_pkt  The invoking packet.
 */
void gnrc_icmpv6_error_param_prob_send(uint8_t code, void *ptr,
                                       const gnrc_pktsnip_t *orig_pkt);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_ICMPV6_ERROR_H */
/** @} */
