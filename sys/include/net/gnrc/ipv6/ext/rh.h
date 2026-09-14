/*
 * SPDX-FileCopyrightText: 2018 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    net_gnrc_ipv6_ext_rh Support for IPv6 routing header extension
 * @ingroup     net_gnrc_ipv6_ext
 * @brief       GNRC implementation of IPv6 routing header extension.
 * @{
 *
 * @file
 * @brief   GNRC routing extension header definitions.
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include "net/gnrc/pkt.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
    /**
     * @brief   An error occurred during routing header processing
     */
    GNRC_IPV6_EXT_RH_ERROR = 0,
    /**
     * @brief   The routing header was successfully processed and this node
     *          is the destination (i.e. ipv6_ext_rh_t::seg_left == 0)
     */
    GNRC_IPV6_EXT_RH_AT_DST,
    /**
     * @brief   The routing header was successfully processed and the packet
     *          was forwarded to another node or should be forwarded to another
     *          node.
     *
     * When @ref gnrc_ipv6_ext_rh_process() returns this value, the packet was
     * already forwarded to another node. Implementations for specific routing
     * header types should leave the forwarding to the calling @ref
     * gnrc_ipv6_ext_rh_process() and should return @ref
     * GNRC_IPV6_EXT_RH_FORWARDED if they want the packet to be forwarded. They
     * should however set ipv6_hdr_t::dst accordingly.
     */
    GNRC_IPV6_EXT_RH_FORWARDED,
};

/**
 * @brief   Process the routing header of an IPv6 packet.
 *
 * @param[in] pkt   An IPv6 packet containing the routing header in the first
 *                  snip
 *
 * @return  @ref GNRC_IPV6_EXT_RH_AT_DST, on success
 * @return  @ref GNRC_IPV6_EXT_RH_FORWARDED, when @p pkt was forwarded
 * @return  @ref GNRC_IPV6_EXT_RH_ERROR, on error
 */
int gnrc_ipv6_ext_rh_process(gnrc_pktsnip_t *pkt);

#ifdef __cplusplus
}
#endif

/** @} */
