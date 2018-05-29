/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <assert.h>

#include <sys/uio.h>

#include "net/netdev.h"
#include "net/gnrc.h"
#include "cc110x.h"
#include "cc110x-netdev.h"
#include "net/gnrc/netif.h"
#include "od.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

static int _send(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt)
{
    cc110x_pkt_t cc110x_pkt;
    netdev_t *dev = netif->dev;
    netdev_cc110x_t *netdev_cc110x = (netdev_cc110x_t *) dev;
    cc110x_t *cc110x = &netdev_cc110x->cc110x;

    assert(pkt != NULL);
    assert(dev->driver == &netdev_cc110x_driver);

    gnrc_netif_hdr_t *netif_hdr;
    gnrc_pktsnip_t *payload;

    payload = pkt->next;

    if (pkt->type != GNRC_NETTYPE_NETIF) {
        DEBUG("gnrc_cc110x: First header was not generic netif header\n");
        gnrc_pktbuf_release(pkt);
        return -EBADMSG;
    }

    netif_hdr = (gnrc_netif_hdr_t *) pkt->data;

    /* set up header */
    if (netif_hdr->src_l2addr_len == 1) {
        uint8_t *_src_addr = gnrc_netif_hdr_get_src_addr(netif_hdr);
        cc110x_pkt.phy_src = *_src_addr;
    }
    else {
        cc110x_pkt.phy_src = cc110x->radio_address;
    }

    if (netif_hdr->flags & (GNRC_NETIF_HDR_FLAGS_BROADCAST |
                GNRC_NETIF_HDR_FLAGS_MULTICAST)) {
        cc110x_pkt.address = 0;
    }
    else {
        uint8_t *_dst_addr = gnrc_netif_hdr_get_dst_addr(netif_hdr);
        cc110x_pkt.address = _dst_addr[netif_hdr->dst_l2addr_len-1];
    }

    switch (payload->type) {
#ifdef MODULE_GNRC_SIXLOWPAN
        case GNRC_NETTYPE_SIXLOWPAN:
            cc110x_pkt.flags = 1;
            break;
#endif
        default:
            cc110x_pkt.flags = 0;
    }

    iolist_t iolist = {
        .iol_base = (char *)&cc110x_pkt,
        .iol_len = sizeof(cc110x_pkt_t)
    };

    unsigned payload_len = 0;
    uint8_t *pos = cc110x_pkt.data;

    while (payload) {
        payload_len += payload->size;

        if (payload_len > CC110X_MAX_DATA_LENGTH) {
            DEBUG("gnrc_cc110x: payload length exceeds maximum"
                    "(%u>%u)\n", payload_len, CC110X_MAX_DATA_LENGTH);
            gnrc_pktbuf_release(pkt);
            return -EBADMSG;
        }

        memcpy(pos, payload->data, payload->size);
        pos += payload->size;
        payload = payload->next;
    }

    /* pkt has been copied into cc110x_pkt, we're done with it. */
    gnrc_pktbuf_release(pkt);

    cc110x_pkt.length = (uint8_t) payload_len + CC110X_HEADER_LENGTH;

    DEBUG("gnrc_cc110x: sending packet from %02x to %02x with payload "
            "length %u\n",
            (unsigned)cc110x_pkt.phy_src,
            (unsigned)cc110x_pkt.address,
            (unsigned)payload_len);
#if defined(MODULE_OD) && ENABLE_DEBUG
    od_hex_dump(cc110x_pkt.data, payload_len, OD_WIDTH_DEFAULT);
#endif

    return dev->driver->send(dev, &iolist);
}

static gnrc_pktsnip_t *_recv(gnrc_netif_t *netif)
{
    netdev_t *dev = netif->dev;
    cc110x_t *cc110x = &((netdev_cc110x_t*) dev)->cc110x;

    cc110x_pkt_t *cc110x_pkt = &cc110x->pkt_buf.packet;

    int payload_length = cc110x_pkt->length - CC110X_HEADER_LENGTH;

    int nettype;

    int addr_len;
    switch (cc110x_pkt->flags) {
#ifdef MODULE_GNRC_SIXLOWPAN
        case 1:
            addr_len = 8;
            nettype = GNRC_NETTYPE_SIXLOWPAN;
            break;
#endif
        default:
            addr_len = 1;
            nettype = GNRC_NETTYPE_UNDEF;
    }

    /* copy packet payload into pktbuf */
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, cc110x_pkt->data,
            payload_length, nettype);

    if(!pkt) {
        DEBUG("gnrc_cc110x: _recv: cannot allocate pktsnip.\n");
        return NULL;
    }


    gnrc_pktsnip_t *netif_hdr;
    netif_hdr = gnrc_pktbuf_add(NULL, NULL,
            sizeof(gnrc_netif_hdr_t) + 2*addr_len,
            GNRC_NETTYPE_NETIF);

    if (netif_hdr == NULL) {
        DEBUG("gnrc_cc110x: no space left in packet buffer\n");
        gnrc_pktbuf_release(pkt);
        return NULL;
    }

    gnrc_netif_hdr_init(netif_hdr->data, addr_len, addr_len);
    if (addr_len == 8) {
        uint64_t src_addr = cc110x_pkt->phy_src;
        uint64_t dst_addr = cc110x_pkt->address;
        gnrc_netif_hdr_set_src_addr(netif_hdr->data, (uint8_t*)&src_addr, addr_len);
        gnrc_netif_hdr_set_dst_addr(netif_hdr->data, (uint8_t*)&dst_addr, addr_len);
    }
    else {
        gnrc_netif_hdr_set_src_addr(netif_hdr->data, (uint8_t*)&cc110x_pkt->phy_src, addr_len);
        gnrc_netif_hdr_set_dst_addr(netif_hdr->data, (uint8_t*)&cc110x_pkt->address, addr_len);
    }

    ((gnrc_netif_hdr_t *)netif_hdr->data)->if_pid = thread_getpid();
    ((gnrc_netif_hdr_t *)netif_hdr->data)->lqi = cc110x->pkt_buf.lqi;
    ((gnrc_netif_hdr_t *)netif_hdr->data)->rssi = cc110x->pkt_buf.rssi;

    DEBUG("gnrc_cc110x: received packet from %02x of length %u\n",
            (unsigned)cc110x_pkt->phy_src,
            (unsigned)cc110x_pkt->length-CC110X_HEADER_LENGTH);
#if defined(MODULE_OD) && ENABLE_DEBUG
    od_hex_dump(cc110x_pkt->data, payload_length, OD_WIDTH_DEFAULT);
#endif


    pkt->next = netif_hdr;

    return pkt;
}

static const gnrc_netif_ops_t _cc110x_ops = {
    .send = _send,
    .recv = _recv,
    .get = gnrc_netif_get_from_netdev,
    .set = gnrc_netif_set_from_netdev,
};

gnrc_netif_t *gnrc_netif_cc110x_create(char *stack, int stacksize, char priority,
                                       char *name, netdev_t *dev)
{
    return gnrc_netif_create(stack, stacksize, priority, name, dev,
                             &_cc110x_ops);
}
