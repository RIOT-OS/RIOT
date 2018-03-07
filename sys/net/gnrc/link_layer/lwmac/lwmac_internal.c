/*
 * Copyright (C) 2015 Daniel Krebs
 *               2016 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc_lwmac
 * @{
 *
 * @file
 * @brief       Implementation of internal functions of LWMAC
 *
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @author      Shuguo Zhuo  <shuguo.zhuo@inria.fr>
 * @}
 */

#include <stdbool.h>

#include "periph/rtt.h"
#include "net/gnrc.h"
#include "net/gnrc/mac/mac.h"
#include "net/gnrc/lwmac/lwmac.h"
#include "include/lwmac_internal.h"
#include "net/gnrc/netif/ieee802154.h"
#include "net/netdev/ieee802154.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

int _gnrc_lwmac_transmit(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt)
{
    netdev_t *dev = netif->dev;
    netdev_ieee802154_t *state = (netdev_ieee802154_t *)netif->dev;
    gnrc_netif_hdr_t *netif_hdr;
    const uint8_t *src, *dst = NULL;
    int res = 0;
    size_t src_len, dst_len;
    uint8_t mhr[IEEE802154_MAX_HDR_LEN];
    uint8_t flags = (uint8_t)(state->flags & NETDEV_IEEE802154_SEND_MASK);
    le_uint16_t dev_pan = byteorder_btols(byteorder_htons(state->pan));

    flags |= IEEE802154_FCF_TYPE_DATA;
    if (pkt == NULL) {
        DEBUG("_send_ieee802154: pkt was NULL\n");
        return -EINVAL;
    }
    if (pkt->type != GNRC_NETTYPE_NETIF) {
        DEBUG("_send_ieee802154: first header is not generic netif header\n");
        return -EBADMSG;
    }
    netif_hdr = pkt->data;
    /* prepare destination address */
    if (netif_hdr->flags & /* If any of these flags is set assume broadcast */
        (GNRC_NETIF_HDR_FLAGS_BROADCAST | GNRC_NETIF_HDR_FLAGS_MULTICAST)) {
        dst = ieee802154_addr_bcast;
        dst_len = IEEE802154_ADDR_BCAST_LEN;
    }
    else {
        dst = gnrc_netif_hdr_get_dst_addr(netif_hdr);
        dst_len = netif_hdr->dst_l2addr_len;
    }
    src_len = netif_hdr->src_l2addr_len;
    if (src_len > 0) {
        src = gnrc_netif_hdr_get_src_addr(netif_hdr);
    }
    else {
        src_len = netif->l2addr_len;
        src = netif->l2addr;
    }
    /* fill MAC header, seq should be set by device */
    if ((res = ieee802154_set_frame_hdr(mhr, src, src_len,
                                        dst, dst_len, dev_pan,
                                        dev_pan, flags, state->seq++)) == 0) {
        DEBUG("_send_ieee802154: Error preperaring frame\n");
        return -EINVAL;
    }

    /* prepare packet for sending */
    iolist_t iolist = {
        .iol_next = (iolist_t *)pkt->next,
        .iol_base = mhr,
        .iol_len = (size_t)res
    };

#ifdef MODULE_NETSTATS_L2
    if (netif_hdr->flags &
            (GNRC_NETIF_HDR_FLAGS_BROADCAST | GNRC_NETIF_HDR_FLAGS_MULTICAST)) {
        netif->dev->stats.tx_mcast_count++;
    }
    else {
        netif->dev->stats.tx_unicast_count++;
    }
#endif
#ifdef MODULE_GNRC_MAC
    if (netif->mac.mac_info & GNRC_NETIF_MAC_INFO_CSMA_ENABLED) {
        res = csma_sender_csma_ca_send(dev, &iolist, &netif->mac.csma_conf);
    }
    else {
        res = dev->driver->send(dev, &iolist);
    }
#else
    res = dev->driver->send(dev, &iolist);
#endif

    /* release old data */
    gnrc_pktbuf_release(pkt);
    return res;
}

int _gnrc_lwmac_parse_packet(gnrc_pktsnip_t *pkt, gnrc_lwmac_packet_info_t *info)
{
    gnrc_netif_hdr_t *netif_hdr;
    gnrc_pktsnip_t *lwmac_snip;
    gnrc_lwmac_hdr_t *lwmac_hdr;

    assert(info != NULL);
    assert(pkt != NULL);

    netif_hdr = (gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_NETIF))->data;
    if (netif_hdr == NULL) {
        return -1;
    }

    /* Dissect LWMAC header, Every frame has header as first member */
    lwmac_hdr = (gnrc_lwmac_hdr_t *) pkt->data;
    switch (lwmac_hdr->type) {
        case GNRC_LWMAC_FRAMETYPE_WR: {
            lwmac_snip = gnrc_pktbuf_mark(pkt, sizeof(gnrc_lwmac_frame_wr_t),
                                          GNRC_NETTYPE_LWMAC);
            break;
        }
        case GNRC_LWMAC_FRAMETYPE_WA: {
            lwmac_snip = gnrc_pktbuf_mark(pkt, sizeof(gnrc_lwmac_frame_wa_t),
                                          GNRC_NETTYPE_LWMAC);
            break;
        }
        case GNRC_LWMAC_FRAMETYPE_DATA_PENDING:
        case GNRC_LWMAC_FRAMETYPE_DATA: {
            lwmac_snip = gnrc_pktbuf_mark(pkt, sizeof(gnrc_lwmac_frame_data_t),
                                          GNRC_NETTYPE_LWMAC);
            break;
        }
        case GNRC_LWMAC_FRAMETYPE_BROADCAST: {
            lwmac_snip = gnrc_pktbuf_mark(pkt, sizeof(gnrc_lwmac_frame_broadcast_t),
                                          GNRC_NETTYPE_LWMAC);
            break;
        }
        default: {
            return -2;
        }
    }

    /* Memory location may have changed while marking */
    lwmac_hdr = lwmac_snip->data;

    if (lwmac_hdr->type == GNRC_LWMAC_FRAMETYPE_WA) {
        /* WA is broadcast, so get dst address out of header instead of netif */
        info->dst_addr = ((gnrc_lwmac_frame_wa_t *)lwmac_hdr)->dst_addr;
    }
    else if (lwmac_hdr->type == GNRC_LWMAC_FRAMETYPE_WR) {
        /* WR is broadcast, so get dst address out of header instead of netif */
        info->dst_addr = ((gnrc_lwmac_frame_wr_t *)lwmac_hdr)->dst_addr;
    }
    else if (netif_hdr->dst_l2addr_len) {
        info->dst_addr.len = netif_hdr->dst_l2addr_len;
        memcpy(info->dst_addr.addr,
               gnrc_netif_hdr_get_dst_addr(netif_hdr),
               netif_hdr->dst_l2addr_len);
    }

    if (netif_hdr->src_l2addr_len) {
        info->src_addr.len = netif_hdr->src_l2addr_len;
        memcpy(info->src_addr.addr,
               gnrc_netif_hdr_get_src_addr(netif_hdr),
               netif_hdr->src_l2addr_len);
    }

    info->header = lwmac_hdr;
    return 0;
}

void _gnrc_lwmac_set_netdev_state(gnrc_netif_t *netif, netopt_state_t devstate)
{
    netif->dev->driver->set(netif->dev,
                            NETOPT_STATE,
                            &devstate,
                            sizeof(devstate));

#if (GNRC_LWMAC_ENABLE_DUTYCYLE_RECORD == 1)
    if (devstate == NETOPT_STATE_IDLE) {
        if (!(netif->mac.prot.lwmac.lwmac_info & GNRC_LWMAC_RADIO_IS_ON)) {
            netif->mac.prot.lwmac.last_radio_on_time_ticks = rtt_get_counter();
            netif->mac.prot.lwmac.lwmac_info |= GNRC_LWMAC_RADIO_IS_ON;
        }
        return;
    }
    else if ((devstate == NETOPT_STATE_SLEEP) &&
             (netif->mac.prot.lwmac.lwmac_info & GNRC_LWMAC_RADIO_IS_ON)) {
        netif->mac.prot.lwmac.radio_off_time_ticks = rtt_get_counter();

        netif->mac.prot.lwmac.awake_duration_sum_ticks +=
            (netif->mac.prot.lwmac.radio_off_time_ticks -
             netif->mac.prot.lwmac.last_radio_on_time_ticks);

        netif->mac.prot.lwmac.lwmac_info &= ~GNRC_LWMAC_RADIO_IS_ON;
    }
#endif
}

netopt_state_t _gnrc_lwmac_get_netdev_state(gnrc_netif_t *netif)
{
    netopt_state_t state;

    if (0 < netif->dev->driver->get(netif->dev,
                                    NETOPT_STATE,
                                    &state,
                                    sizeof(state))) {
        return state;
    }
    return -1;
}

int _gnrc_lwmac_dispatch_defer(gnrc_pktsnip_t *buffer[], gnrc_pktsnip_t *pkt)
{
    assert(buffer != NULL);
    assert(pkt != NULL);

    /* We care about speed here, so assume packet structure */
    assert(pkt->next->type == GNRC_NETTYPE_LWMAC);
    assert(pkt->next->next->type == GNRC_NETTYPE_NETIF);

    gnrc_lwmac_frame_broadcast_t *bcast = NULL;
    if (((gnrc_lwmac_hdr_t *)pkt->next->data)->type == GNRC_LWMAC_FRAMETYPE_BROADCAST) {
        bcast = pkt->next->data;
    }

    for (unsigned i = 0; i < GNRC_MAC_DISPATCH_BUFFER_SIZE; i++) {
        /* Buffer will be filled bottom-up and emptied completely so no holes */
        if (buffer[i] == NULL) {
            buffer[i] = pkt;
            return 0;
        }
        else if (bcast &&
                 (((gnrc_lwmac_hdr_t *)buffer[i]->next->data)->type == GNRC_LWMAC_FRAMETYPE_BROADCAST) &&
                 (bcast->seq_nr == ((gnrc_lwmac_frame_broadcast_t *)buffer[i]->next->data)->seq_nr)) {
            /* Filter same broadcasts, compare sequence number */
            gnrc_netif_hdr_t *hdr_queued, *hdr_new;
            hdr_new = pkt->next->next->data;
            hdr_queued = buffer[i]->next->next->data;

            /* Sequence numbers match, compare source addresses */
            if ((hdr_new->src_l2addr_len == hdr_queued->src_l2addr_len) &&
                (memcmp(gnrc_netif_hdr_get_src_addr(hdr_new),
                        gnrc_netif_hdr_get_src_addr(hdr_queued),
                        hdr_new->src_l2addr_len) == 0)) {
                /* Source addresses match, same packet */
                DEBUG("[LWMAC] Found duplicate broadcast packet, dropping\n");
                gnrc_pktbuf_release(pkt);
                return -2;
            }
        }
    }

    DEBUG("[LWMAC] Dispatch buffer full, dropping packet\n");
    gnrc_pktbuf_release(pkt);

    return -1;
}
