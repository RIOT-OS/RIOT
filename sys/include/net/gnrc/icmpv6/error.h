/*
 * SPDX-FileCopyrightText: 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

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

#if defined(MODULE_GNRC_ICMPV6_ERROR) || defined(DOXYGEN)
/**
 * @brief   Sends an ICMPv6 destination unreachable message for sending.
 *
 * @pre     @p orig_pkt contains a packet snip of type @ref GNRC_NETTYPE_IPV6
 *
 * @note    Won't send if source address of @p orig_pkt is unspecified or
 *          multicast
 *
 * @param[in] code      The [code for the message](@ref net_icmpv6_error_dst_unr_codes).
 * @param[in] orig_pkt  The invoking packet.
 */
void gnrc_icmpv6_error_dst_unr_send(uint8_t code, const gnrc_pktsnip_t *orig_pkt);

/**
 * @brief   Sends an ICMPv6 packet too big message for sending.
 *
 * @pre     @p orig_pkt contains a packet snip of type @ref GNRC_NETTYPE_IPV6
 *
 * @note    Won't send if source address of @p orig_pkt is unspecified or
 *          multicast
 *
 * @param[in] mtu       The maximum transission unit of the next-hop link.
 * @param[in] orig_pkt  The invoking packet.
 */
void gnrc_icmpv6_error_pkt_too_big_send(uint32_t mtu,
                                        const gnrc_pktsnip_t *orig_pkt);

/**
 * @brief   Sends an ICMPv6 time exceeded message for sending.
 *
 * @pre     @p orig_pkt contains a packet snip of type @ref GNRC_NETTYPE_IPV6
 *
 * @note    Won't send if source address of @p orig_pkt is unspecified or
 *          multicast
 *
 * @param[in] code      The [code for the message](@ref net_icmpv6_error_time_exc_codes).
 * @param[in] orig_pkt  The invoking packet.
 */
void gnrc_icmpv6_error_time_exc_send(uint8_t code,
                                     const gnrc_pktsnip_t *orig_pkt);

/**
 * @brief   Sends an ICMPv6 parameter problem message for sending.
 *
 * @pre     @p orig_pkt is in receive order.
 * @pre     @p orig_pkt contains a packet snip of type @ref GNRC_NETTYPE_IPV6
 *
 * @note    Won't send if source address of @p orig_pkt is unspecified or
 *          multicast
 *
 * @param[in] code      The [code for the message](@ref net_icmpv6_error_param_prob_codes).
 * @param[in] ptr       Pointer to the erroneous octet in @p orig_pkt.
 * @param[in] orig_pkt  The invoking packet.
 */
void gnrc_icmpv6_error_param_prob_send(uint8_t code, void *ptr,
                                       const gnrc_pktsnip_t *orig_pkt);
#else   /* defined(MODULE_GNRC_ICMPV6_ERROR) || defined(DOXYGEN) */
/* NOPs to make the usage code more readable */
#define gnrc_icmpv6_error_dst_unr_send(code, orig_pkt) \
        (void)code; (void)orig_pkt
#define gnrc_icmpv6_error_pkt_too_big_send(mtu, orig_pkt) \
        (void)mtu; (void)orig_pkt
#define gnrc_icmpv6_error_time_exc_send(code, orig_pkt) \
        (void)code; (void)orig_pkt
#define gnrc_icmpv6_error_param_prob_send(code, ptr, orig_pkt) \
        (void)code; (void)ptr, (void)orig_pkt
#endif  /* defined(MODULE_GNRC_ICMPV6_ERROR) || defined(DOXYGEN) */

#ifdef __cplusplus
}
#endif

/** @} */
