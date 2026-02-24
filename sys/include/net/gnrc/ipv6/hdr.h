/*
 * SPDX-FileCopyrightText: 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    net_gnrc_ipv6_hdr IPv6 header definitions
 * @ingroup     net_gnrc_ipv6
 * @{
 *
 * @file
 * @brief   IPv6 header
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <stdint.h>

#include "net/ipv6/hdr.h"
#include "net/gnrc/pkt.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Builds an IPv6 header for sending and adds it to the packet buffer.
 *
 * @details Initializes version field with 6, traffic class, flow label, and
 *          hop limit with 0, and next header with @ref PROTNUM_RESERVED.
 *
 * @param[in] payload   Payload for the packet.
 * @param[in] src       Source address for the header. Can be NULL if not
 *                      known or required.
 * @param[in] dst       Destination address for the header. Can be NULL if not
 *                      known or required.
 *
 * @return  The an IPv6 header in packet buffer on success.
 * @return  NULL on error.
 */
gnrc_pktsnip_t *gnrc_ipv6_hdr_build(gnrc_pktsnip_t *payload, const ipv6_addr_t *src,
                                    const ipv6_addr_t *dst);

#ifdef __cplusplus
}
#endif

/** @} */
