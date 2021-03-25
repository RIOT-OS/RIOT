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

#if MODULE_LWIP_DHCP_AUTO
#include "lwip/dhcp.h"
#endif
#include "lwip/err.h"
#include "lwip/ethip6.h"
#include "lwip/netif.h"
#include "lwip/netifapi.h"
#include "lwip/netif/netdev.h"
#include "lwip/opt.h"
#include "lwip/pbuf.h"
#include "netif/etharp.h"
#include "netif/lowpan6.h"

#include "net/eui64.h"
#include "net/ieee802154.h"
#include "net/ipv6/addr.h"
#include "net/netdev.h"
#include "net/netopt.h"
#include "utlist.h"
#include "thread.h"

#define ENABLE_DEBUG                0
#include "debug.h"

#define LWIP_NETDEV_NAME            "lwip_netdev_mux"
#define LWIP_NETDEV_PRIO            (THREAD_PRIORITY_MAIN - 4)
#define LWIP_NETDEV_STACKSIZE       (THREAD_STACKSIZE_DEFAULT)
#define LWIP_NETDEV_QUEUE_LEN       (8)
#define LWIP_NETDEV_MSG_TYPE_EVENT 0x1235

#define ETHERNET_IFNAME1 'E'
#define ETHERNET_IFNAME2 'T'

#define WPAN_IFNAME1 'W'
#define WPAN_IFNAME2 'P'

static kernel_pid_t _pid = KERNEL_PID_UNDEF;
static char _stack[LWIP_NETDEV_STACKSIZE];
static msg_t _queue[LWIP_NETDEV_QUEUE_LEN];
static char _tmp_buf[LWIP_NETDEV_BUFLEN];

#ifdef MODULE_NETDEV_ETH
static err_t _eth_link_output(struct netif *netif, struct pbuf *p);
#endif
#ifdef MODULE_LWIP_SIXLOWPAN
static err_t _ieee802154_link_output(struct netif *netif, struct pbuf *p);
#endif
static void _event_cb(netdev_t *dev, netdev_event_t event);
static void *_event_loop(void *arg);

static void _configure_netdev(netdev_t *dev)
{
    /* Enable RX-complete interrupts */
    static const netopt_enable_t enable = NETOPT_ENABLE;
    int res = dev->driver->set(dev, NETOPT_RX_END_IRQ, &enable, sizeof(enable));
    if (res < 0) {
        DEBUG("lwip_netdev: enable NETOPT_RX_END_IRQ failed: %d\n", res);
    }
}

err_t lwip_netdev_init(struct netif *netif)
{
    LWIP_ASSERT("netif != NULL", (netif != NULL));
    LWIP_ASSERT("netif->state != NULL", (netif->state != NULL));
    netdev_t *netdev;
    netopt_enable_t enabled = 0;
    uint16_t dev_type;
    err_t res = ERR_OK;

    /* start multiplexing thread (only one needed) */
    if (_pid <= KERNEL_PID_UNDEF) {
        _pid = thread_create(_stack, LWIP_NETDEV_STACKSIZE, LWIP_NETDEV_PRIO,
                             THREAD_CREATE_STACKTEST, _event_loop, netif,
                             LWIP_NETDEV_NAME);
        if (_pid <= 0) {
            return ERR_IF;
        }
    }

    /* initialize netdev and netif */
    netdev = (netdev_t *)netif->state;
    netdev->driver->init(netdev);
    _configure_netdev(netdev);
    netdev->event_callback = _event_cb;
    if (netdev->driver->get(netdev, NETOPT_DEVICE_TYPE, &dev_type,
                            sizeof(dev_type)) < 0) {
        return ERR_IF;
    }
#if LWIP_NETIF_HOSTNAME
    netif->hostname = "riot";
#endif /* LWIP_NETIF_HOSTNAME */

    /* XXX: for now assume its Ethernet, since netdev is implemented only by ethernet drivers */
    switch (dev_type) {
#ifdef MODULE_NETDEV_ETH
        case NETDEV_TYPE_ETHERNET:
            netif->name[0] = ETHERNET_IFNAME1;
            netif->name[1] = ETHERNET_IFNAME2;
            netif->hwaddr_len = (u8_t)netdev->driver->get(netdev, NETOPT_ADDRESS, netif->hwaddr,
                                                          sizeof(netif->hwaddr));
            if (netif->hwaddr_len > sizeof(netif->hwaddr)) {
                return ERR_IF;
            }
            /* TODO: get from driver (currently not in netdev_eth) */
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
        case NETDEV_TYPE_IEEE802154:
        {
            u16_t val;
            ip6_addr_t *addr;
            netif->name[0] = WPAN_IFNAME1;
            netif->name[1] = WPAN_IFNAME2;
            if (netdev->driver->get(netdev, NETOPT_NID, &val,
                                    sizeof(val)) < 0) {
                return ERR_IF;
            }
            lowpan6_set_pan_id(val);
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
            /* assure usage of long address as source address */
            val = netif->hwaddr_len;
            if (netdev->driver->set(netdev, NETOPT_SRC_LEN, &val, sizeof(val)) < 0) {
                return ERR_IF;
            }
            /* netif_create_ip6_linklocal_address() does weird byte-swapping
             * with full IIDs, so let's do it ourselves */
            addr = &(netif->ip6_addr[0]);
            /* addr->addr is a uint32_t array */
            if (l2util_ipv6_iid_from_addr(dev_type,
                                          netif->hwaddr, netif->hwaddr_len,
                                          (eui64_t *)&addr->addr[2]) < 0) {
                return ERR_IF;
            }
            ipv6_addr_set_link_local_prefix((ipv6_addr_t *)&addr->addr[0]);
            ip6_addr_assign_zone(addr, IP6_UNICAST, netif);
            /* Set address state. */
#if LWIP_IPV6_DUP_DETECT_ATTEMPTS
            /* Will perform duplicate address detection (DAD). */
            netif->ip6_addr_state[0] = IP6_ADDR_TENTATIVE;
#else
            /* Consider address valid. */
            netif->ip6_addr_state[0] = IP6_ADDR_PREFERRED;
#endif /* LWIP_IPV6_AUTOCONFIG */
            break;
        }
#endif
        default:
            return ERR_IF;  /* device type not supported yet */
    }
    netif->flags |= NETIF_FLAG_UP;
    /* Set link state up if link state is unsupported, or if it is up */
    if (netdev->driver->get(netdev, NETOPT_LINK, &enabled, sizeof(enabled)) <= 0 ||
        enabled) {
        netif->flags |= NETIF_FLAG_LINK_UP;
    }
    netif->flags |= NETIF_FLAG_IGMP;
    netif->flags |= NETIF_FLAG_MLD6;
    netdev->context = netif;
#if LWIP_IPV6_AUTOCONFIG
    netif->ip6_autoconfig_enabled = 1;
#endif

    return res;
}

#ifdef MODULE_NETDEV_ETH
static err_t _eth_link_output(struct netif *netif, struct pbuf *p)
{
    netdev_t *netdev = (netdev_t *)netif->state;
    struct pbuf *q;
    unsigned int count = 0;

#if ETH_PAD_SIZE
    pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif
    LL_COUNT(p, q, count);
    iolist_t iolist[count];

    /* make last point to the last entry of iolist[] */
    iolist_t *last = &iolist[count];
    last--;

    for (q = p, count = 0; q != NULL; q = q->next, count++) {
        iolist_t *iol = &iolist[count];

        iol->iol_next = (iol == last) ? NULL : &iolist[count + 1];

        iol->iol_base = q->payload;
        iol->iol_len = (size_t)q->len;
    }
#if ETH_PAD_SIZE
    pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif
    return (netdev->driver->send(netdev, iolist) > 0) ? ERR_OK : ERR_BUF;
}
#endif

#ifdef MODULE_LWIP_SIXLOWPAN
static err_t _ieee802154_link_output(struct netif *netif, struct pbuf *p)
{
    LWIP_ASSERT("p->next == NULL", p->next == NULL);
    netdev_t *netdev = (netdev_t *)netif->state;
    iolist_t pkt = {
        .iol_base = p->payload,
        .iol_len = (p->len - IEEE802154_FCS_LEN),   /* FCS is written by driver */
    };

    return (netdev->driver->send(netdev, &pkt) > 0) ? ERR_OK : ERR_BUF;
}
#endif

static struct pbuf *_get_recv_pkt(netdev_t *dev)
{
    int len = dev->driver->recv(dev, _tmp_buf, sizeof(_tmp_buf), NULL);

    if (len < 0) {
        DEBUG("lwip_netdev: an error occurred while reading the packet\n");
        return NULL;
    }
    assert(((unsigned)len) <= UINT16_MAX);
    struct pbuf *p = pbuf_alloc(PBUF_RAW, (u16_t)len, PBUF_POOL);

    if (p == NULL) {
        DEBUG("lwip_netdev: can not allocate in pbuf\n");
        return NULL;
    }
    pbuf_take(p, _tmp_buf, len);
    return p;
}

static void _event_cb(netdev_t *dev, netdev_event_t event)
{
    if (event == NETDEV_EVENT_ISR) {
        assert(_pid != KERNEL_PID_UNDEF);
        msg_t msg;

        msg.type = LWIP_NETDEV_MSG_TYPE_EVENT;
        msg.content.ptr = dev;

        if (msg_send(&msg, _pid) <= 0) {
            DEBUG("lwip_netdev: possibly lost interrupt.\n");
        }
    }
    else {
        struct netif *netif = dev->context;
        switch (event) {
            case NETDEV_EVENT_RX_COMPLETE: {
                struct pbuf *p = _get_recv_pkt(dev);
                if (p == NULL) {
                    DEBUG("lwip_netdev: error receiving packet\n");
                    return;
                }
                if (netif->input(p, netif) != ERR_OK) {
                    DEBUG("lwip_netdev: error inputing packet\n");
                    return;
                }
                break;
            }
            case NETDEV_EVENT_LINK_UP: {
                netifapi_netif_set_link_up(netif);
#ifdef MODULE_LWIP_DHCP_AUTO
                netifapi_dhcp_start(netif);
#endif
                break;
            }
            case NETDEV_EVENT_LINK_DOWN: {
                netifapi_netif_set_link_down(netif);
                break;
            }
            default:
                break;
        }
    }
}

static void *_event_loop(void *arg)
{
    (void)arg;
    msg_init_queue(_queue, LWIP_NETDEV_QUEUE_LEN);
    while (1) {
        msg_t msg;
        msg_receive(&msg);
        if (msg.type == LWIP_NETDEV_MSG_TYPE_EVENT) {
            netdev_t *dev = msg.content.ptr;
            dev->driver->isr(dev);
        }
    }
    return NULL;
}

/** @} */
