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
#include "net/gnrc/netdev.h"
#include "net/gnrc/lwmac/lwmac.h"
#include "include/lwmac_internal.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

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

void _gnrc_lwmac_set_netdev_state(gnrc_netdev_t *gnrc_netdev, netopt_state_t devstate)
{
    gnrc_netdev->dev->driver->set(gnrc_netdev->dev,
                                  NETOPT_STATE,
                                  &devstate,
                                  sizeof(devstate));

#if (GNRC_LWMAC_ENABLE_DUTYCYLE_RECORD == 1)
    if (devstate == NETOPT_STATE_IDLE) {
        if (!(gnrc_netdev->lwmac.lwmac_info & GNRC_LWMAC_RADIO_IS_ON)) {
            gnrc_netdev->lwmac.last_radio_on_time_ticks = rtt_get_counter();
            gnrc_netdev->lwmac.lwmac_info |= GNRC_LWMAC_RADIO_IS_ON;
        }
        return;
    }
    else if ((devstate == NETOPT_STATE_SLEEP) &&
             (gnrc_netdev->lwmac.lwmac_info & GNRC_LWMAC_RADIO_IS_ON)) {
        gnrc_netdev->lwmac.radio_off_time_ticks = rtt_get_counter();

        gnrc_netdev->lwmac.awake_duration_sum_ticks +=
            (gnrc_netdev->lwmac.radio_off_time_ticks -
             gnrc_netdev->lwmac.last_radio_on_time_ticks);

        gnrc_netdev->lwmac.lwmac_info &= ~GNRC_LWMAC_RADIO_IS_ON;
    }
#endif
}

netopt_state_t _gnrc_lwmac_get_netdev_state(gnrc_netdev_t *gnrc_netdev)
{
    netopt_state_t state;

    if (0 < gnrc_netdev->dev->driver->get(gnrc_netdev->dev,
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
