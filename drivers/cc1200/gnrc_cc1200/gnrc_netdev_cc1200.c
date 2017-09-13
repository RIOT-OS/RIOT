/*
 * Copyright (C) 2017 TU Berlin DAI Labor
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <assert.h>

#include <sys/uio.h>

#include "net/netdev.h"
#include "net/gnrc.h"
#include "cc1200.h"
#include "cc1200-netdev.h"
#include "net/gnrc/netdev.h"
#include "od.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

static int _send(gnrc_netdev_t *gnrc_netdev, gnrc_pktsnip_t *pkt)
{
    DEBUG("%s:%u\n", __func__, __LINE__);
    cc1200_pkt_t cc1200_pkt;
    netdev_t *dev = gnrc_netdev->dev;
    netdev_cc1200_t *netdev_cc1200 = (netdev_cc1200_t *) dev;
    cc1200_t *cc1200 = &netdev_cc1200->cc1200;

    assert(pkt != NULL);
    assert(dev->driver == &netdev_cc1200_driver);

    gnrc_netif_hdr_t *netif_hdr;
    gnrc_pktsnip_t *payload;

    payload = pkt->next;

    if (pkt->type != GNRC_NETTYPE_NETIF) {
        DEBUG("gnrc_netdev_cc1200: First header was not generic netif header\n");
        gnrc_pktbuf_release(pkt);
        return -EBADMSG;
    }

    netif_hdr = (gnrc_netif_hdr_t *) pkt->data;

    /* set up header */
    if (netif_hdr->src_l2addr_len == 1) {
        uint8_t *_src_addr = gnrc_netif_hdr_get_src_addr(netif_hdr);
        cc1200_pkt.phy_src = *_src_addr;
    }
    else {
        cc1200_pkt.phy_src = cc1200->radio_address;
    }

    if (netif_hdr->flags & (GNRC_NETIF_HDR_FLAGS_BROADCAST |
                GNRC_NETIF_HDR_FLAGS_MULTICAST)) {
        cc1200_pkt.address = 0;
    }
    else {
        uint8_t *_dst_addr = gnrc_netif_hdr_get_dst_addr(netif_hdr);
        cc1200_pkt.address = _dst_addr[netif_hdr->dst_l2addr_len-1];
    }

    switch (payload->type) {
#ifdef MODULE_GNRC_SIXLOWPAN
        case GNRC_NETTYPE_SIXLOWPAN:
            cc1200_pkt.flags = 1;
            break;
#endif
        default:
            cc1200_pkt.flags = 0;
    }
    gnrc_pktsnip_t* tmp = pkt;
    while(tmp){
        DEBUG("%s:%u gnrc_netdev payload:\n", __func__, __LINE__);
        for(int i = 0; i < tmp->size; i++){
            DEBUG("0x%x ", *((char*)(tmp->data)+i));
        }
        tmp = tmp->next;
    }
    DEBUG("%s:%u payload type: 0x%x\n", __func__, __LINE__, payload->type);
    struct iovec vector;
    vector.iov_base = (char*)&cc1200_pkt;
    vector.iov_len = sizeof(cc1200_pkt_t);
    DEBUG("%s:%u vector.iov_len: 0x%x\n", __func__, __LINE__, vector.iov_len);

    unsigned payload_len = 0;
    uint8_t *pos = cc1200_pkt.data;

    while (payload) {
        payload_len += payload->size;

        if (payload_len > CC1200_MAX_DATA_LENGTH) {
            DEBUG("gnrc_netdev_cc1200: payload length exceeds maximum"
                    "(%u>%u)\n", payload_len, CC1200_MAX_DATA_LENGTH);
            gnrc_pktbuf_release(pkt);
            return -EBADMSG;
        }

        memcpy(pos, payload->data, payload->size);
        pos += payload->size;
        payload = payload->next;
    }
    DEBUG("%s:%u payload_len: 0x%x\n", __func__, __LINE__, payload_len);

    /* pkt has been copied into iovec, we're done with it. */
    gnrc_pktbuf_release(pkt);

    cc1200_pkt.length = (uint8_t) payload_len + CC1200_HEADER_LENGTH;

    DEBUG("gnrc_netdev_cc1200: sending packet from %u to %u with payload "
            "length %u\n",
            (unsigned)cc1200_pkt.phy_src,
            (unsigned)cc1200_pkt.address,
            (unsigned)cc1200_pkt.length);

    return dev->driver->send(dev, &vector, 1);
}

static gnrc_pktsnip_t *_recv(gnrc_netdev_t *gnrc_netdev)
{
    DEBUG("%s:%u\n", __func__, __LINE__);
    netdev_t *dev = gnrc_netdev->dev;
    cc1200_t *cc1200 = &((netdev_cc1200_t*) dev)->cc1200;

    cc1200_pkt_t *cc1200_pkt = &cc1200->pkt_buf.packet;

    int payload_length = cc1200_pkt->length - CC1200_HEADER_LENGTH;

    int nettype;

    int addr_len;
    switch (cc1200_pkt->flags) {
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
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, cc1200_pkt->data,
            payload_length, nettype);

    if(!pkt) {
        DEBUG("cc1200: _recv: cannot allocate pktsnip.\n");
        return NULL;
    }


    gnrc_pktsnip_t *netif_hdr;
    netif_hdr = gnrc_pktbuf_add(NULL, NULL,
            sizeof(gnrc_netif_hdr_t) + 2*addr_len,
            GNRC_NETTYPE_NETIF);

    if (netif_hdr == NULL) {
        DEBUG("gnrc_netdev_cc1200: no space left in packet buffer\n");
        gnrc_pktbuf_release(pkt);
        return NULL;
    }

    gnrc_netif_hdr_init(netif_hdr->data, addr_len, addr_len);
    if (addr_len == 8) {
        uint64_t src_addr = cc1200_pkt->phy_src;
        uint64_t dst_addr = cc1200_pkt->address;
        gnrc_netif_hdr_set_src_addr(netif_hdr->data, (uint8_t*)&src_addr, addr_len);
        gnrc_netif_hdr_set_dst_addr(netif_hdr->data, (uint8_t*)&dst_addr, addr_len);
    }
    else {
        gnrc_netif_hdr_set_src_addr(netif_hdr->data, (uint8_t*)&cc1200_pkt->phy_src, addr_len);
        gnrc_netif_hdr_set_dst_addr(netif_hdr->data, (uint8_t*)&cc1200_pkt->address, addr_len);
    }

    ((gnrc_netif_hdr_t *)netif_hdr->data)->if_pid = thread_getpid();
    ((gnrc_netif_hdr_t *)netif_hdr->data)->lqi = cc1200->pkt_buf.lqi;
    ((gnrc_netif_hdr_t *)netif_hdr->data)->rssi = cc1200->pkt_buf.rssi;

    DEBUG("gnrc_netdev_cc1200: received packet from %02x"
            " of length %u\n",
            (unsigned)cc1200_pkt->phy_src,
            (unsigned)cc1200_pkt->length-CC1200_HEADER_LENGTH);
#if defined(MODULE_OD) && ENABLE_DEBUG
    od_hex_dump(cc1200_pkt->data, payload_length, OD_WIDTH_DEFAULT);
#endif


    pkt->next = netif_hdr;

    return pkt;
}

int gnrc_netdev_cc1200_init(gnrc_netdev_t *gnrc_netdev, netdev_t *dev)
{
    gnrc_netdev->send = _send;
    gnrc_netdev->recv = _recv;
    gnrc_netdev->dev = dev;

    return 0;
}
