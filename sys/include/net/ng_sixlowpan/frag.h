/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_ng_sixlowpan_frag   6LoWPAN Fragmentation
 * @ingroup     net_ng_sixlowpan
 * @brief       6LoWPAN Fragmentation headers and functionality
 * @see <a href="https://tools.ietf.org/html/rfc4944#section-5.3">
 *          RFC 4944, section 5.3
 *      </a>
 * @{
 *
 * @file
 * @brief   6LoWPAN Fragmentation definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NG_SIXLOWPAN_FRAG_H_
#define NG_SIXLOWPAN_FRAG_H_

#include <inttypes.h>
#include <stdbool.h>

#include "byteorder.h"
#include "kernel_types.h"
#include "net/ng_pkt.h"
#include "net/sixlowpan.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Sends a packet fragmented.
 *
 * @param[in] pid           The interface to send the packet over.
 * @param[in] pkt           The packet to send.
 * @param[in] datagram_size The length of just the IPv6 packet. It is the value
 *                          that the ng_sixlowpan_frag_t::disp_size field will be
 *                          set to.
 */
void ng_sixlowpan_frag_send(kernel_pid_t pid, ng_pktsnip_t *pkt,
                            size_t datagram_size);

/**
 * @brief   Handles a packet containing a fragment header.
 *
 * @param[in] pkt   The packet to handle.
 */
void ng_sixlowpan_frag_handle_pkt(ng_pktsnip_t *pkt);

#ifdef __cplusplus
}
#endif

#endif /* NG_SIXLOWPAN_FRAG_H_ */
/** @} */
