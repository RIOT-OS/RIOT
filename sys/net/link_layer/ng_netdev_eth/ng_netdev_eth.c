/*
 * Copyright (C) 2015 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>,
 *                    Martine Lenders <mlenders@inf.fu-berlin.de>
 *                    Kaspar Schleiser <kaspar@schleiser.de>
 *
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @ingroup     sys_net_link_layer
 * @{
 *
 * @file
 *
 * @author      Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "byteorder.h"
#include "net/eui64.h"
#include "net/ng_ethernet.h"
#include "net/ng_ethertype.h"
#include "net/ng_netdev.h"
#include "net/ng_netif/hdr.h"
#include "net/ng_pkt.h"
#include "net/ng_pktbuf.h"
#include "net/ng_netdev_eth.h"
#include "net/dev_eth.h"
#include "od.h"
#include "thread.h"
#include "utlist.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

ng_netdev_eth_t ng_netdev_eth;

static uint8_t send_buffer[NG_ETHERNET_MAX_LEN];
static uint8_t recv_buffer[NG_ETHERNET_MAX_LEN];

#define _ISR_EVENT_RX (1U)

/* driver function definitions */
static int _send_data(ng_netdev_t *netdev, ng_pktsnip_t *pkt);
static int _add_event_callback(ng_netdev_t *dev, ng_netdev_event_cb_t cb);
static int _rem_event_callback(ng_netdev_t *dev, ng_netdev_event_cb_t cb);
static int _get(ng_netdev_t *dev, netopt_t opt, void *value, size_t max_len);
static int _set(ng_netdev_t *dev, netopt_t opt, void *value, size_t value_len);
static void _isr_event(ng_netdev_t *dev, uint32_t event_type);

/* netdev driver struct */
const ng_netdev_driver_t ng_netdev_eth_driver = {
    _send_data,
    _add_event_callback,
    _rem_event_callback,
    _get,
    _set,
    _isr_event,
};

/* These functions are not used */
#if !(defined(__FreeBSD__) || defined(__MACH__))
/* internal function definitions */
static inline bool _is_addr_broadcast(uint8_t *addr)
{
    return ((addr[0] == 0xff) && (addr[1] == 0xff) && (addr[2] == 0xff) &&
            (addr[3] == 0xff) && (addr[4] == 0xff) && (addr[5] == 0xff));
}

static inline bool _is_addr_multicast(uint8_t *addr)
{
    /* source: http://ieee802.org/secmail/pdfocSP2xXA6d.pdf */
    return (addr[0] & 0x01);
}
#endif

/* build Ethernet packet from pkt */
static int _marshall_ethernet(ng_netdev_eth_t *dev, uint8_t *buffer, ng_pktsnip_t *pkt);

/* build ISR handler for ISR events */
void _trigger_isr_event(void);

static int _send_data(ng_netdev_t *netdev, ng_pktsnip_t *pkt)
{
    int nsent, to_send;
    ng_netdev_eth_t *dev = (ng_netdev_eth_t *)netdev;

    DEBUG("ng_netdev_eth: send data ");

    if (pkt == NULL) {
        return -EFAULT;
    }

    if ((dev == NULL) || (netdev->driver != &ng_netdev_eth_driver)) {
        DEBUG("[wrong device descriptor]\n");
        ng_pktbuf_release(pkt);
        return -ENODEV;
    }

    DEBUG("\n");

    to_send = _marshall_ethernet(dev, send_buffer, pkt);
    ng_pktbuf_release(pkt);

    if (to_send < 0) {
        errno = -to_send;
        DEBUG("marshall\n");
        return to_send;
    }

    DEBUG("ng_netdev_eth: send %d bytes\n", to_send);
#if defined(MODULE_OD) && ENABLE_DEBUG
    od_hex_dump(send_buffer, to_send, OD_WIDTH_DEFAULT);
#endif

    dev_eth_t *ethdev = dev->ethdev;
    if ((nsent = ethdev->driver->send(ethdev, (char*)send_buffer, to_send)) < 0) {
        DEBUG("write\n");
        return -EIO;
    }

    return nsent;
}

static int _add_event_callback(ng_netdev_t *dev, ng_netdev_event_cb_t cb)
{
    DEBUG("ng_netdev_eth: add event callback");

    if ((dev == NULL) || (dev->driver != &ng_netdev_eth_driver)) {
        DEBUG(" [wrong device descriptor]\n");
        return -ENODEV;
    }

    if (dev->event_cb != NULL) {
        DEBUG(" [no space left]\n");
        return -ENOBUFS;
    }

    DEBUG("\n");
    dev->event_cb = cb;

    return 0;
}

static int _rem_event_callback(ng_netdev_t *dev, ng_netdev_event_cb_t cb)
{
    DEBUG("ng_netdev_eth: remove event callback");

    if ((dev == NULL) || (dev->driver != &ng_netdev_eth_driver)) {
        DEBUG(" [wrong device descriptor]\n");
        return -ENODEV;
    }

    if (dev->event_cb != cb) {
        DEBUG(" [not found]\n");
        return -ENOENT;
    }

    DEBUG("\n");
    dev->event_cb = NULL;

    return 0;
}

/* individual option getters to be called by _get() */
static inline int _get_addr(ng_netdev_eth_t *netdev, uint8_t *value, size_t max_len)
{
    if (max_len < NG_ETHERNET_ADDR_LEN) {
        /* value buffer not big enough */
        return -EOVERFLOW;
    }

    dev_eth_t *dev = netdev->ethdev;
    dev->driver->get_mac_addr(dev, value);

    return NG_ETHERNET_ADDR_LEN;
}

static inline int _get_addr_len(uint16_t *value, size_t max_len)
{
    if (max_len != sizeof(uint16_t)) {
        /* value buffer not big enough */
        return -EOVERFLOW;
    }

    *value = NG_ETHERNET_ADDR_LEN;

    return sizeof(uint16_t);
}

static inline int _get_iid(ng_netdev_eth_t *netdev, eui64_t *value, size_t max_len)
{
    if (max_len < sizeof(eui64_t)) {
        /* value buffer not big enough */
        return -EOVERFLOW;
    }

    dev_eth_t *dev = netdev->ethdev;
    uint8_t addr[NG_ETHERNET_ADDR_LEN];
    dev->driver->get_mac_addr(dev, addr);
    ng_ethernet_get_iid(value, addr);

    return sizeof(eui64_t);
}

static inline int _get_max_pkt_sz(uint16_t *value, size_t max_len)
{
    if (max_len != sizeof(uint16_t)) {
        /* value buffer not big enough */
        return -EOVERFLOW;
    }

    *value = NG_ETHERNET_MAX_LEN;

    return sizeof(uint16_t);
}

static inline int _get_promiscousmode(ng_netdev_eth_t *netdev, netopt_enable_t *value,
                                      size_t max_len)
{
    if (max_len != sizeof(netopt_enable_t)) {
        /* value buffer not big enough */
        return -EOVERFLOW;
    }


    dev_eth_t *dev = netdev->ethdev;
    *value = (netopt_enable_t)dev->driver->get_promiscous(dev);

    return sizeof(netopt_enable_t);
}

static int _get(ng_netdev_t *dev, netopt_t opt, void *value, size_t max_len)
{
    DEBUG("ng_netdev_eth: get ");

    if ((dev == NULL) || (dev->driver != &ng_netdev_eth_driver)) {
        DEBUG("[wrong device descriptor]\n");
        return -ENODEV;
    }

    switch (opt) {
        case NETOPT_ADDRESS:
            DEBUG("address\n");
            return _get_addr((ng_netdev_eth_t *)dev, value, max_len);

        case NETOPT_ADDR_LEN:
            DEBUG("address length\n");
            return _get_addr_len(value, max_len);

        case NETOPT_IPV6_IID:
            DEBUG("IPv6 IID\n");
            return _get_iid((ng_netdev_eth_t *)dev, value, max_len);

        case NETOPT_MAX_PACKET_SIZE:
            DEBUG("maximum packet size\n");
            return _get_max_pkt_sz(value, max_len);

        case NETOPT_PROMISCUOUSMODE:
            DEBUG("promiscous mode\n");
            return _get_promiscousmode((ng_netdev_eth_t *)dev, value, max_len);

        default:
            DEBUG("[not supported: %d]\n", opt);
            return -ENOTSUP;
    }
}

/* individual option getters to be called by _get() */
static inline int _set_promiscousmode(ng_netdev_eth_t *netdev, netopt_enable_t *value,
                                      size_t value_len)
{
    if (value_len != sizeof(netopt_enable_t)) {
        /* value buffer not big enough */
        return -EOVERFLOW;
    }

    dev_eth_t *dev = netdev->ethdev;

    dev->driver->set_promiscous(dev, (uint8_t)*value);

    return sizeof(netopt_enable_t);
}

static int _set(ng_netdev_t *dev, netopt_t opt, void *value, size_t value_len)
{
    DEBUG("ng_netdev_eth: set ");

    if ((dev == NULL) || (dev->driver != &ng_netdev_eth_driver)) {
        DEBUG("[wrong device descriptor]\n");
        return -ENODEV;
    }

    switch (opt) {
        case NETOPT_PROMISCUOUSMODE:
            DEBUG("promiscous mode\n");
            return _set_promiscousmode((ng_netdev_eth_t *)dev, value, value_len);

        default:
            DEBUG("[not supported: %d]\n", opt);
            return -ENOTSUP;
    }
}

/* individual event handlers called by _isr_event() */
static void _rx_event(ng_netdev_eth_t *dev);

static void _isr_event(ng_netdev_t *dev, uint32_t event_type)
{
    DEBUG("ng_netdev_eth: ISR event ");

    if ((dev == NULL) || (dev->driver != &ng_netdev_eth_driver)) {
        return;
    }

    ng_netdev_eth_t *netdev = (ng_netdev_eth_t*)dev;

    switch (event_type) {
        case _ISR_EVENT_RX:

            DEBUG("[ISR]\n");

#ifdef NETDEV_ETH_DELAY_SEND
            DEBUG("netdev_eth: delaying send...\n");
            volatile int i = NETDEV_ETH_DELAY_SEND;
            while(i--);
            DEBUG("netdev_eth: delay done.\n");
#endif

            dev_eth_t *ethdev = netdev->ethdev;
            ethdev->driver->isr(ethdev);
            break;

        default:
            DEBUG("[unknown event_type]\n");
            break;
    }
}

static inline void _addr_set_broadcast(uint8_t *dst)
{
    memset(dst, 0xff, NG_ETHERNET_ADDR_LEN);
}

#define _IPV6_DST_OFFSET    (36)    /* sizeof(ipv6_hdr_t) - 4  */

static inline void _addr_set_multicast(uint8_t *dst, ng_pktsnip_t *payload)
{
    switch (payload->type) {
#ifdef MODULE_NG_IPV6
        case NG_NETTYPE_IPV6:
            dst[0] = 0x33;
            dst[1] = 0x33;
            if ((payload != NULL) && (payload->data != NULL)) {
                memcpy(dst + 2, ((uint8_t *)payload->data) + _IPV6_DST_OFFSET, 4);
            }
            /* TODO change to proper types when ng_ipv6_hdr_t got merged */
            break;
#endif
        default:
            _addr_set_broadcast(dst);
            break;
    }
}

static int _marshall_ethernet(ng_netdev_eth_t *dev, uint8_t *buffer, ng_pktsnip_t *pkt)
{
    int data_len = 0;
    ng_ethernet_hdr_t *hdr = (ng_ethernet_hdr_t *)buffer;
    ng_netif_hdr_t *netif_hdr;
    ng_pktsnip_t *payload;

    if (pkt == NULL) {
        DEBUG("ng_netdev_eth: pkt was NULL");
        return -EINVAL;
    }

    payload = pkt->next;

    if (pkt->type != NG_NETTYPE_NETIF) {
        DEBUG("ng_netdev_eth: First header was not generic netif header\n");
        return -EBADMSG;
    }

    if (payload) {
        hdr->type = byteorder_htons(ng_nettype_to_ethertype(payload->type));
    }
    else {
        hdr->type = byteorder_htons(NG_ETHERTYPE_UNKNOWN);
    }

    netif_hdr = pkt->data;

    /* set ethernet header */
    if (netif_hdr->src_l2addr_len == NG_ETHERNET_ADDR_LEN) {
        memcpy(hdr->dst, ng_netif_hdr_get_src_addr(netif_hdr),
               netif_hdr->src_l2addr_len);
    }
    else {
        dev_eth_t *ethdev = dev->ethdev;
        ethdev->driver->get_mac_addr(ethdev, hdr->src);
    }

    if (netif_hdr->flags & NG_NETIF_HDR_FLAGS_BROADCAST) {
        _addr_set_broadcast(hdr->dst);
    }
    else if (netif_hdr->flags & NG_NETIF_HDR_FLAGS_MULTICAST) {
        _addr_set_multicast(hdr->dst, payload);
    }
    else if (netif_hdr->dst_l2addr_len == NG_ETHERNET_ADDR_LEN) {
        memcpy(hdr->dst, ng_netif_hdr_get_dst_addr(netif_hdr),
               NG_ETHERNET_ADDR_LEN);
    }
    else {
        DEBUG("ng_netdev_eth: destination address had unexpected format\n");
        return -EBADMSG;
    }

    DEBUG("ng_netdev_eth: send to %02x:%02x:%02x:%02x:%02x:%02x\n",
          hdr->dst[0], hdr->dst[1], hdr->dst[2],
          hdr->dst[3], hdr->dst[4], hdr->dst[5]);

    data_len += sizeof(ng_ethernet_hdr_t);

    while (payload != NULL) {
        if ((data_len + payload->size) > NG_ETHERNET_MAX_LEN) {
            DEBUG("ng_netdev_eth: Packet too big for ethernet frame\n");
            return -ENOBUFS;
        }

        memcpy(send_buffer + data_len, payload->data, payload->size);

        data_len += payload->size;
        payload = payload->next;
    }

    /* Pad to minimum payload size.
     * Linux does this on its own, but it doesn't hurt to do it here.
     * As of now only tuntaposx needs this. */
    if (data_len < (NG_ETHERNET_MIN_LEN)) {
        DEBUG("ng_netdev_eth: padding data! (%d -> ", data_len);
        memset(send_buffer + data_len, 0, NG_ETHERNET_MIN_LEN - data_len);
        data_len = NG_ETHERNET_MIN_LEN;
        DEBUG("%d)\n", data_len);
    }

    return data_len;
}

void dev_eth_isr(dev_eth_t* dev)
{
    (void)dev;
    msg_t msg;

    DEBUG("ng_netdev_eth: Trigger ISR event\n");

    /* TODO: check whether this is an input or an output event
       TODO: refactor this into general io-signal multiplexer */

    msg.type = NG_NETDEV_MSG_TYPE_EVENT;
    msg.content.value = _ISR_EVENT_RX;

    if (msg_send(&msg, ng_netdev_eth.mac_pid) <= 0) {
        puts("dev_eth_isr: possibly lost interrupt.");
    }
}

void dev_eth_rx_handler(dev_eth_t* dev) {
    (void)dev;
    _rx_event(&ng_netdev_eth);
}

void dev_eth_linkstate_handler(dev_eth_t *dev, int newstate)
{
    DEBUG("ng_dev_eth: dev=0x%08x link %s\n", (unsigned)dev, newstate ? "UP" : "DOWN");
    (void)dev; (void)newstate;
}

static void _rx_event(ng_netdev_eth_t *netdev)
{
    dev_eth_t *dev = netdev->ethdev;
    int nread = dev->driver->recv(dev, (char*)recv_buffer, NG_ETHERNET_MAX_LEN);

    DEBUG("ng_netdev_eth: read %d bytes\n", nread);

    if (nread > 0) {
        ng_ethernet_hdr_t *hdr = (ng_ethernet_hdr_t *)recv_buffer;
        ng_pktsnip_t *netif_hdr, *pkt;
        ng_nettype_t receive_type = NG_NETTYPE_UNDEF;
        size_t data_len = (nread - sizeof(ng_ethernet_hdr_t));

        /* TODO: implement multicast groups? */

        netif_hdr = ng_pktbuf_add(NULL, NULL,
                                  sizeof(ng_netif_hdr_t) + (2 * NG_ETHERNET_ADDR_LEN),
                                  NG_NETTYPE_NETIF);

        if (netif_hdr == NULL) {
            DEBUG("ng_netdev_eth: no space left in packet buffer\n");
            return;
        }

        ng_netif_hdr_init(netif_hdr->data, NG_ETHERNET_ADDR_LEN, NG_ETHERNET_ADDR_LEN);
        ng_netif_hdr_set_src_addr(netif_hdr->data, hdr->src, NG_ETHERNET_ADDR_LEN);
        ng_netif_hdr_set_dst_addr(netif_hdr->data, hdr->dst, NG_ETHERNET_ADDR_LEN);
        ((ng_netif_hdr_t *)netif_hdr->data)->if_pid = thread_getpid();

        receive_type = ng_nettype_from_ethertype(byteorder_ntohs(hdr->type));

        DEBUG("ng_netdev_eth: received packet from %02x:%02x:%02x:%02x:%02x:%02x "
              "of length %zu\n",
              hdr->src[0], hdr->src[1], hdr->src[2], hdr->src[3], hdr->src[4],
              hdr->src[5], data_len);
#if defined(MODULE_OD) && ENABLE_DEBUG
        od_hex_dump(hdr, nread, OD_WIDTH_DEFAULT);
#endif

        /* Mark netif header and payload for next layer */
        if ((pkt = ng_pktbuf_add(netif_hdr, recv_buffer + sizeof(ng_ethernet_hdr_t),
                                 data_len, receive_type)) == NULL) {
            ng_pktbuf_release(netif_hdr);
            DEBUG("ng_netdev_eth: no space left in packet buffer\n");
            return;
        }

        if (netdev->event_cb != NULL) {
            netdev->event_cb(NETDEV_EVENT_RX_COMPLETE, pkt);
        }
        else {
            ng_pktbuf_release(pkt); /* netif_hdr is released automatically too */
        }
    }
    else {
        DEBUG("ng_netdev_eth: spurious _rx_event: %d\n", nread);
    }
}

int ng_netdev_eth_init(ng_netdev_eth_t *netdev, dev_eth_t *ethdev) {
    if ((netdev == NULL) || (ethdev == NULL) || (netdev != &ng_netdev_eth)) {
        return -ENODEV;
    }

    /* initialize low-level driver */
    dev_eth_init(ethdev);

    /* initialize device descriptor */
    netdev->driver = (ng_netdev_driver_t *)(&ng_netdev_eth_driver);
    netdev->ethdev = ethdev;

    DEBUG("ng_netdev_eth: initialized.\n");

    return 0;
}
/**
 * @}
 */
