/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_ng_ipv6_hdr IPv6 header defintions
 * @ingroup     net_ng_ipv6
 * @{
 *
 * @file
 * @brief   IPv6 header
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NG_IPV6_HDR_H_
#define NG_IPV6_HDR_H_

#include <stdint.h>

#include "net/ipv6/hdr.h"
#include "net/ng_pkt.h"

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
 * @param[in] src_len   Length of @p src. Can be 0 if not known or required or
 *                      must be `sizeof(ipv6_addr_t)`.
 * @param[in] dst       Destination address for the header. Can be NULL if not
 *                      known or required.
 * @param[in] dst_len   Length of @p dst. Can be 0 if not known or required or
 *                      must be `sizeof(ipv6_addr_t)`.
 *
 * @return  The an IPv6 header in packet buffer on success.
 * @return  NULL on error.
 */
ng_pktsnip_t *ng_ipv6_hdr_build(ng_pktsnip_t *payload,
                                uint8_t *src, uint8_t src_len,
                                uint8_t *dst, uint8_t dst_len);

#ifdef __cplusplus
}
#endif

#endif /* NG_IPV6_HDR_H_ */
/** @} */
