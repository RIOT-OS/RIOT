/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 */

#include "kernel_types.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/netapi.h"
#include "net/gnrc/netif/hdr.h"
#include "net/gnrc/sixlowpan/frag.h"
#include "net/gnrc/sixlowpan/netif.h"
#include "net/sixlowpan.h"
#include "utlist.h"

#include "rbuf.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if ENABLE_DEBUG
/* For PRIu16 etc. */
#include <inttypes.h>
#endif

static uint16_t _tag;

static inline uint16_t _floor8(uint16_t length)
{
    return length & 0xf8U;
}

static inline size_t _min(size_t a, size_t b)
{
    return (a < b) ? a : b;
}

static gnrc_pktsnip_t *_build_frag_pkt(gnrc_pktsnip_t *pkt, size_t payload_len,
                                       size_t size)
{
    gnrc_netif_hdr_t *hdr = pkt->data, *new_hdr;
    gnrc_pktsnip_t *netif, *frag;

    netif = gnrc_netif_hdr_build(gnrc_netif_hdr_get_src_addr(hdr), hdr->src_l2addr_len,
                                 gnrc_netif_hdr_get_dst_addr(hdr), hdr->dst_l2addr_len);

    if (netif == NULL) {
        DEBUG("6lo frag: error allocating new link-layer header\n");
        return NULL;
    }

    new_hdr = netif->data;
    new_hdr->if_pid = hdr->if_pid;
    new_hdr->flags = hdr->flags;
    new_hdr->rssi = hdr->rssi;
    new_hdr->lqi = hdr->lqi;

    frag = gnrc_pktbuf_add(NULL, NULL, _min(size, payload_len),
                           GNRC_NETTYPE_SIXLOWPAN);

    if (frag == NULL) {
        DEBUG("6lo frag: error allocating first fragment\n");
        gnrc_pktbuf_release(netif);
        return NULL;
    }

    LL_PREPEND(frag, netif);

    return frag;
}

static uint16_t _send_1st_fragment(gnrc_sixlowpan_netif_t *iface, gnrc_pktsnip_t *pkt,
                                   size_t payload_len, size_t datagram_size)
{
    gnrc_pktsnip_t *frag;
    uint16_t local_offset = 0;
    /* payload_len: actual size of the packet vs
     * datagram_size: size of the uncompressed IPv6 packet */
    int payload_diff = (datagram_size - payload_len);
    /* virtually add payload_diff to flooring to account for offset (must be divisable by 8)
     * in uncompressed datagram */
    uint16_t max_frag_size = _floor8(iface->max_frag_size + payload_diff -
                                     sizeof(sixlowpan_frag_t)) - payload_diff;
    sixlowpan_frag_t *hdr;
    uint8_t *data;

    DEBUG("6lo frag: determined max_frag_size = %" PRIu16 "\n", max_frag_size);

    frag = _build_frag_pkt(pkt, payload_len,
                           max_frag_size + sizeof(sixlowpan_frag_t));

    if (frag == NULL) {
        return 0;
    }

    hdr = frag->next->data;
    data = (uint8_t *)(hdr + 1);

    hdr->disp_size = byteorder_htons((uint16_t)datagram_size);
    hdr->disp_size.u8[0] |= SIXLOWPAN_FRAG_1_DISP;
    hdr->tag = byteorder_htons(_tag);

    pkt = pkt->next;    /* don't copy netif header */

    while (pkt != NULL) {
        size_t clen = _min(max_frag_size - local_offset, pkt->size);

        memcpy(data + local_offset, pkt->data, clen);
        local_offset += clen;

        if (local_offset >= max_frag_size) {
            break;
        }

        pkt = pkt->next;
    }

    DEBUG("6lo frag: send first fragment (datagram size: %u, "
          "datagram tag: %" PRIu16 ", fragment size: %" PRIu16 ")\n",
          (unsigned int)datagram_size, _tag, local_offset);
    if (gnrc_netapi_send(iface->pid, frag) < 1) {
        DEBUG("6lo frag: unable to send first fragment\n");
        gnrc_pktbuf_release(frag);
    }

    return local_offset;
}

static uint16_t _send_nth_fragment(gnrc_sixlowpan_netif_t *iface, gnrc_pktsnip_t *pkt,
                                   size_t payload_len, size_t datagram_size,
                                   uint16_t offset)
{
    gnrc_pktsnip_t *frag;
    /* since dispatches aren't supposed to go into subsequent fragments, we need not account
     * for payload difference as for the first fragment */
    uint16_t max_frag_size = _floor8(iface->max_frag_size - sizeof(sixlowpan_frag_n_t));
    uint16_t local_offset = 0, offset_count = 0;
    sixlowpan_frag_n_t *hdr;
    uint8_t *data;

    DEBUG("6lo frag: determined max_frag_size = %" PRIu16 "\n", max_frag_size);

    frag = _build_frag_pkt(pkt,
                           payload_len - offset + sizeof(sixlowpan_frag_n_t),
                           max_frag_size + sizeof(sixlowpan_frag_n_t));

    if (frag == NULL) {
        return 0;
    }

    hdr = frag->next->data;
    data = (uint8_t *)(hdr + 1);

    /* XXX: truncation of datagram_size > 4095 may happen here */
    hdr->disp_size = byteorder_htons((uint16_t)datagram_size);
    hdr->disp_size.u8[0] |= SIXLOWPAN_FRAG_N_DISP;
    hdr->tag = byteorder_htons(_tag);
    /* don't mention payload diff in offset */
    hdr->offset = (uint8_t)((offset + (datagram_size - payload_len)) >> 3);
    pkt = pkt->next;    /* don't copy netif header */

    while ((pkt != NULL) && (offset_count != offset)) {   /* go to offset */
        offset_count += (uint16_t)pkt->size;

        if (offset_count > offset) {    /* we overshot */
            /* => copy rest of partly send packet snip */
            uint16_t pkt_offset = offset - (offset_count - ((uint16_t)pkt->size));
            size_t clen = _min(max_frag_size, pkt->size - pkt_offset);

            memcpy(data, ((uint8_t *)pkt->data) + pkt_offset, clen);
            local_offset = clen;
            pkt = pkt->next;
            break;
        }

        pkt = pkt->next;
    }

    if (local_offset < max_frag_size) { /* copy other packet snips */
        while (pkt != NULL) {
            size_t clen = _min(max_frag_size - local_offset, pkt->size);

            memcpy(data + local_offset, pkt->data, clen);
            local_offset += clen;

            if (local_offset == max_frag_size) {
                break;
            }

            pkt = pkt->next;
        }
    }

    DEBUG("6lo frag: send subsequent fragment (datagram size: %u, "
          "datagram tag: %" PRIu16 ", offset: %" PRIu8 " (%u bytes), "
          "fragment size: %" PRIu16 ")\n",
          (unsigned int)datagram_size, _tag, hdr->offset, hdr->offset << 3,
          local_offset);
    if (gnrc_netapi_send(iface->pid, frag) < 1) {
        DEBUG("6lo frag: unable to send subsequent fragment\n");
        gnrc_pktbuf_release(frag);
    }

    return local_offset;
}

void gnrc_sixlowpan_frag_send(kernel_pid_t pid, gnrc_pktsnip_t *pkt,
                              size_t datagram_size)
{
    gnrc_sixlowpan_netif_t *iface = gnrc_sixlowpan_netif_get(pid);
    uint16_t offset = 0, res;
    /* payload_len: actual size of the packet vs
     * datagram_size: size of the uncompressed IPv6 packet */
    size_t payload_len = gnrc_pkt_len(pkt->next);

#if defined(DEVELHELP) && defined(ENABLE_DEBUG)
    if (iface == NULL) {
        DEBUG("6lo frag: iface == NULL, expect segmentation fault.\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
#endif

    if ((res = _send_1st_fragment(iface, pkt, payload_len, datagram_size)) == 0) {
        /* error sending first fragment */
        DEBUG("6lo frag: error sending 1st fragment\n");
        gnrc_pktbuf_release(pkt);
        return;
    }

    offset += res;
    thread_yield();

    /* (offset + (datagram_size - payload_len) < datagram_size) simplified */
    while (offset < payload_len) {
        if ((res = _send_nth_fragment(iface, pkt, payload_len, datagram_size,
                                      offset)) == 0) {
            /* error sending subsequent fragment */
            DEBUG("6lo frag: error sending subsequent fragment (offset = %" PRIu16
                  ")\n", offset);
            gnrc_pktbuf_release(pkt);
            return;
        }

        offset += res;
        thread_yield();
    }

    /* remove original packet from packet buffer */
    gnrc_pktbuf_release(pkt);
    _tag++;
}

void gnrc_sixlowpan_frag_handle_pkt(gnrc_pktsnip_t *pkt)
{
    gnrc_netif_hdr_t *hdr = pkt->next->data;
    sixlowpan_frag_t *frag = pkt->data;
    uint16_t offset = 0;
    size_t frag_size;

    switch (frag->disp_size.u8[0] & SIXLOWPAN_FRAG_DISP_MASK) {
        case SIXLOWPAN_FRAG_1_DISP:
            frag_size = (pkt->size - sizeof(sixlowpan_frag_t));
            break;

        case SIXLOWPAN_FRAG_N_DISP:
            offset = (((sixlowpan_frag_n_t *)frag)->offset * 8);
            frag_size = (pkt->size - sizeof(sixlowpan_frag_n_t));
            break;

        default:
            DEBUG("6lo rbuf: Not a fragment header.\n");
            gnrc_pktbuf_release(pkt);

            return;
    }

    rbuf_add(hdr, pkt, frag_size, offset);

    gnrc_pktbuf_release(pkt);
}

/** @} */
