/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_sixlowpan_iphc   IPv6 header compression (IPHC)
 * @ingroup     net_gnrc_sixlowpan
 * @brief       IPv6 header compression for 6LoWPAN.
 * @{
 *
 * @file
 * @brief   6LoWPAN IPHC definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef GNRC_SIXLOWPAN_IPHC_H_
#define GNRC_SIXLOWPAN_IPHC_H_

#include <stdbool.h>

#include "net/gnrc/pkt.h"
#include "net/sixlowpan.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Decompresses a received 6LoWPAN IPHC frame.
 *
 * @pre (ipv6 != NULL) && (ipv6->size >= sizeof(gnrc_ipv6_hdr_t))
 *
 * @param[out] ipv6     A pre-allocated IPv6 header. Will not be inserted into
 *                      @p pkt
 * @param[in,out] pkt   A received 6LoWPAN IPHC frame. IPHC dispatch will not
 *                      be marked.
 * @param[in] offset    Offset of the IPHC dispatch in 6LoWPaN frame.
 *
 * @return  length of the HC dispatches + inline values on success.
 * @return  0 on error.
 */
size_t gnrc_sixlowpan_iphc_decode(gnrc_pktsnip_t *ipv6, gnrc_pktsnip_t *pkt, size_t offset);

/**
 * @brief   Compresses a 6LoWPAN for IPHC.
 *
 * @param[in,out] pkt   A 6LoWPAN frame with an uncompressed IPv6 header to
 *                      send. Will be translated to an 6LoWPAN IPHC frame.
 *
 * @return  true, on success
 * @return  false, on error.
 */
bool gnrc_sixlowpan_iphc_encode(gnrc_pktsnip_t *pkt);

#ifdef __cplusplus
}
#endif

#endif /* GNRC_SIXLOWPAN_IPHC_H_ */
/** @} */
