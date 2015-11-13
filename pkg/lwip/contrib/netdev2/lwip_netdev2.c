/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <assert.h>
#include <sys/uio.h>
#include <inttypes.h>

#include "lwip/err.h"
#include "lwip/ethip6.h"
#include "lwip/netif.h"
#include "lwip/netif/netdev2.h"
#include "lwip/opt.h"
#include "lwip/pbuf.h"
#include "netif/etharp.h"
#include "netif/lowpan6.h"

#include "net/ieee802154.h"
#include "net/netdev2.h"
#include "net/netopt.h"
#include "utlist.h"
#include "thread.h"

#define ENABLE_DEBUG                (0)
#include "debug.h"

#define LWIP_NETDEV2_NAME           "lwip_netdev2_mux"
#define LWIP_NETDEV2_PRIO           (THREAD_PRIORITY_MAIN - 4)
#define LWIP_NETDEV2_STACKSIZE      (THREAD_STACKSIZE_DEFAULT)
#define LWIP_NETDEV2_QUEUE_LEN      (8)
#define LWIP_NETDEV2_MSG_TYPE_EVENT 0x1235

#define ETHERNET_IFNAME1 'E'
#define ETHERNET_IFNAME2 'T'

/* running number for different interfaces */
static uint8_t _num = 0;
static kernel_pid_t _pid = KERNEL_PID_UNDEF;
static char _stack[LWIP_NETDEV2_STACKSIZE];
static msg_t _queue[LWIP_NETDEV2_QUEUE_LEN];
static char _tmp_buf[LWIP_NETDEV2_BUFLEN];

#ifdef MODULE_NETDEV2_ETH
static err_t _eth_link_output(struct netif *netif, struct pbuf *p);
#endif
#ifdef MODULE_LWIP_SIXLOWPAN
static err_t _ieee802154_link_output(struct netif *netif, struct pbuf *p);
#endif
static void _event_cb(netdev2_t *dev, netdev2_event_t event, void *arg);
static void *_event_loop(void *arg);

err_t lwip_netdev2_init(struct netif *netif)
{
    LWIP_ASSERT("netif != NULL", (netif != NULL));
    LWIP_ASSERT("netif->state != NULL", (netif->state != NULL));
    netdev2_t *netdev;
    uint16_t dev_type;
    err_t res = ERR_OK;

    /* start multiplexing thread (only one needed) */
    if (_pid <= KERNEL_PID_UNDEF) {
        _pid = thread_create(_stack, LWIP_NETDEV2_STACKSIZE, LWIP_NETDEV2_PRIO,
                             THREAD_CREATE_STACKTEST, _event_loop, netif,
                             LWIP_NETDEV2_NAME);
        if (_pid <= 0) {
            return ERR_IF;
        }
    }

    /* initialize netdev and netif */
    netdev = (netdev2_t *)netif->state;
    netdev->driver->init(netdev);
    netdev->event_callback = _event_cb;
    if (netdev->driver->get(netdev, NETOPT_DEVICE_TYPE, &dev_type,
                            sizeof(dev_type)) < 0) {
        return ERR_IF;
    }
    netif->num = _num++;
#if LWIP_NETIF_HOSTNAME
    netif->hostname = "riot";
#endif /* LWIP_NETIF_HOSTNAME */

    /* XXX: for now assume its Ethernet, since netdev2 is implemented only by ethernet drivers */
    netif->flags = 0;
    switch (dev_type) {
#ifdef MODULE_NETDEV2_ETH
        case NETDEV2_TYPE_ETHERNET:
            netif->name[0] = ETHERNET_IFNAME1;
            netif->name[1] = ETHERNET_IFNAME2;
            netif->hwaddr_len = (u8_t)netdev->driver->get(netdev, NETOPT_ADDRESS, netif->hwaddr,
                                                          sizeof(netif->hwaddr));
            if (netif->hwaddr_len > sizeof(netif->hwaddr)) {
                return ERR_IF;
            }
            /* TODO: get from driver (currently not in netdev2_eth) */
            netif->mtu = ETHERNET_DATA_LEN;
            netif->linkoutput = _eth_link_output;
#if LWIP_IPV4
            netif->output = etharp_output;
#endif
#if LWIP_IPV6
            netif->output_ip6 = ethip6_output;
            netif_create_ip6_linklocal_address(netif, 1);   /* 1: hwaddr is 48-bit MAC addr */
#endif
            netif->flags |= NETIF_FLAG_BROADCAST;
            netif->flags |= NETIF_FLAG_ETHARP;
            netif->flags |= NETIF_FLAG_ETHERNET;
            break;
#endif
#ifdef MODULE_LWIP_SIXLOWPAN
        case NETDEV2_TYPE_IEEE802154:
        {
            u16_t pan_id;
            if (netdev->driver->get(netdev, NETOPT_NID, &pan_id,
                                    sizeof(pan_id)) < 0) {
                return ERR_IF;
            }
            lowpan6_set_pan_id(pan_id);
            netif->hwaddr_len = (u8_t)netdev->driver->get(netdev, NETOPT_ADDRESS_LONG,
                                                          netif->hwaddr, sizeof(netif->hwaddr));
            if (netif->hwaddr_len > sizeof(netif->hwaddr)) {
                return ERR_IF;
            }
            netif->linkoutput = _ieee802154_link_output;
            res = lowpan6_if_init(netif);
            if (res != ERR_OK) {
                return res;
            }
            netif_create_ip6_linklocal_address(netif, 0);   /* 0: hwaddr is assumed to be 64-bit */
            break;
        }
#endif
        default:
            return ERR_IF;  /* device type not supported yet */
    }
    netif->flags |= NETIF_FLAG_UP;
    netif->flags |= NETIF_FLAG_LINK_UP;
    netif->flags |= NETIF_FLAG_IGMP;
    netif->flags |= NETIF_FLAG_MLD6;
    netdev->isr_arg = netif;
    netdev->event_callback = _event_cb;
#if LWIP_IPV6_AUTOCONFIG
    netif->ip6_autoconfig_enabled = 1;
#endif

    return res;
}

#ifdef MODULE_NETDEV2_ETH
static err_t _eth_link_output(struct netif *netif, struct pbuf *p)
{
    netdev2_t *netdev = (netdev2_t *)netif->state;
    struct pbuf *q;
    unsigned int count = 0;

#if ETH_PAD_SIZE
    pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif
    LL_COUNT(p, q, count);
    struct iovec pkt[count];
    for (q = p, count = 0; q != NULL; q = q->next, count++) {
        pkt[count].iov_base = q->payload;
        pkt[count].iov_len = (size_t)q->len;
    }
#if ETH_PAD_SIZE
    pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif
    return (netdev->driver->send(netdev, pkt, count) > 0) ? ERR_OK : ERR_BUF;
}
#endif

#ifdef MODULE_LWIP_SIXLOWPAN
static err_t _ieee802154_link_output(struct netif *netif, struct pbuf *p)
{
    LWIP_ASSERT("p->next == NULL", p->next == NULL);
    netdev2_t *netdev = (netdev2_t *)netif->state;
    struct iovec pkt = {
        .iov_base = p->payload,
        .iov_len = (p->len - IEEE802154_FCS_LEN),   /* FCS is written by driver */
    };

    return (netdev->driver->send(netdev, &pkt, 1) > 0) ? ERR_OK : ERR_BUF;
}
#endif

static struct pbuf *_get_recv_pkt(netdev2_t *dev)
{
    int len = dev->driver->recv(dev, _tmp_buf, sizeof(_tmp_buf), NULL);

    assert(((unsigned)len) <= UINT16_MAX);
    struct pbuf *p = pbuf_alloc(PBUF_RAW, (u16_t)len, PBUF_POOL);

    if (p == NULL) {
        DEBUG("lwip_netdev2: can not allocate in pbuf\n");
        return NULL;
    }
    pbuf_take(p, _tmp_buf, len);
    return p;
}

static void _event_cb(netdev2_t *dev, netdev2_event_t event, void *arg)
{
    (void)arg;
    if (event == NETDEV2_EVENT_ISR) {
        assert(_pid != KERNEL_PID_UNDEF);
        msg_t msg;

        msg.type = LWIP_NETDEV2_MSG_TYPE_EVENT;
        msg.content.ptr = (char *)dev;

        if (msg_send(&msg, _pid) <= 0) {
            DEBUG("lwip_netdev2: possibly lost interrupt.\n");
        }
    }
    else {
        struct netif *netif = dev->isr_arg;
        switch (event) {
            case NETDEV2_EVENT_RX_COMPLETE: {
                struct pbuf *p = _get_recv_pkt(dev);
                if (p == NULL) {
                    DEBUG("lwip_netdev2: error receiving packet\n");
                    return;
                }
                if (netif->input(p, netif) != ERR_OK) {
                    DEBUG("lwip_netdev2: error inputing packet\n");
                    return;
                }
            }
            break;
            default:
                break;
        }
    }
}

static void *_event_loop(void *arg)
{
    (void)arg;
    msg_init_queue(_queue, LWIP_NETDEV2_QUEUE_LEN);
    while (1) {
        msg_t msg;
        msg_receive(&msg);
        if (msg.type == LWIP_NETDEV2_MSG_TYPE_EVENT) {
            netdev2_t *dev = (netdev2_t *)msg.content.ptr;
            dev->driver->isr(dev);
        }
    }
    return NULL;
}

/** @} */
