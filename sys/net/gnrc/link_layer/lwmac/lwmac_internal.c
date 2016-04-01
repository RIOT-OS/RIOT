/*
 * Copyright (C) 2015 Daniel Krebs
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     net_lwmac
 * @file
 * @brief       Internal functions of LWMAC
 *
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @}
 */

#include <stdbool.h>
#include <periph/rtt.h>
#include <net/gnrc.h>
#include <net/gnrc/lwmac/lwmac.h>
#include <net/gnrc/lwmac/packet_queue.h>

#include "include/lwmac_internal.h"
#include "include/lwmac_types.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/******************************************************************************/

int _get_dest_address(gnrc_pktsnip_t* pkt, uint8_t* pointer_to_addr[])
{
    int res;
    gnrc_netif_hdr_t* netif_hdr;

    if(!pkt)
        return -ENODEV;

    netif_hdr = (gnrc_netif_hdr_t*) pkt->data;
    if( (res = netif_hdr->dst_l2addr_len) <= 0)
        return -ENOENT;

    *pointer_to_addr = gnrc_netif_hdr_get_dst_addr(netif_hdr);
    return res;
}

/******************************************************************************/

/* Find a payload based on it's protocol type */
void* _gnrc_pktbuf_find(gnrc_pktsnip_t* pkt, gnrc_nettype_t type)
{
    while(pkt != NULL)
    {
        if(pkt->type == type) {
            return pkt->data;
        }
        pkt = pkt->next;
    }
    return NULL;
}

/******************************************************************************/

int _find_neighbour(lwmac_t* lwmac, uint8_t* dst_addr, int addr_len)
{
    lwmac_tx_neighbour_t* neighbours = lwmac->tx.neighbours;

    for(int i = 0; i < LWMAC_NEIGHBOUR_COUNT; i++) {
        if(neighbours[i].l2_addr.len == addr_len) {
            if(memcmp(&(neighbours[i].l2_addr.addr), dst_addr, addr_len) == 0) {
                return i;
            }
        }
    }
    return -1;
}

/******************************************************************************/

/* Free first empty queue that is not active */
int _free_neighbour(lwmac_t* lwmac)
{
    lwmac_tx_neighbour_t* neighbours = lwmac->tx.neighbours;

    for(int i = 0; i < LWMAC_NEIGHBOUR_COUNT; i++) {
        if( (packet_queue_length(&(neighbours[i].queue)) == 0) &&
            (&neighbours[i] != lwmac->tx.current_neighbour) ) {
            /* Mark as free */
            neighbours[i].l2_addr.len = 0;
            return i;
        }
    }
    return -1;
}

/******************************************************************************/

int _alloc_neighbour(lwmac_t* lwmac)
{
    lwmac_tx_neighbour_t* neighbours = lwmac->tx.neighbours;

    for(int i = 0; i < LWMAC_NEIGHBOUR_COUNT; i++) {
        if(neighbours[i].l2_addr.len == 0) {
            packet_queue_init(&(neighbours[i].queue),
                              lwmac->tx._queue_nodes,
                              (sizeof(lwmac->tx._queue_nodes) / sizeof(packet_queue_node_t)));
            return i;
        }
    }
    return -1;
}

/******************************************************************************/

void _init_neighbour(lwmac_tx_neighbour_t* neighbour, uint8_t* addr, int len)
{
    assert(neighbour != NULL);
    assert(addr  != NULL);
    assert(len > 0);

    neighbour->l2_addr.len = len;
    neighbour->phase = LWMAC_PHASE_UNINITIALIZED;
    memcpy(&(neighbour->l2_addr.addr), addr, len);
}

/******************************************************************************/

//static bool _tx_packet_present(lwmac_t* lwmac)
//{
//    for(int i = 0; i < LWMAC_NEIGHBOUR_COUNT; i++) {
//        if( (lwmac->tx.queues[i].addr_len > 0) &&
//            (packet_queue_length(&lwmac->tx.queues[i].queue) > 0) ) {
//            return true;
//        }
//    }
//    return false;
//}

/******************************************************************************/
/* TODO: maybe static inline */
uint32_t _ticks_to_phase(uint32_t ticks)
{
    return (ticks % RTT_US_TO_TICKS(LWMAC_WAKEUP_INTERVAL_US));
}

/******************************************************************************/
/* TODO: maybe static inline */
uint32_t _phase_to_ticks(uint32_t phase)
{
    uint32_t rtt_now = rtt_get_counter();
    uint32_t phase_now = _ticks_to_phase(rtt_now);

    /* Start of current interval */
    rtt_now -= phase_now;

    /* Phase only in next interval */
    if(phase < phase_now) {
        rtt_now += RTT_US_TO_TICKS(LWMAC_WAKEUP_INTERVAL_US);
    }

    /* Advance to phase */
    return (rtt_now + phase);
}

/******************************************************************************/
/* TODO: maybe static inline */
uint32_t _phase_now(void)
{
    return _ticks_to_phase(rtt_get_counter());
}

/******************************************************************************/
/* TODO: maybe static inline */
uint32_t _ticks_until_phase(uint32_t phase)
{
    long int tmp = phase - _phase_now();
    if(tmp < 0) {
        /* Phase in next interval */
        tmp += RTT_US_TO_TICKS(LWMAC_WAKEUP_INTERVAL_US);
    }

    return (uint32_t)tmp;
}

/******************************************************************************/

/* Find the neighbour that has a packet queued and is next for sending */
lwmac_tx_neighbour_t* _next_tx_neighbour(lwmac_t* lwmac)
{
    int next = -1;

    uint32_t phase_check;
    uint32_t phase_nearest = LWMAC_PHASE_MAX;

    for(int i = 0; i < LWMAC_NEIGHBOUR_COUNT; i++) {

        if(packet_queue_length(&(_get_neighbour(lwmac, i)->queue)) > 0) {

            /* Unknown destinations are initialized with their phase at the end
             * of the local interval, so known destinations that still wakeup
             * in this interval will be preferred. */
            phase_check = _ticks_until_phase(_get_neighbour(lwmac, i)->phase);

            if(phase_check <= phase_nearest) {
                next = i;
                phase_nearest = phase_check;
                DEBUG("[lwmac-int] Advancing queue #%d\n", i);
            }
        }
    }

    return (next < 0) ? NULL : &(lwmac->tx.neighbours[next]);
}

/******************************************************************************/

int _time_until_tx_us(lwmac_t* lwmac)
{
    lwmac_tx_neighbour_t* neighbour = _next_tx_neighbour(lwmac);

    if(neighbour == NULL) {
        return -1;
    }
    return RTT_TICKS_TO_US(_ticks_until_phase(neighbour->phase));
}

/******************************************************************************/

bool _queue_tx_packet(lwmac_t* lwmac,  gnrc_pktsnip_t* pkt)
{

    lwmac_tx_neighbour_t* neighbour;
    int neighbour_id;

    if(_packet_is_broadcast(pkt)) {
        /* Broadcast queue is neighbour 0 by definition */
        neighbour_id = 0;
        neighbour = _get_neighbour(lwmac, neighbour_id);

    } else {
        uint8_t* addr;
        int addr_len;
        bool neighbour_known = true;

        /* Get destination address of packet */
        addr_len = _get_dest_address(pkt, &addr);
        if(addr_len <= 0) {
            DEBUG("[lwmac-int] Packet has no destination address\n");
            gnrc_pktbuf_release(pkt);
            return false;
        }

        /* Search for existing queue for destination */
        neighbour_id = _find_neighbour(lwmac, addr, addr_len);

        /* Neighbour node doesn't have a queue yet */
        if(neighbour_id < 0) {
            neighbour_known = false;

            /* Try to allocate neighbour entry */
            neighbour_id = _alloc_neighbour(lwmac);

            /* No neighbour entries left */
            if(neighbour_id < 0) {
                DEBUG("[lwmac-int] No neighbour entries left, maybe increase "
                      "LWMAC_NEIGHBOUR_COUNT for better performance\n");

                /* Try to free an unused queue */
                neighbour_id = _free_neighbour(lwmac);

                /* All queues are in use, so reject */
                if(neighbour_id < 0) {
                    DEBUG("[lwmac-int] Couldn't allocate tx queue for packet\n");
                    gnrc_pktbuf_release(pkt);
                    return false;
                }
            }
        }

        neighbour = _get_neighbour(lwmac, neighbour_id);

        if(!neighbour_known) {
            _init_neighbour(neighbour, addr, addr_len);
        }

    }

    if(packet_queue_push(&(neighbour->queue), pkt, 0) == NULL) {
        DEBUG("[lwmac-int] Can't push to neighbour #%d's queue, no entries left\n",
                neighbour_id);
        gnrc_pktbuf_release(pkt);
        return false;
    }

    DEBUG("[lwmac-int] Queuing pkt to neighbour #%d\n", neighbour_id);

    return true;
}

/******************************************************************************/

int _parse_packet(gnrc_pktsnip_t* pkt, lwmac_packet_info_t* info)
{
    gnrc_netif_hdr_t* netif_hdr;
    gnrc_pktsnip_t* lwmac_snip;
    lwmac_hdr_t* lwmac_hdr;

    assert(info != NULL);
    assert(pkt != NULL);

    netif_hdr = _gnrc_pktbuf_find(pkt, GNRC_NETTYPE_NETIF);
    if(netif_hdr == NULL) {
        return -1;
    }

    /* Dissect lwMAC header */

    /* every frame has header as first member */
    lwmac_hdr = (lwmac_hdr_t*) pkt->data;
    switch(lwmac_hdr->type) {
    case FRAMETYPE_WR:
        lwmac_snip = gnrc_pktbuf_mark(pkt, sizeof(lwmac_frame_wr_t), GNRC_NETTYPE_LWMAC);
        break;
    case FRAMETYPE_WA:
        lwmac_snip = gnrc_pktbuf_mark(pkt, sizeof(lwmac_frame_wa_t), GNRC_NETTYPE_LWMAC);
        break;
    case FRAMETYPE_DATA:
        lwmac_snip = gnrc_pktbuf_mark(pkt, sizeof(lwmac_frame_data_t), GNRC_NETTYPE_LWMAC);
        break;
    case FRAMETYPE_BROADCAST:
        lwmac_snip = gnrc_pktbuf_mark(pkt, sizeof(lwmac_frame_broadcast_t), GNRC_NETTYPE_LWMAC);
        break;
    default:
        return -2;
    }

    /* Memory location may have changed while marking */
    lwmac_hdr = lwmac_snip->data;

    if(netif_hdr->dst_l2addr_len > sizeof(info->dst_addr)) {
        return -3;
    }

    if(netif_hdr->src_l2addr_len > sizeof(info->src_addr)) {
        return -4;
    }

    if(lwmac_hdr->type == FRAMETYPE_WA) {
        /* WA is broadcast, so get dst address out of header instead of netif */
        info->dst_addr = ((lwmac_frame_wa_t*)lwmac_hdr)->dst_addr;
    } else {
        if(netif_hdr->dst_l2addr_len) {
            info->dst_addr.len = netif_hdr->dst_l2addr_len;
            memcpy(info->dst_addr.addr,
                   gnrc_netif_hdr_get_dst_addr(netif_hdr),
                   netif_hdr->dst_l2addr_len);
        }
    }

    if(netif_hdr->src_l2addr_len) {
        info->src_addr.len = netif_hdr->src_l2addr_len;
        memcpy(info->src_addr.addr,
               gnrc_netif_hdr_get_src_addr(netif_hdr),
               netif_hdr->src_l2addr_len);
    }

    info->header = lwmac_hdr;

    return 0;
}

/******************************************************************************/

// TODO: Don't use global variables
void _set_netdev_state(lwmac_t* lwmac, netopt_state_t devstate)
{
    lwmac->netdev->driver->set(lwmac->netdev,
                               NETOPT_STATE,
                               &devstate,
                               sizeof(devstate));
}

/******************************************************************************/

netopt_state_t _get_netdev_state(lwmac_t* lwmac)
{
    netopt_state_t state;
    if (0 < lwmac->netdev->driver->get(lwmac->netdev,
                                       NETOPT_STATE,
                                       &state,
                                       sizeof(state))) {
        return state;
    }
    return -1;
}

/******************************************************************************/

/* Parameters in rtt timer ticks */
uint32_t _next_inphase_event(uint32_t last, uint32_t interval)
{
    /* Counter did overflow since last wakeup */
    if(rtt_get_counter() < last)
    {
        /* TODO: Not sure if this was tested :) */
        uint32_t tmp = -last;
        tmp /= interval;
        tmp++;
        last += tmp * interval;
    }

    /* Add margin to next wakeup so that it will be at least 2ms in the future */
    while(last < (rtt_get_counter() + LWMAC_RTT_EVENT_MARGIN_TICKS))
    {
        last += interval;
    }

    return last;
}

/******************************************************************************/

void lwmac_print_hdr(lwmac_hdr_t* hdr)
{
    assert(hdr != NULL);

    printf("LwMAC header:\n  Type: ");
    switch(hdr->type) {
    case FRAMETYPE_WR:
        puts("Wakeup request (WR)");
        break;
    case FRAMETYPE_WA:
    {
        puts("Wakeup acknowledge (WA)");
        printf("  Src addr:");
        lwmac_frame_wa_t* wa = (lwmac_frame_wa_t*) hdr;
        for(int i = 0; i < wa->dst_addr.len; i++) {
            printf("0x%02x", wa->dst_addr.addr[i]);
            if(i < (wa->dst_addr.len - 1)) {
                printf(":");
            }
        }
        break;
    }
    case FRAMETYPE_DATA:
        puts("User data");
        break;
    case FRAMETYPE_BROADCAST:
        puts("Broadcast user data");
        printf("  Sequence number: %d\n", ((lwmac_frame_broadcast_t*)hdr)->seq_nr);
        break;
    default:
        puts("Unkown type");
        printf("  Raw:  0x%02x\n", hdr->type);
    }
}

/******************************************************************************/

int _dispatch_defer(gnrc_pktsnip_t* buffer[], gnrc_pktsnip_t* pkt)
{
    assert(buffer != NULL);
    assert(pkt != NULL);

    /* We care about speed here, so assume packet structure */
    assert(pkt->next->type == GNRC_NETTYPE_LWMAC);
    assert(pkt->next->next->type == GNRC_NETTYPE_NETIF);

    lwmac_frame_broadcast_t* bcast = NULL;
    if(((lwmac_hdr_t*)pkt->next->data)->type == FRAMETYPE_BROADCAST) {
        bcast = pkt->next->data;
    }

    for(unsigned i = 0; i < LWMAC_DISPATCH_BUFFER_SIZE; i++) {
        /* Buffer will be filled bottom-up and emptied completely so no holes */
        if(buffer[i] == NULL) {
            buffer[i] = pkt;
            return 0;
        } else {
            /* Filter same broadcasts, compare sequence number */
            if(bcast &&
               (((lwmac_hdr_t*)buffer[i]->next->data)->type == FRAMETYPE_BROADCAST) &&
               (bcast->seq_nr == ((lwmac_frame_broadcast_t*)buffer[i]->next->data)->seq_nr))
            {
                gnrc_netif_hdr_t *hdr_queued, *hdr_new;
                hdr_new = pkt->next->next->data;
                hdr_queued = buffer[i]->next->next->data;

                /* Sequence numbers match, compare source addresses */
                if(hdr_new->src_l2addr_len == hdr_queued->src_l2addr_len) {
                    if(memcmp(gnrc_netif_hdr_get_src_addr(hdr_new),
                              gnrc_netif_hdr_get_src_addr(hdr_queued),
                              hdr_new->src_l2addr_len) == 0) {
                        /* Source addresses match, same packet */
                        DEBUG("[lwmac] Found duplicate broadcast packet, dropping\n");
                        gnrc_pktbuf_release(pkt);
                        return -2;
                    }
                }
            }
        }
    }

    DEBUG("[lwmac] Dispatch buffer full, dropping packet\n");
    gnrc_pktbuf_release(pkt);

    return -1;
}

/******************************************************************************/

void _dispatch(gnrc_pktsnip_t* buffer[])
{
    assert(buffer != NULL);

    for(unsigned i = 0; i < LWMAC_DISPATCH_BUFFER_SIZE; i++) {
        if(buffer[i]) {
            if (!gnrc_netapi_dispatch_receive(buffer[i]->type, GNRC_NETREG_DEMUX_CTX_ALL, buffer[i])) {
                DEBUG("Unable to forward packet of type %i\n", buffer[i]->type);
                gnrc_pktbuf_release(buffer[i]);
            }
            buffer[i] = NULL;
        }
    }
}
