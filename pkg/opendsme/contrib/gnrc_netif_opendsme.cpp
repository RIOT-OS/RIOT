/*
 * Copyright (C) 2022 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  José I. Álamos <jose.alamos@haw-hamburg.de>
 */

#include "opendsme/DSMEPlatform.h"
#include "opendsme/opendsme.h"
#include "mac_services/DSME_Common.h"
#include "net/gnrc/netif/hdr.h"

dsme::DSMEPlatform m_dsme;

extern "C" {
static bool _pan_coord;
extern void heap_stats(void);
static int _send(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt)
{
    /* Note that there the short address is assigned by the coordinator and the
     * user should not have control over it.
     * Given that, and the fact the current MAC does not communicate with
     * different PANs, we can always use the short address */
    uint8_t bcast[2] = { 0xFF, 0xFF };
    uint8_t *addr;

    pkt = gnrc_pktbuf_start_write(pkt);
    gnrc_netif_hdr_t *hdr = (gnrc_netif_hdr_t *)pkt->data;

    if (hdr->flags &= GNRC_NETIF_HDR_FLAGS_MULTICAST) {
        addr = static_cast<uint8_t *>(&bcast[0]);
    }
    else if (hdr->dst_l2addr_len == IEEE802154_LONG_ADDRESS_LEN) {
        addr = gnrc_netif_hdr_get_dst_addr(hdr)
               + IEEE802154_LONG_ADDRESS_LEN
               - IEEE802154_SHORT_ADDRESS_LEN;
    }
    else if (hdr->dst_l2addr_len == IEEE802154_SHORT_ADDRESS_LEN) {
        addr = gnrc_netif_hdr_get_dst_addr(hdr);
    }
    else {
        gnrc_pktbuf_release(pkt);
        return -EBADMSG;
    }

    uint16_t _addr = byteorder_ntohs(*((network_uint16_t *)addr));

    pkt = gnrc_pktbuf_remove_snip(pkt, pkt);
    m_dsme.sendFrame(_addr, (iolist_t *)pkt);

    return 0;
}

static gnrc_pktsnip_t *_recv(gnrc_netif_t *netif)
{
    /* Not used */
    return NULL;
}

static int _get(gnrc_netif_t *netif, gnrc_netapi_opt_t *opt)
{
    gnrc_netif_acquire(netif);
    int res;
    network_uint16_t addr;
    le_uint64_t ext_addr;
    uint8_t *addr_ptr = static_cast<uint8_t *>(ext_addr.u8);

    switch (opt->opt) {
    case NETOPT_MAX_PDU_SIZE:
        *((uint16_t *)opt->data) = IEEE802154_FRAME_LEN_MAX;
        res = sizeof(uint16_t);
        break;
    case NETOPT_ADDR_LEN:
        *((uint16_t *)opt->data) = IEEE802154_SHORT_ADDRESS_LEN;
        res = sizeof(uint16_t);
        break;
    case NETOPT_ADDRESS:
        assert(opt->data_len >= IEEE802154_SHORT_ADDRESS_LEN);
        m_dsme.getShortAddress(&addr);
        memcpy(opt->data, &addr, IEEE802154_SHORT_ADDRESS_LEN);
        res = IEEE802154_SHORT_ADDRESS_LEN;
        break;
    case NETOPT_ADDRESS_LONG: {
        assert(opt->data_len >= IEEE802154_LONG_ADDRESS_LEN);
        addr_ptr << m_dsme.getAddress();
        *((be_uint64_t *)opt->data) = byteorder_ltobll(ext_addr);
        res = IEEE802154_LONG_ADDRESS_LEN;
        break;
    }
    case NETOPT_LINK:
        assert(opt->data_len >= sizeof(netopt_enable_t));
        *((netopt_enable_t *)opt->data) = m_dsme.isAssociated()
                                          ? NETOPT_ENABLE
                                          : NETOPT_DISABLE;
        return sizeof(netopt_enable_t);
    default:
        res = -ENOTSUP;
        break;
    }

    return res;
}

static int _set(gnrc_netif_t *netif, const gnrc_netapi_opt_t *opt)
{
    int res;

    switch (opt->opt) {
    case NETOPT_LINK:
        m_dsme.initialize(_pan_coord);
        m_dsme.start();
        assert(*(netopt_enable_t *)opt->data == NETOPT_ENABLE);
        res = sizeof(netopt_enable_t);
        break;
    case NETOPT_PAN_COORD:
        if (*((bool *)opt->data) == true) {
            _pan_coord = true;
        }
        else {
            _pan_coord = false;
        }
        res = sizeof(netopt_enable_t);
        break;
#if IS_ACTIVE(CONFIG_IEEE802154_DSME_STATIC_GTS)
    case NETOPT_GTS_ALLOC: {
        ieee802154_dsme_alloc_t *alloc = (ieee802154_dsme_alloc_t *)opt->data;
        uint16_t _addr = byteorder_ntohs(alloc->addr);
        m_dsme.allocateGTS(alloc->superframe_id, alloc->slot_id, alloc->channel_id,
                           alloc->tx ? dsme::Direction::TX : dsme::Direction::RX, _addr);
        res = sizeof(ieee802154_dsme_alloc_t);
        break;
    }
#endif
    case NETOPT_PROTO:
        assert(opt->data_len == sizeof(gnrc_nettype_t));
        res = sizeof(gnrc_nettype_t);
        break;
    case NETOPT_GTS_TX:
        assert(opt->data_len == sizeof(netopt_enable_t));
        m_dsme.setGTSTransmission(*((netopt_enable_t *)opt->data) ? true : false);
        res = sizeof(netopt_enable_t);
        break;
    case NETOPT_ACK_REQ:
        assert(opt->data_len == sizeof(netopt_enable_t));
        m_dsme.setAckReq(*((netopt_enable_t *)opt->data) ? true : false);
        res = sizeof(netopt_enable_t);
        break;
    case NETOPT_SRC_LEN:
        res = sizeof(uint16_t);
        break;
    default:
        res = -ENOTSUP;
        break;
    }
    return res;
}

static int _init(gnrc_netif_t *netif)
{
    netif->flags = 0;
    netif_register(&netif->netif);
    return 0;
}

static const gnrc_netif_ops_t dsme_ops = {
    .init = _init,
    .send = _send,
    .recv = _recv,
    .get = _get,
    .set = _set,
};

int gnrc_netif_opendsme_create(gnrc_netif_t *netif, char *stack, int stacksize,
                               char priority, const char *name, netdev_t *dev)
{
    m_dsme.setGNRCNetif(netif);
    return gnrc_netif_create(netif, stack, stacksize, priority, name, dev,
                             &dsme_ops);
}
}

/** @} */
