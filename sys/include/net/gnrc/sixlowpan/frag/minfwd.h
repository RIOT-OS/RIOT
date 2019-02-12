/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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
#ifndef NET_GNRC_SIXLOWPAN_FRAG_MINFWD_H
#define NET_GNRC_SIXLOWPAN_FRAG_MINFWD_H

#include <stddef.h>

#include "net/gnrc/pkt.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/sixlowpan/frag.h"
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

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_SIXLOWPAN_FRAG_MINFWD_H */
/** @} */
