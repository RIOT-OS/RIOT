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

#include "net/ieee802154.h"
#include "net/gnrc/netif/hdr.h"
#include "net/gnrc/pkt.h"
#include "net/gnrc/sixlowpan.h"

#include "net/gnrc/sixlowpan/frag/rb.h"

#include "rbuf.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

void gnrc_sixlowpan_frag_rbuf_base_rm(gnrc_sixlowpan_rbuf_base_t *entry)
{
    while (entry->ints != NULL) {
        gnrc_sixlowpan_rbuf_int_t *next = entry->ints->next;

        entry->ints->start = 0;
        entry->ints->end = 0;
        entry->ints->next = NULL;
        entry->ints = next;
    }
    entry->datagram_size = 0;
}

void gnrc_sixlowpan_frag_rbuf_gc(void)
{
    rbuf_gc();
}

void gnrc_sixlowpan_frag_rbuf_remove(gnrc_sixlowpan_rbuf_t *rbuf)
{
    assert(rbuf != NULL);
    rbuf_rm(rbuf);
}

void gnrc_sixlowpan_frag_rbuf_dispatch_when_complete(gnrc_sixlowpan_rbuf_t *rbuf,
                                                     gnrc_netif_hdr_t *netif_hdr)
{
    assert(rbuf);
    assert(netif_hdr);
    if (rbuf->super.current_size == rbuf->super.datagram_size) {
        gnrc_pktsnip_t *netif = gnrc_netif_hdr_build(rbuf->super.src,
                                                     rbuf->super.src_len,
                                                     rbuf->super.dst,
                                                     rbuf->super.dst_len);

        if (netif == NULL) {
            DEBUG("6lo rbuf: error allocating netif header\n");
            gnrc_pktbuf_release(rbuf->pkt);
            gnrc_sixlowpan_frag_rbuf_remove(rbuf);
            return;
        }

        /* copy the transmit information of the latest fragment into the newly
         * created header to have some link_layer information. The link_layer
         * info of the previous fragments is discarded.
         */
        gnrc_netif_hdr_t *new_netif_hdr = netif->data;
        new_netif_hdr->if_pid = netif_hdr->if_pid;
        new_netif_hdr->flags = netif_hdr->flags;
        new_netif_hdr->lqi = netif_hdr->lqi;
        new_netif_hdr->rssi = netif_hdr->rssi;
        LL_APPEND(rbuf->pkt, netif);
        gnrc_sixlowpan_dispatch_recv(rbuf->pkt, NULL, 0);
        gnrc_sixlowpan_frag_rbuf_remove(rbuf);
    }
}


/** @} */
