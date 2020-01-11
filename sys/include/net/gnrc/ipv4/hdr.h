/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_ipv4_hdr IPv4 header defintions
 * @ingroup     net_gnrc_ipv4
 * @{
 *
 * @file
 * @brief   IPv4 header
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NET_GNRC_IPV4_HDR_H
#define NET_GNRC_IPV4_HDR_H

#include <stdint.h>

#include "net/ipv4/hdr.h"
#include "net/gnrc/pkt.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Builds an IPv4 header for sending and adds it to the packet buffer.
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
 * @return  The an IPv4 header in packet buffer on success.
 * @return  NULL on error.
 */
gnrc_pktsnip_t *gnrc_ipv4_hdr_build(gnrc_pktsnip_t *payload, const ipv4_addr_t *src,
                                    const ipv4_addr_t *dst);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_IPV4_HDR_H */
/** @} */
