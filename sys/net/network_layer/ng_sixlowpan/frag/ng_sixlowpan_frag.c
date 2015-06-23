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
#include "net/ng_pktbuf.h"
#include "net/ng_netapi.h"
#include "net/ng_netif/hdr.h"
#include "net/ng_sixlowpan/frag.h"
#include "net/ng_sixlowpan/netif.h"
#include "utlist.h"

#include "rbuf.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

static uint16_t _tag;

static inline uint16_t _floor8(uint16_t length)
{
    return length & 0xf8U;
}

static inline size_t _min(size_t a, size_t b)
{
    return (a < b) ? a : b;
}

static ng_pktsnip_t *_build_frag_pkt(ng_pktsnip_t *pkt, size_t payload_len,
                                     size_t size)
{
    ng_netif_hdr_t *hdr = pkt->data, *new_hdr;
    ng_pktsnip_t *netif, *frag;

    netif = ng_netif_hdr_build(ng_netif_hdr_get_src_addr(hdr), hdr->src_l2addr_len,
                               ng_netif_hdr_get_dst_addr(hdr), hdr->dst_l2addr_len);

    if (netif == NULL) {
        DEBUG("6lo frag: error allocating new link-layer header\n");
        return NULL;
    }

    new_hdr = netif->data;
    new_hdr->if_pid = hdr->if_pid;
    new_hdr->flags = hdr->flags;
    new_hdr->rssi = hdr->rssi;
    new_hdr->lqi = hdr->lqi;

    frag = ng_pktbuf_add(NULL, NULL, _min(size, payload_len),
                         NG_NETTYPE_SIXLOWPAN);

    if (frag == NULL) {
        DEBUG("6lo frag: error allocating first fragment\n");
        ng_pktbuf_release(netif);
        return NULL;
    }

    LL_PREPEND(frag, netif);

    return frag;
}

static uint16_t _send_1st_fragment(ng_sixlowpan_netif_t *iface, ng_pktsnip_t *pkt,
                                   size_t payload_len, size_t datagram_size)
{
    ng_pktsnip_t *frag;
    uint16_t max_frag_size = _floor8(iface->max_frag_size -
                                     (payload_len - datagram_size) -
                                     sizeof(ng_sixlowpan_frag_t));
    uint16_t local_offset = 0;
    ng_sixlowpan_frag_t *hdr;
    uint8_t *data;

    DEBUG("6lo frag: determined max_frag_size = %" PRIu16 "\n", max_frag_size);

    /* 6LoWPAN dispatches don't count into that */
    max_frag_size += (payload_len - datagram_size);

    frag = _build_frag_pkt(pkt, payload_len,
                           max_frag_size + sizeof(ng_sixlowpan_frag_t));

    if (frag == NULL) {
        return 0;
    }

    hdr = frag->next->data;
    data = (uint8_t *)(hdr + 1);

    hdr->disp_size = byteorder_htons((uint16_t)datagram_size);
    hdr->disp_size.u8[0] |= NG_SIXLOWPAN_FRAG_1_DISP;
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
    ng_netapi_send(iface->pid, frag);

    return local_offset;
}

static uint16_t _send_nth_fragment(ng_sixlowpan_netif_t *iface, ng_pktsnip_t *pkt,
                                   size_t payload_len, size_t datagram_size,
                                   uint16_t offset)
{
    ng_pktsnip_t *frag;
    uint16_t max_frag_size = _floor8(iface->max_frag_size - sizeof(ng_sixlowpan_frag_n_t));
    uint16_t local_offset = 0, offset_count = 0;
    ng_sixlowpan_frag_n_t *hdr;
    uint8_t *data;

    DEBUG("6lo frag: determined max_frag_size = %" PRIu16 "\n", max_frag_size);

    frag = _build_frag_pkt(pkt,
                           payload_len - offset + sizeof(ng_sixlowpan_frag_n_t),
                           max_frag_size + sizeof(ng_sixlowpan_frag_n_t));

    if (frag == NULL) {
        return 0;
    }

    hdr = frag->next->data;
    data = (uint8_t *)(hdr + 1);

    /* XXX: truncation of datagram_size > 4095 may happen here */
    hdr->disp_size = byteorder_htons((uint16_t)datagram_size);
    hdr->disp_size.u8[0] |= NG_SIXLOWPAN_FRAG_N_DISP;
    hdr->tag = byteorder_htons(_tag);
    hdr->offset = (uint8_t)(offset >> 3);
    pkt = pkt->next;    /* don't copy netif header */

    while ((pkt != NULL) || (offset_count == offset)) {   /* go to offset */
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

    DEBUG("6lo frag: send first fragment (datagram size: %u, "
          "datagram tag: %" PRIu16 ", offset: %" PRIu8 " (%u bytes), "
          "fragment size: %" PRIu16 ")\n",
          (unsigned int)datagram_size, _tag, hdr->offset, hdr->offset << 3,
          local_offset);
    ng_netapi_send(iface->pid, frag);

    return local_offset;
}

void ng_sixlowpan_frag_send(kernel_pid_t pid, ng_pktsnip_t *pkt,
                            size_t payload_len, size_t datagram_size)
{
    ng_sixlowpan_netif_t *iface = ng_sixlowpan_netif_get(pid);
    uint16_t offset = 0, res;

#if defined(DEVELHELP) && defined(ENABLE_DEBUG)
    if (iface == NULL) {
        DEBUG("6lo frag: iface == NULL, expect segmentation fault.\n");
        ng_pktbuf_release(pkt);
        return;
    }
#endif

    if ((res = _send_1st_fragment(iface, pkt, payload_len, datagram_size)) == 0) {
        /* error sending first fragment */
        ng_pktbuf_release(pkt);
        return;
    }

    offset += res;

    while (offset < datagram_size) {
        if ((res = _send_nth_fragment(iface, pkt, payload_len, datagram_size,
                                      offset)) == 0) {
            /* error sending first fragment */
            ng_pktbuf_release(pkt);
            return;
        }

        offset += res;
    }

    /* remove original packet from packet buffer */
    ng_pktbuf_release(pkt);
    _tag++;
}

void ng_sixlowpan_frag_handle_pkt(ng_pktsnip_t *pkt)
{
    ng_netif_hdr_t *hdr = pkt->next->data;
    ng_sixlowpan_frag_t *frag = pkt->data;
    uint16_t offset = 0;
    size_t frag_size;

    switch (frag->disp_size.u8[0] & NG_SIXLOWPAN_FRAG_DISP_MASK) {
        case NG_SIXLOWPAN_FRAG_1_DISP:
            frag_size = (pkt->size - sizeof(ng_sixlowpan_frag_t));
            break;

        case NG_SIXLOWPAN_FRAG_N_DISP:
            offset = (((ng_sixlowpan_frag_n_t *)frag)->offset * 8);
            frag_size = (pkt->size - sizeof(ng_sixlowpan_frag_n_t));
            break;

        default:
            DEBUG("6lo rbuf: Not a fragment header.\n");
            ng_pktbuf_release(pkt);

            return;
    }

    rbuf_add(hdr, frag, frag_size, offset);

    ng_pktbuf_release(pkt);
}

/** @} */
