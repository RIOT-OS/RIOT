/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_icmpv4_error ICMPv4 error messages
 * @ingroup     net_gnrc_icmpv4
 * @brief       ICMPv4 error message handling and creation
 * @{
 *
 * @file
 * @brief   ICMPv4 error message definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NET_GNRC_ICMPV4_ERROR_H
#define NET_GNRC_ICMPV4_ERROR_H

#include <errno.h>
#include <stdint.h>

#include "net/icmp.h"
#include "net/ipv4/hdr.h"
#include "net/gnrc/ipv4.h"
#include "net/gnrc/netapi.h"
#include "net/gnrc/pkt.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(MODULE_GNRC_ICMPV4_ERROR) || defined(DOXYGEN)
/**
 * @brief   Sends an ICMPv4 destination unreachable message for sending.
 *
 * @pre     @p orig_pkt contains a packet snip of type @ref GNRC_NETTYPE_IPV4
 *
 * @note    Won't send if source address of @p orig_pkt is unspecified or
 *          multicast
 *
 * @param[in] code      The [code for the message](@ref net_icmpv4_error_dst_unr_codes).
 * @param[in] orig_pkt  The invoking packet.
 */
void gnrc_icmpv4_error_dst_unr_send(uint8_t code, const gnrc_pktsnip_t *orig_pkt);

/**
 * @brief   Sends an ICMPv4 packet too big message for sending.
 *
 * @pre     @p orig_pkt contains a packet snip of type @ref GNRC_NETTYPE_IPV4
 *
 * @note    Won't send if source address of @p orig_pkt is unspecified or
 *          multicast
 *
 * @param[in] mtu       The maximum transission unit of the next-hop link.
 * @param[in] orig_pkt  The invoking packet.
 */
void gnrc_icmpv4_error_pkt_too_big_send(uint32_t mtu,
                                        const gnrc_pktsnip_t *orig_pkt);

/**
 * @brief   Sends an ICMPv4 time exceeded message for sending.
 *
 * @pre     @p orig_pkt contains a packet snip of type @ref GNRC_NETTYPE_IPV4
 *
 * @note    Won't send if source address of @p orig_pkt is unspecified or
 *          multicast
 *
 * @param[in] code      The [code for the message](@ref net_icmpv4_error_time_exc_codes).
 * @param[in] orig_pkt  The invoking packet.
 */
void gnrc_icmpv4_error_time_exc_send(uint8_t code,
                                     const gnrc_pktsnip_t *orig_pkt);

/**
 * @brief   Sends an ICMPv4 parameter problem message for sending.
 *
 * @pre     @p orig_pkt is in receive order.
 * @pre     @p orig_pkt contains a packet snip of type @ref GNRC_NETTYPE_IPV4
 *
 * @note    Won't send if source address of @p orig_pkt is unspecified or
 *          multicast
 *
 * @param[in] code      The [code for the message](@ref net_icmpv4_error_param_prob_codes).
 * @param[in] ptr       Pointer to the errorneous octet in @p orig_pkt.
 * @param[in] orig_pkt  The invoking packet.
 */
void gnrc_icmpv4_error_param_prob_send(uint8_t code, void *ptr,
                                       const gnrc_pktsnip_t *orig_pkt);
#else   /* defined(MODULE_GNRC_ICMPV4_ERROR) || defined(DOXYGEN) */
/* NOPs to make the usage code more readable */
#define gnrc_icmpv4_error_dst_unr_send(code, orig_pkt) \
        (void)code; (void)orig_pkt
#define gnrc_icmpv4_error_pkt_too_big_send(mtu, orig_pkt) \
        (void)mtu; (void)orig_pkt
#define gnrc_icmpv4_error_time_exc_send(code, orig_pkt) \
        (void)code; (void)orig_pkt
#define gnrc_icmpv4_error_param_prob_send(code, ptr, orig_pkt) \
        (void)code; (void)ptr, (void)orig_pkt
#endif  /* defined(MODULE_GNRC_ICMPV4_ERROR) || defined(DOXYGEN) */

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_ICMPV4_ERROR_H */
/** @} */
