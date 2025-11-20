/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    net_gnrc_sixlowpan_frag_minfwd  Minimal fragment forwarding
 * @ingroup     net_gnrc_sixlowpan_frag
 * @brief       Provides minimal fragment forwarding using the VRB
 * @see         [RFC 8930](https://tools.ietf.org/html/rfc8930)
 * @see         @ref net_gnrc_sixlowpan_frag_vrb
 * @experimental
 * @{
 *
 * @file
 * @brief   Minimal fragment forwarding definitions
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <stddef.h>

#include "net/gnrc/pkt.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/sixlowpan/frag.h"
#include "net/gnrc/sixlowpan/frag/fb.h"
#include "net/gnrc/sixlowpan/frag/vrb.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Forwards a fragment according to a VRB entry
 *
 * @param[in] pkt       The fragment to forward (without fragmentation header).
 *                      Is consumed by this function.
 * @param[in] frag      The originally received fragmentation header.
 * @param[in] vrbe      Virtual reassembly buffer containing the forwarding
 *                      information. Removed when datagram was completely
 *                      forwarded.
 * @param[in] page      Current 6Lo dispatch parsing page.
 *
 * @pre `vrbe != NULL`
 * @pre `pkt != NULL`
 * @pre `frag != NULL`
 *
 * @return  0 on success.
 * @return  -ENOMEM, when packet buffer is too full to prepare packet for
 *          forwarding.
 */
int gnrc_sixlowpan_frag_minfwd_forward(gnrc_pktsnip_t *pkt,
                                       const sixlowpan_frag_n_t *frag,
                                       gnrc_sixlowpan_frag_vrb_t *vrbe,
                                       unsigned page);

/**
 * @brief   Fragments a packet with just the IPHC (and padding payload to get
 *          to 8 byte) as the first fragment
 *
 * @pre `(frag_msg != NULL)`
 * @pre `(pkt != NULL) && (pkt->type == GNRC_NETTYPE_NETIF)`
 * @pre `(pkt->next != NULL) && (pkt->next->type == GNRC_NETTYPE_SIXLOWPAN)`
 *
 * @param[in] pkt                   The compressed packet to be sent. Must be in
 *                                  send order with a packet snip of type
 *                                  @ref GNRC_NETTYPE_NETIF first,
 *                                  @ref GNRC_NETTYPE_SIXLOWPAN (the IPHC
 *                                  header including NHC) second, and 0 or more
 *                                  snips of payload.
 * @param[in] orig_datagram_size    The size of the @p pkt before compression
 *                                  (without @ref GNRC_NETTYPE_NETIF snip).
 *                                  This can differ from @p frag_msg's
 *                                  gnrc_sixlowpan_msg_frag_t::datagram_size
 *                                  as it might just be a fragment in forwarding
 *                                  that is re-compressed in @p pkt.
 * @param[in] ipv6_addr             The (uncompressed) destination address of
 *                                  @p pkt.
 * @param[in] fbuf                  A fragmentation buffer entry.
 *
 * @return  0, when fragmentation was successful
 * @return  -1, on error. @p pkt is **not** released in that case and *should*
 *          be handled by normal fragmentation.
 */
int gnrc_sixlowpan_frag_minfwd_frag_iphc(gnrc_pktsnip_t *pkt,
                                         size_t orig_datagram_size,
                                         const ipv6_addr_t *ipv6_addr,
                                         gnrc_sixlowpan_frag_fb_t *fbuf);

#ifdef __cplusplus
}
#endif

/** @} */
