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
#ifndef GNRC_ICMPV6_ERROR_H_
#define GNRC_ICMPV6_ERROR_H_

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
 * @brief   Builds an ICMPv6 destination unreachable message for sending.
 *
 * @param[in] code      The code for the message @see net/icmpv6.h.
 * @param[in] orig_pkt  The invoking packet.
 *
 * @return  The destination unreachable message on success.
 * @return  NULL, on failure.
 */
gnrc_pktsnip_t *gnrc_icmpv6_error_dst_unr_build(uint8_t code, gnrc_pktsnip_t *orig_pkt);

/**
 * @brief   Builds an ICMPv6 packet too big message for sending.
 *
 * @param[in] mtu       The maximum transission unit of the next-hop link.
 * @param[in] orig_pkt  The invoking packet.
 *
 * @return  The packet too big message on success.
 * @return  NULL, on failure.
 */
gnrc_pktsnip_t *gnrc_icmpv6_error_pkt_too_big_build(uint32_t mtu, gnrc_pktsnip_t *orig_pkt);

/**
 * @brief   Builds an ICMPv6 time exceeded message for sending.
 *
 * @param[in] code      The code for the message @see net/icmpv6.h.
 * @param[in] orig_pkt  The invoking packet.
 *
 * @return  The time exceeded message on success.
 * @return  NULL, on failure.
 */
gnrc_pktsnip_t *gnrc_icmpv6_error_time_exc_build(uint8_t code, gnrc_pktsnip_t *orig_pkt);

/**
 * @brief   Builds an ICMPv6 parameter problem message for sending.
 *
 * @param[in] code      The code for the message @see net/icmpv6.h.
 * @param[in] ptr       Pointer to the errorneous octet in @p orig_pkt.
 * @param[in] orig_pkt  The invoking packet.
 *
 * @return  The parameter problem message on success.
 * @return  NULL, on failure.
 */
gnrc_pktsnip_t *gnrc_icmpv6_error_param_prob_build(uint8_t code, void *ptr,
                                                   gnrc_pktsnip_t *orig_pkt);

/**
 * @brief   Sends an ICMPv6 destination unreachable message for sending.
 *
 * @param[in] code      The code for the message @see net/icmpv6.h.
 * @param[in] orig_pkt  The invoking packet.
 */
static inline void gnrc_icmpv6_error_dst_unr_send(uint8_t code, gnrc_pktsnip_t *orig_pkt)
{
    gnrc_pktsnip_t *pkt = gnrc_icmpv6_error_dst_unr_build(code, orig_pkt);

    if (pkt != NULL) {
        gnrc_netapi_send(gnrc_ipv6_pid, pkt);
    }
#ifdef MODULE_GNRC_PKTBUF
    gnrc_pktbuf_release_error(orig_pkt, EHOSTUNREACH);
#else
    (void)orig_pkt;
#endif
}

/**
 * @brief   Sends an ICMPv6 packet too big message for sending.
 *
 * @param[in] mtu       The maximum transission unit of the next-hop link.
 * @param[in] orig_pkt  The invoking packet.
 */
static inline void gnrc_icmpv6_error_pkt_too_big_send(uint32_t mtu, gnrc_pktsnip_t *orig_pkt)
{
    gnrc_pktsnip_t *pkt = gnrc_icmpv6_error_pkt_too_big_build(mtu, orig_pkt);

    if (pkt != NULL) {
        gnrc_netapi_send(gnrc_ipv6_pid, pkt);
    }
#ifdef MODULE_GNRC_PKTBUF
    gnrc_pktbuf_release_error(orig_pkt, EMSGSIZE);
#else
    (void)orig_pkt;
#endif
}

/**
 * @brief   Sends an ICMPv6 time exceeded message for sending.
 *
 * @param[in] code      The code for the message @see net/icmpv6.h.
 * @param[in] orig_pkt  The invoking packet.
 */
static inline void gnrc_icmpv6_error_time_exc_send(uint8_t code, gnrc_pktsnip_t *orig_pkt)
{
    gnrc_pktsnip_t *pkt = gnrc_icmpv6_error_time_exc_build(code, orig_pkt);

    if (pkt != NULL) {
        gnrc_netapi_send(gnrc_ipv6_pid, pkt);
    }
#ifdef MODULE_GNRC_PKTBUF
    gnrc_pktbuf_release_error(orig_pkt, ETIMEDOUT);
#else
    (void)orig_pkt;
#endif
}

/**
 * @brief   Sends an ICMPv6 parameter problem message for sending.
 *
 * @param[in] code      The code for the message @see net/icmpv6.h.
 * @param[in] ptr       Pointer to the errorneous octet in @p orig_pkt.
 * @param[in] orig_pkt  The invoking packet.
 */
static inline void gnrc_icmpv6_error_param_prob_send(uint8_t code, void *ptr,
                                                     gnrc_pktsnip_t *orig_pkt)
{
    gnrc_pktsnip_t *pkt = gnrc_icmpv6_error_param_prob_build(code, ptr, orig_pkt);

    if (pkt != NULL) {
        gnrc_netapi_send(gnrc_ipv6_pid, pkt);
    }
#ifdef MODULE_GNRC_PKTBUF
    gnrc_pktbuf_release_error(orig_pkt, EINVAL);
#else
    (void)orig_pkt;
#endif
}

#ifdef __cplusplus
}
#endif

#endif /* GNRC_ICMPV6_ERROR_H_ */
/** @} */
