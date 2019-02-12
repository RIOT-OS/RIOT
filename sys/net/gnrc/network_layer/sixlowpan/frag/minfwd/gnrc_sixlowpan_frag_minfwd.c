/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <errno.h>
#include <stdbool.h>

#include "net/gnrc/netif/hdr.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/sixlowpan/internal.h"
#include "utlist.h"

#include "net/gnrc/sixlowpan/frag/minfwd.h"

#define ENABLE_DEBUG    0
#include "debug.h"

static gnrc_pktsnip_t *_netif_hdr_from_vrbe(const gnrc_sixlowpan_frag_vrb_t *vrbe)
{
    gnrc_pktsnip_t *res = gnrc_netif_hdr_build(NULL, 0, vrbe->super.dst,
                                               vrbe->super.dst_len);
    if (res == NULL) {
        DEBUG("6lo minfwd: can't allocate netif header for forwarding.\n");
        return NULL;
    }
    gnrc_netif_hdr_set_netif(res->data, vrbe->out_netif);
    return res;
}

static inline bool _is_last_frag(const gnrc_sixlowpan_frag_vrb_t *vrbe)
{
    return (vrbe->super.current_size >= vrbe->super.datagram_size);
}

int gnrc_sixlowpan_frag_minfwd_forward(gnrc_pktsnip_t *pkt,
                                       const sixlowpan_frag_n_t *frag,
                                       gnrc_sixlowpan_frag_vrb_t *vrbe,
                                       unsigned page)
{
    sixlowpan_frag_t *new;
    gnrc_pktsnip_t *tmp;
    const size_t fragsnip_size = sizeof(sixlowpan_frag_t) +
                                 ((sixlowpan_frag_1_is((sixlowpan_frag_t *)frag))
                                 ? 0U : sizeof(frag->offset));

    assert(vrbe != NULL);
    assert(pkt != NULL);
    assert(frag != NULL);
    if ((tmp = gnrc_pktbuf_add(pkt, frag, fragsnip_size,
                               GNRC_NETTYPE_SIXLOWPAN)) == NULL) {
        DEBUG("6lo minfwd: unable to allocate new fragmentation header.\n");
        gnrc_pktbuf_release(pkt);
        return -ENOMEM;
    }
    pkt = tmp;
    new = pkt->data;
    new->tag = byteorder_htons(vrbe->out_tag);
    tmp = _netif_hdr_from_vrbe(vrbe);
    if (tmp == NULL) {
        gnrc_pktbuf_release(pkt);
        return -ENOMEM;
    }
    if (_is_last_frag(vrbe)) {
        DEBUG("6lo minfwd: current_size (%u) >= datagram_size (%u)\n",
              vrbe->super.current_size, vrbe->super.datagram_size);
        gnrc_sixlowpan_frag_vrb_rm(vrbe);
    }
    else {
        gnrc_netif_hdr_t *netif_hdr = tmp->data;

        netif_hdr->flags |= GNRC_NETIF_HDR_FLAGS_MORE_DATA;
    }
    pkt = gnrc_pkt_prepend(pkt, tmp);
    gnrc_sixlowpan_dispatch_send(pkt, NULL, page);
    return 0;
}

/** @} */
