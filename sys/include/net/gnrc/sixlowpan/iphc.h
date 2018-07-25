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
#ifndef NET_GNRC_SIXLOWPAN_IPHC_H
#define NET_GNRC_SIXLOWPAN_IPHC_H

#include <stdbool.h>

#include "net/gnrc/pkt.h"
#include "net/sixlowpan.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Decompresses a received 6LoWPAN IPHC frame.
 *
 * @pre (dec_hdr != NULL) && (*dec_hdr != NULL) && ((*dec_hdr)->size >= sizeof(gnrc_ipv6_hdr_t))
 *
 * @param[out] dec_hdr      A pre-allocated IPv6 header. Will not be inserted into
 *                          @p pkt. May change due to next headers being added in NHC.
 * @param[in] pkt           A received 6LoWPAN IPHC frame. IPHC dispatch will not
 *                          be marked.
 * @param[in] datagram_size Size of the full uncompressed IPv6 datagram. May be 0, if @p pkt
 *                          contains the full (unfragmented) IPv6 datagram.
 * @param[in] offset        Offset of the IPHC dispatch in 6LoWPaN frame.
 * @param[in, out] nh_len   Pointer to next header length
 *
 * @return  length of the HC dispatches + inline values on success.
 * @return  0 on error.
 */
size_t gnrc_sixlowpan_iphc_decode(gnrc_pktsnip_t **dec_hdr, gnrc_pktsnip_t *pkt,
                                  size_t datagram_size, size_t offset,
                                  size_t *nh_len);

/**
 * @brief   Compresses a 6LoWPAN for IPHC.
 *
 * @pre (pkt != NULL)
 *
 * @param[in] pkt   A 6LoWPAN frame with an uncompressed IPv6 header to send.
 *                  Will be translated to an 6LoWPAN IPHC frame.
 * @param[in] ctx   Context for the packet. May be NULL.
 * @param[in] page  Current 6Lo dispatch parsing page.
 *
 */
void gnrc_sixlowpan_iphc_send(gnrc_pktsnip_t *pkt, void *ctx, unsigned page);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_SIXLOWPAN_IPHC_H */
/** @} */
