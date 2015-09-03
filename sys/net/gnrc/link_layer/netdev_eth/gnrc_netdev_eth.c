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
#include "net/ethernet.h"
#include "net/ethertype.h"
#include "net/ipv6/hdr.h"
#include "net/gnrc/netdev.h"
#include "net/gnrc/netif/hdr.h"
#include "net/gnrc/pkt.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/netdev_eth.h"
#include "net/dev_eth.h"
#include "od.h"
#include "thread.h"
#include "utlist.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

gnrc_netdev_eth_t gnrc_netdev_eth;

static uint8_t send_buffer[ETHERNET_MAX_LEN];
static uint8_t recv_buffer[ETHERNET_MAX_LEN];

#define _ISR_EVENT_RX (1U)

/* driver function definitions */
static int _send_data(gnrc_netdev_t *netdev, gnrc_pktsnip_t *pkt);
static int _add_event_callback(gnrc_netdev_t *dev, gnrc_netdev_event_cb_t cb);
static int _rem_event_callback(gnrc_netdev_t *dev, gnrc_netdev_event_cb_t cb);
static int _get(gnrc_netdev_t *dev, netopt_t opt, void *value, size_t max_len);
static int _set(gnrc_netdev_t *dev, netopt_t opt, void *value, size_t value_len);
static void _isr_event(gnrc_netdev_t *dev, uint32_t event_type);

/* netdev driver struct */
const gnrc_netdev_driver_t gnrc_netdev_eth_driver = {
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
#endif /* !(defined(__FreeBSD__) || defined(__MACH__)) */


/* build Ethernet packet from pkt */
static int _marshall_ethernet(gnrc_netdev_eth_t *dev, uint8_t *buffer, gnrc_pktsnip_t *pkt);

/* build ISR handler for ISR events */
void _trigger_isr_event(void);

static int _send_data(gnrc_netdev_t *netdev, gnrc_pktsnip_t *pkt)
{
    int nsent, to_send;
    gnrc_netdev_eth_t *dev = (gnrc_netdev_eth_t *)netdev;

    DEBUG("gnrc_netdev_eth: send data ");

    if (pkt == NULL) {
        return -EFAULT;
    }

    if ((dev == NULL) || (netdev->driver != &gnrc_netdev_eth_driver)) {
        DEBUG("[wrong device descriptor]\n");
        gnrc_pktbuf_release(pkt);
        return -ENODEV;
    }

    DEBUG("\n");

    to_send = _marshall_ethernet(dev, send_buffer, pkt);
    gnrc_pktbuf_release(pkt);

    if (to_send < 0) {
        errno = -to_send;
        DEBUG("marshall\n");
        return to_send;
    }

    DEBUG("gnrc_netdev_eth: send %d bytes\n", to_send);
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

static int _add_event_callback(gnrc_netdev_t *dev, gnrc_netdev_event_cb_t cb)
{
    DEBUG("gnrc_netdev_eth: add event callback");

    if ((dev == NULL) || (dev->driver != &gnrc_netdev_eth_driver)) {
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

static int _rem_event_callback(gnrc_netdev_t *dev, gnrc_netdev_event_cb_t cb)
{
    DEBUG("gnrc_netdev_eth: remove event callback");

    if ((dev == NULL) || (dev->driver != &gnrc_netdev_eth_driver)) {
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
static inline int _get_addr(gnrc_netdev_eth_t *netdev, uint8_t *value, size_t max_len)
{
    if (max_len < ETHERNET_ADDR_LEN) {
        /* value buffer not big enough */
        return -EOVERFLOW;
    }

    dev_eth_t *dev = netdev->ethdev;
    dev->driver->get_mac_addr(dev, value);

    return ETHERNET_ADDR_LEN;
}

static inline int _get_addr_len(uint16_t *value, size_t max_len)
{
    if (max_len != sizeof(uint16_t)) {
        /* value buffer not big enough */
        return -EOVERFLOW;
    }

    *value = ETHERNET_ADDR_LEN;

    return sizeof(uint16_t);
}

static inline int _get_iid(gnrc_netdev_eth_t *netdev, eui64_t *value, size_t max_len)
{
    if (max_len < sizeof(eui64_t)) {
        /* value buffer not big enough */
        return -EOVERFLOW;
    }

    dev_eth_t *dev = netdev->ethdev;
    uint8_t addr[ETHERNET_ADDR_LEN];
    dev->driver->get_mac_addr(dev, addr);
    ethernet_get_iid(value, addr);

    return sizeof(eui64_t);
}

static inline int _get_max_pkt_sz(uint16_t *value, size_t max_len)
{
    if (max_len != sizeof(uint16_t)) {
        /* value buffer not big enough */
        return -EOVERFLOW;
    }

    *value = ETHERNET_DATA_LEN;

    return sizeof(uint16_t);
}

static inline int _get_promiscousmode(gnrc_netdev_eth_t *netdev, netopt_enable_t *value,
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

static int _get(gnrc_netdev_t *dev, netopt_t opt, void *value, size_t max_len)
{
    DEBUG("gnrc_netdev_eth: get ");

    if ((dev == NULL) || (dev->driver != &gnrc_netdev_eth_driver)) {
        DEBUG("[wrong device descriptor]\n");
        return -ENODEV;
    }

    switch (opt) {
        case NETOPT_ADDRESS:
            DEBUG("address\n");
            return _get_addr((gnrc_netdev_eth_t *)dev, value, max_len);

        case NETOPT_ADDR_LEN:
            DEBUG("address length\n");
            return _get_addr_len(value, max_len);

        case NETOPT_IPV6_IID:
            DEBUG("IPv6 IID\n");
            return _get_iid((gnrc_netdev_eth_t *)dev, value, max_len);

        case NETOPT_MAX_PACKET_SIZE:
            DEBUG("maximum packet size\n");
            return _get_max_pkt_sz(value, max_len);

        case NETOPT_PROMISCUOUSMODE:
            DEBUG("promiscous mode\n");
            return _get_promiscousmode((gnrc_netdev_eth_t *)dev, value, max_len);

        case NETOPT_IS_WIRED:
            DEBUG("is wired\n");
            return 1;

        default:
            DEBUG("[not supported: %d]\n", opt);
            return -ENOTSUP;
    }
}

/* individual option getters to be called by _get() */
static inline int _set_promiscousmode(gnrc_netdev_eth_t *netdev, netopt_enable_t *value,
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

static int _set(gnrc_netdev_t *dev, netopt_t opt, void *value, size_t value_len)
{
    DEBUG("gnrc_netdev_eth: set ");

    if ((dev == NULL) || (dev->driver != &gnrc_netdev_eth_driver)) {
        DEBUG("[wrong device descriptor]\n");
        return -ENODEV;
    }

    switch (opt) {
        case NETOPT_PROMISCUOUSMODE:
            DEBUG("promiscous mode\n");
            return _set_promiscousmode((gnrc_netdev_eth_t *)dev, value, value_len);

        default:
            DEBUG("[not supported: %d]\n", opt);
            return -ENOTSUP;
    }
}

/* individual event handlers called by _isr_event() */
static void _rx_event(gnrc_netdev_eth_t *dev);

static void _isr_event(gnrc_netdev_t *dev, uint32_t event_type)
{
    DEBUG("gnrc_netdev_eth: ISR event ");

    if ((dev == NULL) || (dev->driver != &gnrc_netdev_eth_driver)) {
        return;
    }

    gnrc_netdev_eth_t *netdev = (gnrc_netdev_eth_t*)dev;

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
    memset(dst, 0xff, ETHERNET_ADDR_LEN);
}

static inline void _addr_set_multicast(uint8_t *dst, gnrc_pktsnip_t *payload)
{
    switch (payload->type) {
#ifdef MODULE_GNRC_IPV6
        case GNRC_NETTYPE_IPV6:
            dst[0] = 0x33;
            dst[1] = 0x33;
            if (payload->data != NULL) {
                ipv6_hdr_t *hdr = payload->data;
                uint16_t *prefix = (uint16_t *)(&dst[2]);
                prefix[0] = hdr->dst.u16[6].u16;
                prefix[1] = hdr->dst.u16[7].u16;
            }
            break;
#endif
        default:
            _addr_set_broadcast(dst);
            break;
    }
}

static int _marshall_ethernet(gnrc_netdev_eth_t *dev, uint8_t *buffer, gnrc_pktsnip_t *pkt)
{
    int data_len = 0;
    ethernet_hdr_t *hdr = (ethernet_hdr_t *)buffer;
    gnrc_netif_hdr_t *netif_hdr;
    gnrc_pktsnip_t *payload;

    if (pkt == NULL) {
        DEBUG("gnrc_netdev_eth: pkt was NULL");
        return -EINVAL;
    }

    payload = pkt->next;

    if (pkt->type != GNRC_NETTYPE_NETIF) {
        DEBUG("gnrc_netdev_eth: First header was not generic netif header\n");
        return -EBADMSG;
    }

    if (payload) {
        hdr->type = byteorder_htons(gnrc_nettype_to_ethertype(payload->type));
    }
    else {
        hdr->type = byteorder_htons(ETHERTYPE_UNKNOWN);
    }

    netif_hdr = pkt->data;

    /* set ethernet header */
    if (netif_hdr->src_l2addr_len == ETHERNET_ADDR_LEN) {
        memcpy(hdr->dst, gnrc_netif_hdr_get_src_addr(netif_hdr),
               netif_hdr->src_l2addr_len);
    }
    else {
        dev_eth_t *ethdev = dev->ethdev;
        ethdev->driver->get_mac_addr(ethdev, hdr->src);
    }

    if (netif_hdr->flags & GNRC_NETIF_HDR_FLAGS_BROADCAST) {
        _addr_set_broadcast(hdr->dst);
    }
    else if (netif_hdr->flags & GNRC_NETIF_HDR_FLAGS_MULTICAST) {
        if (payload == NULL) {
            DEBUG("gnrc_netdev_eth: empty multicast packets over Ethernet are "\
                  "not yet supported\n");
            return -ENOTSUP;

        }
        _addr_set_multicast(hdr->dst, payload);
    }
    else if (netif_hdr->dst_l2addr_len == ETHERNET_ADDR_LEN) {
        memcpy(hdr->dst, gnrc_netif_hdr_get_dst_addr(netif_hdr),
               ETHERNET_ADDR_LEN);
    }
    else {
        DEBUG("gnrc_netdev_eth: destination address had unexpected format\n");
        return -EBADMSG;
    }

    DEBUG("gnrc_netdev_eth: send to %02x:%02x:%02x:%02x:%02x:%02x\n",
          hdr->dst[0], hdr->dst[1], hdr->dst[2],
          hdr->dst[3], hdr->dst[4], hdr->dst[5]);

    data_len += sizeof(ethernet_hdr_t);

    while (payload != NULL) {
        if ((data_len + payload->size) > ETHERNET_MAX_LEN) {
            DEBUG("gnrc_netdev_eth: Packet too big for ethernet frame\n");
            return -ENOBUFS;
        }

        memcpy(send_buffer + data_len, payload->data, payload->size);

        data_len += payload->size;
        payload = payload->next;
    }

    /* Pad to minimum payload size.
     * Linux does this on its own, but it doesn't hurt to do it here.
     * As of now only tuntaposx needs this. */
    if (data_len < (ETHERNET_MIN_LEN)) {
        DEBUG("gnrc_netdev_eth: padding data! (%d -> ", data_len);
        memset(send_buffer + data_len, 0, ETHERNET_MIN_LEN - data_len);
        data_len = ETHERNET_MIN_LEN;
        DEBUG("%d)\n", data_len);
    }

    return data_len;
}

void dev_eth_isr(dev_eth_t* dev)
{
    (void)dev;
    msg_t msg;

    DEBUG("gnrc_netdev_eth: Trigger ISR event\n");

    /* TODO: check whether this is an input or an output event
       TODO: refactor this into general io-signal multiplexer */

    msg.type = GNRC_NETDEV_MSG_TYPE_EVENT;
    msg.content.value = _ISR_EVENT_RX;

    if (msg_send(&msg, gnrc_netdev_eth.mac_pid) <= 0) {
        puts("dev_eth_isr: possibly lost interrupt.");
    }
}

void dev_eth_rx_handler(dev_eth_t* dev) {
    (void)dev;
    _rx_event(&gnrc_netdev_eth);
}

void dev_eth_linkstate_handler(dev_eth_t *dev, int newstate)
{
    DEBUG("gnrc_dev_eth: dev=0x%08x link %s\n", (unsigned)dev, newstate ? "UP" : "DOWN");
    (void)dev; (void)newstate;
}

static void _rx_event(gnrc_netdev_eth_t *netdev)
{
    dev_eth_t *dev = netdev->ethdev;
    int nread = dev->driver->recv(dev, (char*)recv_buffer, ETHERNET_MAX_LEN);

    DEBUG("gnrc_netdev_eth: read %d bytes\n", nread);

    if (nread > 0) {
        ethernet_hdr_t *hdr = (ethernet_hdr_t *)recv_buffer;
        gnrc_pktsnip_t *netif_hdr, *pkt;
        gnrc_nettype_t receive_type = GNRC_NETTYPE_UNDEF;
        size_t data_len = (nread - sizeof(ethernet_hdr_t));

        /* TODO: implement multicast groups? */

        netif_hdr = gnrc_pktbuf_add(NULL, NULL,
                                    sizeof(gnrc_netif_hdr_t) + (2 * ETHERNET_ADDR_LEN),
                                    GNRC_NETTYPE_NETIF);

        if (netif_hdr == NULL) {
            DEBUG("gnrc_netdev_eth: no space left in packet buffer\n");
            return;
        }

        gnrc_netif_hdr_init(netif_hdr->data, ETHERNET_ADDR_LEN, ETHERNET_ADDR_LEN);
        gnrc_netif_hdr_set_src_addr(netif_hdr->data, hdr->src, ETHERNET_ADDR_LEN);
        gnrc_netif_hdr_set_dst_addr(netif_hdr->data, hdr->dst, ETHERNET_ADDR_LEN);
        ((gnrc_netif_hdr_t *)netif_hdr->data)->if_pid = thread_getpid();

        receive_type = gnrc_nettype_from_ethertype(byteorder_ntohs(hdr->type));

        DEBUG("gnrc_netdev_eth: received packet from %02x:%02x:%02x:%02x:%02x:%02x "
              "of length %zu\n",
              hdr->src[0], hdr->src[1], hdr->src[2], hdr->src[3], hdr->src[4],
              hdr->src[5], data_len);
#if defined(MODULE_OD) && ENABLE_DEBUG
        od_hex_dump(hdr, nread, OD_WIDTH_DEFAULT);
#endif

        /* Mark netif header and payload for next layer */
        if ((pkt = gnrc_pktbuf_add(netif_hdr, recv_buffer + sizeof(ethernet_hdr_t),
                                   data_len, receive_type)) == NULL) {
            gnrc_pktbuf_release(netif_hdr);
            DEBUG("gnrc_netdev_eth: no space left in packet buffer\n");
            return;
        }

        if (netdev->event_cb != NULL) {
            netdev->event_cb(NETDEV_EVENT_RX_COMPLETE, pkt);
        }
        else {
            gnrc_pktbuf_release(pkt);   /* netif_hdr is released automatically too */
        }
    }
    else {
        DEBUG("gnrc_netdev_eth: spurious _rx_event: %d\n", nread);
    }
}

int gnrc_netdev_eth_init(gnrc_netdev_eth_t *netdev, dev_eth_t *ethdev)
{
    if ((netdev == NULL) || (ethdev == NULL) || (netdev != &gnrc_netdev_eth)) {
        return -ENODEV;
    }

    /* initialize low-level driver */
    dev_eth_init(ethdev);

    /* initialize device descriptor */
    netdev->driver = (gnrc_netdev_driver_t *)(&gnrc_netdev_eth_driver);
    netdev->ethdev = ethdev;

    DEBUG("gnrc_netdev_eth: initialized.\n");

    return 0;
}
/**
 * @}
 */
