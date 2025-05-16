/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

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

#include <stdbool.h>

#include "net/gnrc/pkt.h"
#include "net/sixlowpan.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Decompresses a received 6LoWPAN IPHC frame.
 *
 * @pre (pkt != NULL)
 *
 * @param[in] pkt           A received 6LoWPAN IPHC frame. The first snip is to
 *                          be expected to start with the IPHC dispatch.
 * @param[in,out] ctx       Context for the packet. May be NULL. If not NULL it
 *                          is expected to be of type
 *                          @ref gnrc_sixlowpan_frag_rb_t. This function might
 *                          change the content of that.
 * @param[in] page          Current 6Lo dispatch parsing page.
 */
void gnrc_sixlowpan_iphc_recv(gnrc_pktsnip_t *pkt, void *ctx, unsigned page);

/**
 * @brief   Compresses a 6LoWPAN for IPHC.
 *
 * @pre (pkt != NULL)
 *
 * @param[in] pkt   A 6LoWPAN frame with an uncompressed IPv6 header to send.
 *                  Will be translated to an 6LoWPAN IPHC frame.
 * @param[in] ctx   Context for the packet. May be NULL.
 *                  If not NULL it is expected to be of type @ref
 *                  gnrc_sixlowpan_frag_fb_t to provide initial information for
 *                  possible fragmentation after compression (see
 *                  net_gnrc_sixlowpan_frag_hint). This function might change
 *                  the content of that. Depending on the compile configuration
 *                  it might be ignored completely.
 * @param[in] page  Current 6Lo dispatch parsing page.
 *
 */
void gnrc_sixlowpan_iphc_send(gnrc_pktsnip_t *pkt, void *ctx, unsigned page);

#ifdef __cplusplus
}
#endif

/** @} */
