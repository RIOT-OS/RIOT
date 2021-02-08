/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 * Copyright (C) 2015 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author  Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 */

#include <assert.h>
#include <inttypes.h>

#include "net/gnrc/netapi.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/netif/hdr.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/sixlowpan/frag.h"
#include "net/gnrc/sixlowpan/frag/rb.h"
#include "net/gnrc/sixlowpan/internal.h"
#include "net/gnrc/tx_sync.h"
#include "net/sixlowpan.h"
#include "utlist.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static inline uint16_t _floor8(uint16_t length)
{
    return length & 0xfff8U;
}

static inline size_t _min(size_t a, size_t b)
{
    return (a < b) ? a : b;
}

static inline uint8_t _max_frag_size(gnrc_netif_t *iface,
                                     gnrc_sixlowpan_frag_fb_t *fbuf)
{
#ifdef MODULE_GNRC_SIXLOWPAN_FRAG_HINT
    if (fbuf->hint.fragsz > 0) {
        /* account for rounding down to 8*/
        return (fbuf->hint.fragsz & 0x7)
               ? (fbuf->hint.fragsz + 8U)
               : fbuf->hint.fragsz;
    }
#endif /* MODULE_GNRC_SIXLOWPAN_FRAG_HINT */
    (void)fbuf;
    return iface->sixlo.max_frag_size;
}

static inline int _payload_diff(gnrc_sixlowpan_frag_fb_t *fbuf,
                                size_t payload_len)
{
#ifdef MODULE_GNRC_SIXLOWPAN_FRAG_HINT
    if (fbuf->hint.fragsz > 0) {
        return fbuf->hint.fragsz_uncomp - fbuf->hint.fragsz;
    }
#endif /* MODULE_GNRC_SIXLOWPAN_FRAG_HINT */
    return (fbuf->datagram_size - payload_len);
}

static gnrc_pktsnip_t *_build_frag_pkt(gnrc_pktsnip_t *pkt,
                                       gnrc_sixlowpan_frag_fb_t *fbuf,
                                       size_t payload_len, size_t size)
{
    sixlowpan_frag_t *frag_hdr;
    gnrc_netif_hdr_t *netif_hdr = pkt->data, *new_netif_hdr;
    gnrc_pktsnip_t *netif, *frag;
#ifdef MODULE_GNRC_SIXLOWPAN_FRAG_HINT
    size_t fragment_size = ((fbuf->hint.fragsz > 0) &&
                            (fbuf->offset == 0))
                         ? size     /* we want the calculated fragment size
                                     * to include full IPHC header */
                         : _min(size, payload_len);
#else   /* MODULE_GNRC_SIXLOWPAN_FRAG_HINT */
    size_t fragment_size = _min(size, payload_len);
#endif  /* MODULE_GNRC_SIXLOWPAN_FRAG_HINT */

    netif = gnrc_netif_hdr_build(gnrc_netif_hdr_get_src_addr(netif_hdr),
                                 netif_hdr->src_l2addr_len,
                                 gnrc_netif_hdr_get_dst_addr(netif_hdr),
                                 netif_hdr->dst_l2addr_len);

    if (netif == NULL) {
        DEBUG("6lo frag: error allocating new link-layer header\n");
        return NULL;
    }

    new_netif_hdr = netif->data;
    /* src_l2addr_len and dst_l2addr_len are already the same, now copy the rest */
    *new_netif_hdr = *netif_hdr;

    frag = gnrc_pktbuf_add(NULL, NULL, fragment_size, GNRC_NETTYPE_SIXLOWPAN);

    if (frag == NULL) {
        DEBUG("6lo frag: error allocating first fragment\n");
        gnrc_pktbuf_release(netif);
        return NULL;
    }
    frag_hdr = frag->data;
    /* XXX: truncation of datagram_size > 4095 may happen here */
    frag_hdr->disp_size = byteorder_htons(fbuf->datagram_size);
    frag_hdr->tag = byteorder_htons(fbuf->tag);

    return gnrc_pkt_prepend(frag, netif);
}

static uint16_t _copy_pkt_to_frag(uint8_t *data, const gnrc_pktsnip_t *pkt,
                                  uint16_t max_frag_size, uint16_t init_offset)
{
    uint16_t offset = init_offset;

    while ((pkt != NULL) && (offset < max_frag_size)) {
        uint16_t len = _min(max_frag_size - offset, pkt->size);

        memcpy(data + offset, pkt->data, len);

        offset += len;
        pkt = pkt->next;
    }
    return offset;
}

static uint16_t _send_1st_fragment(gnrc_netif_t *iface,
                                   gnrc_sixlowpan_frag_fb_t *fbuf,
                                   size_t payload_len)
{
    gnrc_pktsnip_t *frag, *pkt = fbuf->pkt;
    sixlowpan_frag_t *hdr;
    uint8_t *data;
    /* payload_len: actual size of the packet vs
     * datagram_size: size of the uncompressed IPv6 packet */
    int payload_diff = _payload_diff(fbuf, payload_len);
    uint16_t local_offset;
    /* virtually add payload_diff to flooring to account for offset (must be dividable by 8)
     * in uncompressed datagram */
    uint16_t max_frag_size = _floor8(_max_frag_size(iface, fbuf) +
                                     payload_diff - sizeof(sixlowpan_frag_t)) -
                             payload_diff;

    DEBUG("6lo frag: determined max_frag_size = %" PRIu16 "\n", max_frag_size);

    frag = _build_frag_pkt(pkt, fbuf, payload_len,
                           max_frag_size + sizeof(sixlowpan_frag_t));

    if (frag == NULL) {
        return 0;
    }

    hdr = frag->next->data;
    data = (uint8_t *)(hdr + 1);
    hdr->disp_size.u8[0] |= SIXLOWPAN_FRAG_1_DISP;

    /* Tell the link layer that we will send more fragments */
    gnrc_netif_hdr_t *netif_hdr = frag->data;
    netif_hdr->flags |= GNRC_NETIF_HDR_FLAGS_MORE_DATA;

    pkt = pkt->next;    /* don't copy netif header */
    local_offset = _copy_pkt_to_frag(data, pkt, max_frag_size, 0);

    DEBUG("6lo frag: send first fragment (datagram size: %u, "
          "datagram tag: %" PRIu16 ", fragment size: %" PRIu16 ")\n",
          fbuf->datagram_size, fbuf->tag, local_offset);
    gnrc_sixlowpan_dispatch_send(frag, NULL, 0);
    return local_offset;
}

static uint16_t _send_nth_fragment(gnrc_netif_t *iface,
                                   gnrc_sixlowpan_frag_fb_t *fbuf,
                                   size_t payload_len,
                                   gnrc_pktsnip_t **tx_sync)
{
    gnrc_pktsnip_t *frag, *pkt = fbuf->pkt;
    sixlowpan_frag_n_t *hdr;
    uint8_t *data;
    uint16_t local_offset = 0, offset_count = 0, offset = fbuf->offset;
    /* since dispatches aren't supposed to go into subsequent fragments, we need not account
     * for payload difference as for the first fragment */
    uint16_t max_frag_size = _floor8(iface->sixlo.max_frag_size -
                                     sizeof(sixlowpan_frag_n_t));

    DEBUG("6lo frag: determined max_frag_size = %" PRIu16 "\n", max_frag_size);

    frag = _build_frag_pkt(pkt, fbuf,
                           payload_len - offset + sizeof(sixlowpan_frag_n_t),
                           max_frag_size + sizeof(sixlowpan_frag_n_t));

    if (frag == NULL) {
        return 0;
    }

    hdr = frag->next->data;
    data = (uint8_t *)(hdr + 1);
    hdr->disp_size.u8[0] |= SIXLOWPAN_FRAG_N_DISP;
    /* don't mention payload diff in offset */
    hdr->offset = (uint8_t)((offset + _payload_diff(fbuf,
                                                    payload_len)) >> 3);
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
    /* copy remaining packet snips */
    local_offset = _copy_pkt_to_frag(data, pkt, max_frag_size, local_offset);

    if ((offset + local_offset) < payload_len) {
        gnrc_netif_hdr_t *netif_hdr = frag->data;
        netif_hdr->flags |= GNRC_NETIF_HDR_FLAGS_MORE_DATA;
    } else if (IS_USED(MODULE_GNRC_TX_SYNC) && (*tx_sync != NULL)) {
        gnrc_pkt_append(frag, *tx_sync);
        *tx_sync = NULL;
    }
    DEBUG("6lo frag: send subsequent fragment (datagram size: %u, "
          "datagram tag: %" PRIu16 ", offset: %" PRIu8 " (%u bytes), "
          "fragment size: %" PRIu16 ")\n",
          fbuf->datagram_size, fbuf->tag, hdr->offset,
          hdr->offset << 3, local_offset);
    gnrc_sixlowpan_dispatch_send(frag, NULL, 0);
    return local_offset;
}

void gnrc_sixlowpan_frag_send(gnrc_pktsnip_t *pkt, void *ctx, unsigned page)
{
    assert(ctx != NULL);
    gnrc_sixlowpan_frag_fb_t *fbuf = ctx;
    gnrc_netif_t *iface;
    gnrc_pktsnip_t *tx_sync = NULL;
    uint16_t res;
    /* payload_len: actual size of the packet vs
     * datagram_size: size of the uncompressed IPv6 packet */
    size_t payload_len = gnrc_pkt_len(fbuf->pkt->next);

    assert((fbuf->pkt == pkt) || (pkt == NULL));
    (void)page;
    (void)pkt;
    iface = gnrc_netif_hdr_get_netif(fbuf->pkt->data);
#if defined(DEVELHELP) && ENABLE_DEBUG
    if (iface == NULL) {
        DEBUG("6lo frag: iface == NULL, expect segmentation fault.\n");
        /* remove original packet from packet buffer */
        gnrc_pktbuf_release(fbuf->pkt);
        /* 6LoWPAN free for next fragmentation */
        fbuf->pkt = NULL;
        return;
    }
#endif

    if (IS_USED(MODULE_GNRC_TX_SYNC)) {
        tx_sync = gnrc_tx_sync_split((pkt) ? pkt : fbuf->pkt);
    }

    /* Check whether to send the first or an Nth fragment */
    if (fbuf->offset == 0) {
        if ((res = _send_1st_fragment(iface, fbuf, payload_len)) == 0) {
            /* error sending first fragment */
            DEBUG("6lo frag: error sending 1st fragment\n");
            goto error;
        }
    }
    /* (offset + (datagram_size - payload_len) < datagram_size) simplified */
    else if (fbuf->offset < payload_len) {
        if ((res = _send_nth_fragment(iface, fbuf, payload_len, &tx_sync)) == 0) {
            /* error sending subsequent fragment */
            DEBUG("6lo frag: error sending subsequent fragment"
                  "(offset = %u)\n", fbuf->offset);
            goto error;
        }
    }
    else {
        goto error;
    }
    fbuf->offset += res;
    if (IS_USED(MODULE_GNRC_TX_SYNC) && tx_sync) {
        /* re-attach tx_sync to allow releasing it at end
         * of transmission, or transmission failure */
        gnrc_pkt_append((pkt) ? pkt : fbuf->pkt, tx_sync);
    }
    if (!gnrc_sixlowpan_frag_fb_send(fbuf)) {
        DEBUG("6lo frag: message queue full, can't issue next fragment "
              "sending\n");
        goto error;
    }
    thread_yield();
    return;
error:
    gnrc_pktbuf_release(fbuf->pkt);
    fbuf->pkt = NULL;
    if (IS_USED(MODULE_GNRC_TX_SYNC) && tx_sync) {
        gnrc_pktbuf_release(tx_sync);
    }
}

void gnrc_sixlowpan_frag_recv(gnrc_pktsnip_t *pkt, void *ctx, unsigned page)
{
    gnrc_pktsnip_t *netif_hdr = pkt->next;
    gnrc_netif_hdr_t *hdr = netif_hdr->data;
    sixlowpan_frag_t *frag = pkt->data;
    gnrc_sixlowpan_frag_rb_t *rbe;
    uint16_t offset = 0;

    (void)ctx;
    switch (frag->disp_size.u8[0] & SIXLOWPAN_FRAG_DISP_MASK) {
        case SIXLOWPAN_FRAG_1_DISP:
            break;

        case SIXLOWPAN_FRAG_N_DISP:
            offset = (((sixlowpan_frag_n_t *)frag)->offset * 8);
            break;

        default:
            DEBUG("6lo rbuf: Not a fragment header.\n");
            gnrc_pktbuf_release(pkt);

            return;
    }

    gnrc_pktbuf_hold(netif_hdr, 1); /* hold netif header to use it with
                                     * dispatch_when_complete()
                                     * (rb_add() releases `pkt`) */
    rbe = gnrc_sixlowpan_frag_rb_add(hdr, pkt, offset, page);
    if (rbe != NULL) {
        gnrc_sixlowpan_frag_rb_dispatch_when_complete(rbe, hdr);
    }
    gnrc_pktbuf_release(netif_hdr);
}

/** @} */
