/*
 * Copyright (C) 2015 Daniel Krebs
 *               2016 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc_mac
 * @{
 *
 * @file
 * @brief       Implementation of internal functions of GNRC_MAC
 *
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @author      Shuguo Zhuo  <shuguo.zhuo@inria.fr>
 * @}
 */

#include <assert.h>
#include <stdbool.h>

#include "net/gnrc.h"
#include "net/gnrc/mac/internal.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#if ((GNRC_MAC_TX_QUEUE_SIZE != 0) || (GNRC_MAC_RX_QUEUE_SIZE != 0))
gnrc_priority_pktqueue_node_t *_alloc_pktqueue_node(gnrc_priority_pktqueue_node_t *nodes,
                                                    uint32_t size)
{
    assert(nodes != NULL);
    assert(size > 0);

    /* search for free packet_queue_node */
    for (size_t i = 0; i < size; i++) {
        if ((nodes[i].pkt == NULL) &&
            (nodes[i].next == NULL)) {
            return &nodes[i];
        }
    }

    return NULL;
}
#endif /* ((GNRC_MAC_TX_QUEUE_SIZE != 0) || (GNRC_MAC_RX_QUEUE_SIZE != 0)) */

#if GNRC_MAC_TX_QUEUE_SIZE != 0
#if CONFIG_GNRC_MAC_NEIGHBOR_COUNT != 0
/* Find the neighbor's id based on the given address */
int _gnrc_mac_find_neighbor(gnrc_mac_tx_t *tx, const uint8_t *dst_addr, int addr_len)
{
    assert(tx != NULL);
    assert(dst_addr != NULL);
    assert(addr_len > 0);

    gnrc_mac_tx_neighbor_t *neighbors;
    neighbors = tx->neighbors;

    /* Don't attempt to find broadcast neighbor, so start at index 1 */
    for (int i = 1; i <= (signed)CONFIG_GNRC_MAC_NEIGHBOR_COUNT; i++) {
        if (neighbors[i].l2_addr_len == addr_len) {
            if (memcmp(&(neighbors[i].l2_addr), dst_addr, addr_len) == 0) {
                return i;
            }
        }
    }
    return -ENOENT;
}

/* Free first empty queue (neighbor) that is not active */
int _gnrc_mac_free_neighbor(gnrc_mac_tx_t *tx)
{
    assert(tx != NULL);

    gnrc_mac_tx_neighbor_t *neighbors;
    neighbors = tx->neighbors;

    /* Don't attempt to free broadcast neighbor, so start at index 1 */
    for (int i = 1; i <= (signed)CONFIG_GNRC_MAC_NEIGHBOR_COUNT; i++) {
        if ((gnrc_priority_pktqueue_length(&(neighbors[i].queue)) == 0) &&
            (&neighbors[i] != tx->current_neighbor)) {
            /* Mark as free */
            neighbors[i].l2_addr_len = 0;
            return i;
        }
    }
    return -ENOSPC;
}

/* Allocate first unused queue (neighbor) */
int _gnrc_mac_alloc_neighbor(gnrc_mac_tx_t *tx)
{
    assert(tx != NULL);

    gnrc_mac_tx_neighbor_t *neighbors;
    neighbors = tx->neighbors;

    /* Don't attempt to allocate broadcast neighbor, so start at index 1 */
    for (int i = 1; i <= (signed)CONFIG_GNRC_MAC_NEIGHBOR_COUNT; i++) {
        if (neighbors[i].l2_addr_len == 0) {
            gnrc_priority_pktqueue_init(&(neighbors[i].queue));
            return i;
        }
    }
    return -ENOSPC;
}

/* Initialize the neighbor */
void _gnrc_mac_init_neighbor(gnrc_mac_tx_neighbor_t *neighbor, const uint8_t *addr, int len)
{
    assert(neighbor != NULL);
    assert(addr != NULL);
    assert(len > 0);

    neighbor->l2_addr_len = len;
    neighbor->phase = GNRC_MAC_PHASE_MAX;
    memcpy(&(neighbor->l2_addr), addr, len);
}
#endif /* CONFIG_GNRC_MAC_NEIGHBOR_COUNT != 0 */

bool gnrc_mac_queue_tx_packet(gnrc_mac_tx_t *tx, uint32_t priority, gnrc_pktsnip_t *pkt)
{
    assert(tx != NULL);
    assert(pkt != NULL);

#if CONFIG_GNRC_MAC_NEIGHBOR_COUNT == 0

    gnrc_priority_pktqueue_node_t *node;
    node = _alloc_pktqueue_node(tx->_queue_nodes, GNRC_MAC_TX_QUEUE_SIZE);

    if (node) {
        gnrc_priority_pktqueue_node_init(node, priority, pkt);
        gnrc_priority_pktqueue_push(&tx->queue, node);
        return true;
    }

    DEBUG("[gnrc_mac-int] Can't push to TX queue, no entries left\n");
    return false;

#else

    gnrc_mac_tx_neighbor_t *neighbor;
    int neighbor_id;

    /* Check whether the packet it for broadcast or multicast */
    if (gnrc_netif_hdr_get_flag(pkt) &
        (GNRC_NETIF_HDR_FLAGS_MULTICAST | GNRC_NETIF_HDR_FLAGS_BROADCAST)) {
        /* Broadcast/multicast queue is neighbor 0 by definition */
        neighbor_id = 0;
        neighbor = &tx->neighbors[neighbor_id];

    }
    else {
        uint8_t *addr;
        int addr_len;
        bool neighbor_known = true;

        /* Get destination address of packet */
        addr_len = gnrc_netif_hdr_get_dstaddr(pkt, &addr);
        if (addr_len <= 0) {
            DEBUG("[gnrc_mac-int] Packet has no destination address\n");
            return false;
        }

        /* Search for existing queue for destination */
        neighbor_id = _gnrc_mac_find_neighbor(tx, addr, addr_len);

        /* neighbor node doesn't have a queue yet */
        if (neighbor_id < 0) {
            neighbor_known = false;

            /* Try to allocate neighbor entry */
            neighbor_id = _gnrc_mac_alloc_neighbor(tx);

            /* No neighbor entries left */
            if (neighbor_id < 0) {
                DEBUG("[gnrc_mac-int] No neighbor entries left, maybe increase "
                      "CONFIG_GNRC_MAC_NEIGHBOR_COUNT for better performance\n");

                /* Try to free an unused queue */
                neighbor_id = _gnrc_mac_free_neighbor(tx);

                /* All queues are in use, so reject */
                if (neighbor_id < 0) {
                    DEBUG("[gnrc_mac-int] Couldn't allocate tx queue for packet\n");
                    return false;
                }
            }
        }

        neighbor = &tx->neighbors[neighbor_id];

        if (!neighbor_known) {
            _gnrc_mac_init_neighbor(neighbor, addr, addr_len);
        }
    }

    gnrc_priority_pktqueue_node_t *node;
    node = _alloc_pktqueue_node(tx->_queue_nodes, GNRC_MAC_TX_QUEUE_SIZE);
    if (node) {
        gnrc_priority_pktqueue_node_init(node, priority, pkt);
        gnrc_priority_pktqueue_push(&neighbor->queue, node);
        DEBUG("[gnrc_mac-int] Queuing pkt to neighbor #%d\n", neighbor_id);
        return true;
    }

    DEBUG("[gnrc_mac-int] Can't push to neighbor #%d's queue, no entries left\n",
          neighbor_id);
    return false;

#endif  /* CONFIG_GNRC_MAC_NEIGHBOR_COUNT == 0 */
}
#endif  /* GNRC_MAC_TX_QUEUE_SIZE != 0 */

#if GNRC_MAC_RX_QUEUE_SIZE != 0
bool gnrc_mac_queue_rx_packet(gnrc_mac_rx_t *rx, uint32_t priority, gnrc_pktsnip_t *pkt)
{
    assert(rx != NULL);
    assert(pkt != NULL);

    gnrc_priority_pktqueue_node_t *node;
    node = _alloc_pktqueue_node(rx->_queue_nodes, GNRC_MAC_RX_QUEUE_SIZE);

    if (node) {
        gnrc_priority_pktqueue_node_init(node, priority, pkt);
        gnrc_priority_pktqueue_push(&rx->queue, node);
        return true;
    }

    DEBUG("[gnrc_mac] Can't push RX packet @ %p, no entries left\n", (void*)pkt);
    return false;
}
#endif /* GNRC_MAC_RX_QUEUE_SIZE != 0 */

#if GNRC_MAC_DISPATCH_BUFFER_SIZE != 0
void gnrc_mac_dispatch(gnrc_mac_rx_t *rx)
{
    assert(rx != NULL);

    for (unsigned i = 0; i < GNRC_MAC_DISPATCH_BUFFER_SIZE; i++) {
        if (rx->dispatch_buffer[i]) {
#ifdef MODULE_GNRC_LWMAC
            /* save pointer to netif header */
            gnrc_pktsnip_t *netif = rx->dispatch_buffer[i]->next->next;

            /* remove lwmac header */
            rx->dispatch_buffer[i]->next->next = NULL;
            gnrc_pktbuf_release(rx->dispatch_buffer[i]->next);

            /* make append netif header after payload again */
            rx->dispatch_buffer[i]->next = netif;
#endif

            if (!gnrc_netapi_dispatch_receive(rx->dispatch_buffer[i]->type,
                                              GNRC_NETREG_DEMUX_CTX_ALL,
                                              rx->dispatch_buffer[i])) {
                DEBUG("Unable to forward packet of type %i\n", rx->dispatch_buffer[i]->type);
                gnrc_pktbuf_release(rx->dispatch_buffer[i]);
            }
            rx->dispatch_buffer[i] = NULL;
        }
    }
}
#endif /* GNRC_MAC_DISPATCH_BUFFER_SIZE != 0 */
