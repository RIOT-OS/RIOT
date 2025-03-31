/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef NET_GNRC_SIXLOWPAN_ND_H
#define NET_GNRC_SIXLOWPAN_ND_H
/**
 * @defgroup    net_gnrc_sixlowpan_nd 6LoWPAN neighbor discovery
 * @ingroup     net_gnrc_sixlowpan
 * @brief       Neighbor Discovery Optimization for 6LoWPAN
 * @see         <a href="https://tools.ietf.org/html/rfc6775">
 *                  RFC 6775
 *              </a>
 * @{
 *
 * @file
 * @brief   General 6LoWPAN ND definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <stdint.h>

#include "net/gnrc/pkt.h"
#include "net/gnrc/sixlowpan/config.h"
#include "net/ipv6/addr.h"
#include "net/ndp.h"
#include "net/sixlowpan/nd.h"
#include "timex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Builds the address registration option.
 *
 * @param[in] status    Status for the ARO.
 * @param[in] ltime     Registration lifetime for the ARO.
 * @param[in] eui64     The EUI-64 for the ARO
 * @param[in] next          More options in the packet. NULL, if there are none.
 *
 * @return  The pkt snip list of options, on success
 * @return  NULL, if packet buffer is full
 */
gnrc_pktsnip_t *gnrc_sixlowpan_nd_opt_ar_build(uint8_t status, uint16_t ltime, eui64_t *eui64,
                                               gnrc_pktsnip_t *next);

/**
 * @brief   Builds the 6LoWPAN context option.
 *
 * @param[in] prefix_len    The length of the context's prefix.
 * @param[in] flags         Flags + CID for the context.
 * @param[in] ltime         Lifetime of the context.
 * @param[in] prefix        The context's prefix
 * @param[in] next      More options in the packet. NULL, if there are none.
 *
 * @return  The pkt snip list of options, on success
 * @return  NULL, if packet buffer is full or on error
 */
gnrc_pktsnip_t *gnrc_sixlowpan_nd_opt_6ctx_build(uint8_t prefix_len, uint8_t flags, uint16_t ltime,
                                                 ipv6_addr_t *prefix, gnrc_pktsnip_t *next);

/**
 * @brief   Builds the authoritative border router option.
 *
 * @param[in] version   Version of the border router information.
 * @param[in] ltime     Registration lifetime for the border router.
 * @param[in] braddr    The IPv6 address of the border router.
 * @param[in] next      More options in the packet. NULL, if there are none.
 *
 * @return  The pkt snip list of options, on success
 * @return  NULL, if packet buffer is full or on error
 */
gnrc_pktsnip_t *gnrc_sixlowpan_nd_opt_abr_build(uint32_t version, uint16_t ltime,
                                                ipv6_addr_t *braddr, gnrc_pktsnip_t *next);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NET_GNRC_SIXLOWPAN_ND_H */
