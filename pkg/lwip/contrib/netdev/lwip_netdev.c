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
#include <inttypes.h>
#include <sys/uio.h>

#include "architecture.h"
#include "event.h"
#include "lwip.h"
#include "lwip/err.h"
#include "lwip/ethip6.h"
#include "lwip/netif.h"
#include "lwip/netif/compat.h"
#include "lwip/netif/netdev.h"
#include "lwip/netifapi.h"
#include "lwip/opt.h"
#include "lwip/pbuf.h"
#include "net/eui64.h"
#include "net/ieee802154.h"
#include "net/ipv6/addr.h"
#include "net/netdev.h"
#include "net/netopt.h"
#include "netif/etharp.h"
#include "netif/lowpan6.h"
#include "thread.h"
#include "thread_flags.h"
#include "tiny_strerror.h"
#include "utlist.h"

#define ENABLE_DEBUG                0
#include "debug.h"

#define LWIP_NETDEV_NAME            "lwip_netdev_mux"
#define LWIP_NETDEV_PRIO            (THREAD_PRIORITY_MAIN - 4)
#define LWIP_NETDEV_STACKSIZE       (THREAD_STACKSIZE_DEFAULT)
#define LWIP_NETDEV_MSG_TYPE_EVENT  0x1235

#define ETHERNET_IFNAME1 'E'
#define ETHERNET_IFNAME2 'T'

#define WPAN_IFNAME1 'W'
#define WPAN_IFNAME2 'P'

event_queue_t lwip_event_queue = { 0 };
#define THREAD_FLAG_LWIP_TX_DONE    (1U << 11)

static kernel_pid_t _pid = KERNEL_PID_UNDEF;
static WORD_ALIGNED char _stack[LWIP_NETDEV_STACKSIZE];
static char _tmp_buf[LWIP_NETDEV_BUFLEN];

#ifdef MODULE_NETDEV_ETH
static err_t _eth_link_output(struct netif *netif, struct pbuf *p);
#endif
#ifdef MODULE_LWIP_SIXLOWPAN
static err_t _ieee802154_link_output(struct netif *netif, struct pbuf *p);
#endif
#ifdef MODULE_SLIPMUX_NET
static err_t _slip_link_output(struct netif *netif, struct pbuf *p);
#if LWIP_IPV4
static err_t slip_output4(struct netif *netif, struct pbuf *q, const ip4_addr_t *ipaddr);
#endif
#if LWIP_IPV6
static err_t slip_output6(struct netif *netif, struct pbuf *q, const ip6_addr_t *ip6addr);
#endif
#endif
static void _event_cb(netdev_t *dev, netdev_event_t event);
static void *_event_loop(void *arg);
static void _isr(event_t *ev);

bool is_netdev_legacy_api(netdev_t *netdev)
{
    static_assert(IS_USED(MODULE_NETDEV_NEW_API) || IS_USED(MODULE_NETDEV_LEGACY_API),
                  "used netdev misses dependency to netdev_legacy_api or netdev_new_api");
    if (!IS_USED(MODULE_NETDEV_NEW_API)) {
        return true;
    }
    if (!IS_USED(MODULE_NETDEV_LEGACY_API)) {
        return false;
    }
    return (netdev->driver->confirm_send == NULL);
}

err_t lwip_netdev_init(struct netif *netif)
{
    LWIP_ASSERT("netif != NULL", (netif != NULL));
    LWIP_ASSERT("netif->state != NULL", (netif->state != NULL));
    lwip_netif_t *compat_netif = container_of(netif, lwip_netif_t, lwip_netif);
    compat_netif->ev_isr.handler = _isr;
    netdev_t *netdev;
    netopt_enable_t enabled = 0;
    uint16_t dev_type;
    err_t res = ERR_OK;

    /* Init device lock */
    lwip_netif_dev_lock_init(netif);

    /* start multiplexing thread (only one needed) */
    if (_pid <= KERNEL_PID_UNDEF) {
        _pid = thread_create(_stack, LWIP_NETDEV_STACKSIZE, LWIP_NETDEV_PRIO,
                             0, _event_loop, netif,
                             LWIP_NETDEV_NAME);
        if (_pid <= 0) {
            return ERR_IF;
        }
    }

    netdev = netif->state;
    lwip_netif_dev_acquire(netif);
    netdev->event_callback = _event_cb;
    netdev->driver->init(netdev);
    if (netdev->driver->get(netdev, NETOPT_DEVICE_TYPE, &dev_type,
                            sizeof(dev_type)) < 0) {
        res = ERR_IF;
        goto free;
    }
#if LWIP_NETIF_HOSTNAME
    netif->hostname = "riot";
#endif /* LWIP_NETIF_HOSTNAME */

    switch (dev_type) {
#ifdef MODULE_NETDEV_ETH
    case NETDEV_TYPE_ETHERNET:
        netif->name[0] = ETHERNET_IFNAME1;
        netif->name[1] = ETHERNET_IFNAME2;
        netif->hwaddr_len = (u8_t)netdev->driver->get(netdev, NETOPT_ADDRESS, netif->hwaddr,
                                                      sizeof(netif->hwaddr));
        if (netif->hwaddr_len > sizeof(netif->hwaddr)) {
            res = ERR_IF;
            goto free;
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
            res = ERR_IF;
            goto free;
        }
        lowpan6_set_pan_id(val);
        netif->hwaddr_len = (u8_t)netdev->driver->get(netdev, NETOPT_ADDRESS_LONG,
                                                      netif->hwaddr, sizeof(netif->hwaddr));
        if (netif->hwaddr_len > sizeof(netif->hwaddr)) {
            res = ERR_IF;
            goto free;
        }
        netif->linkoutput = _ieee802154_link_output;
        res = lowpan6_if_init(netif);
        if (res != ERR_OK) {
            return res;
        }
        /* assure usage of long address as source address */
        val = netif->hwaddr_len;
        if (netdev->driver->set(netdev, NETOPT_SRC_LEN, &val, sizeof(val)) < 0) {
            res = ERR_IF;
            goto free;
        }
        /* netif_create_ip6_linklocal_address() does weird byte-swapping
         * with full IIDs, so let's do it ourselves */
        addr = ip_2_ip6(&(netif->ip6_addr[0]));
        /* addr->addr is a uint32_t array */
        if (l2util_ipv6_iid_from_addr(dev_type,
                                      netif->hwaddr, netif->hwaddr_len,
                                      (eui64_t *)&addr->addr[2]) < 0) {
            res = ERR_IF;
            goto free;
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
#ifdef MODULE_SLIPMUX_NET
    case NETDEV_TYPE_SLIP:
        netif->name[0] = 'S';
        netif->name[1] = 'L';

        /* TODO: get from driver (currently not in netdev_eth) */
        netif->mtu = ETHERNET_DATA_LEN;
        netif->linkoutput = _slip_link_output;
#if LWIP_IPV4
        netif->output = slip_output4;
#endif
#if LWIP_IPV6
        netif->output_ip6 = slip_output6;

        if (IS_USED(MODULE_SLIPMUX_L2ADDR)) {
            netif->hwaddr_len = (u8_t)netdev->driver->get(netdev, NETOPT_ADDRESS_LONG,
                                                          netif->hwaddr,
                                                          sizeof(netif->hwaddr));
            if (netif->hwaddr_len > sizeof(netif->hwaddr)) {
                res = ERR_IF;
                goto free;
            }

            /* netif_create_ip6_linklocal_address() does weird byte-swapping
             * with full IIDs, so let's do it ourselves */
            ip6_addr_t *addr = ip_2_ip6(&(netif->ip6_addr[0]));
            /* addr->addr is a uint32_t array */
            if (l2util_ipv6_iid_from_addr(dev_type,
                                          netif->hwaddr, netif->hwaddr_len,
                                          (eui64_t *)&addr->addr[2]) < 0) {
                res = ERR_IF;
                goto free;
            }
            ipv6_addr_set_link_local_prefix((ipv6_addr_t *)&addr->addr[0]);
            ip6_addr_assign_zone(addr, IP6_UNICAST, netif);
            /* Consider address valid. */
            netif->ip6_addr_state[0] = IP6_ADDR_PREFERRED;
        }
#endif /* LWIP_IPV6 */
        break;
#endif
    default:
        res = ERR_IF;
        goto free;
    }
    netif->flags |= NETIF_FLAG_UP;
    /* Set link state up if link state is unsupported, or if it is up */
    if (netdev->driver->get(netdev, NETOPT_LINK, &enabled, sizeof(enabled)) <= 0 ||
        enabled) {
        netif->flags |= NETIF_FLAG_LINK_UP;
    }
    netif->flags |= NETIF_FLAG_IGMP;
    netif->flags |= NETIF_FLAG_MLD6;
#if LWIP_IPV6_AUTOCONFIG
    netif->ip6_autoconfig_enabled = 1;
#endif
free:
    lwip_netif_dev_release(netif);

    return res;
}

#if (IS_USED(MODULE_NETDEV_NEW_API))
static err_t _common_link_output(struct netif *netif, netdev_t *netdev, iolist_t *iolist)
{
    lwip_netif_dev_acquire(netif);

    int res;
    if (is_netdev_legacy_api(netdev)) {
        res = netdev->driver->send(netdev, iolist);
        lwip_netif_dev_release(netif);
        return res > 0 ? ERR_OK : ERR_BUF;
    }

    unsigned irq_state;
    lwip_netif_t *compat_netif = container_of(netif, lwip_netif_t, lwip_netif);

    irq_state = irq_disable();
    compat_netif->thread_doing_tx = thread_get_active();
    irq_restore(irq_state);

    res = netdev->driver->send(netdev, iolist);
    if (res < 0) {
        lwip_netif_dev_release(netif);
        irq_state = irq_disable();
        compat_netif->thread_doing_tx = NULL;
        irq_restore(irq_state);
        DEBUG("[lwip_netdev] send() returned %s\n", tiny_strerror(res));
        return ERR_IF;
    }

    /* `res > 0` means transmission already completed according to API contract.
     * ==> only waiting when res == 0 */
    if (res == 0) {
        /* block until TX completion is signaled from IRQ */
        thread_flags_wait_any(THREAD_FLAG_LWIP_TX_DONE);

        irq_state = irq_disable();
        compat_netif->thread_doing_tx = NULL;
        irq_restore(irq_state);

        /* async send */
        while (-EAGAIN == (res = netdev->driver->confirm_send(netdev, NULL))) {
            /* this should not happen, as the driver really only should emit the
             * TX done event when it is actually done. But better be safe than
             * sorry */
            DEBUG_PUTS("[lwip_netdev] confirm_send() returned -EAGAIN\n");
        }
    }

    lwip_netif_dev_release(netif);

    if (res < 0) {
        DEBUG("[lwip_netdev] confirm_send() returned %s\n", tiny_strerror(res));
        return ERR_IF;
    }

    return ERR_OK;
}
#else /* only old API */
static err_t _common_link_output(struct netif *netif, netdev_t *netdev, iolist_t *iolist)
{
    lwip_netif_dev_acquire(netif);
    int res = netdev->driver->send(netdev, iolist);
    lwip_netif_dev_release(netif);
    return res > 0 ? ERR_OK : ERR_BUF;
}
#endif

#ifdef MODULE_NETDEV_ETH
static err_t _eth_link_output(struct netif *netif, struct pbuf *p)
{
    netdev_t *netdev = netif->state;
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
    return _common_link_output(netif, netdev, iolist);
}
#endif

#ifdef MODULE_LWIP_SIXLOWPAN
static err_t _ieee802154_link_output(struct netif *netif, struct pbuf *p)
{
    LWIP_ASSERT("p->next == NULL", p->next == NULL);
    netdev_t *netdev = netif->state;
    iolist_t pkt = {
        .iol_base = p->payload,
        .iol_len = (p->len - IEEE802154_FCS_LEN),   /* FCS is written by driver */
    };

    return _common_link_output(netif, netdev, &pkt);
}
#endif

#ifdef MODULE_SLIPMUX_NET
#if LWIP_IPV4
static err_t slip_output4(struct netif *netif, struct pbuf *q, const ip4_addr_t *ipaddr)
{
    (void)ipaddr;
    return netif->linkoutput(netif, q);
}
#endif
#if LWIP_IPV6
static err_t slip_output6(struct netif *netif, struct pbuf *q, const ip6_addr_t *ip6addr)
{
    (void)ip6addr;
    return netif->linkoutput(netif, q);
}
#endif

static err_t _slip_link_output(struct netif *netif, struct pbuf *p)
{
    LWIP_ASSERT("p->next == NULL", p->next == NULL);
    netdev_t *netdev = netif->state;
    iolist_t pkt = {
        .iol_base = p->payload,
        .iol_len = p->len,
    };

    return _common_link_output(netif, netdev, &pkt);
}
#endif

static struct pbuf *_get_recv_pkt(netdev_t *dev)
{
    lwip_netif_t *compat_netif = dev->context;
    struct netif *netif = &compat_netif->lwip_netif;
    lwip_netif_dev_acquire(netif);
    int len = dev->driver->recv(dev, _tmp_buf, sizeof(_tmp_buf), NULL);
    lwip_netif_dev_release(netif);

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
    lwip_netif_t *compat_netif = dev->context;
    assert(compat_netif != NULL);
    struct netif *netif = &compat_netif->lwip_netif;

    switch (event) {
    case NETDEV_EVENT_ISR:
        DEBUG_PUTS("[lwip_netdev] NETDEV_EVENT_ISR");
        event_post(&lwip_event_queue, &compat_netif->ev_isr);
        break;
#if (IS_USED(MODULE_NETDEV_NEW_API))
    case NETDEV_EVENT_TX_COMPLETE:
        DEBUG_PUTS("[lwip_netdev] NETDEV_EVENT_TX_COMPLETE");
        {
            unsigned irq_state = irq_disable();
            thread_t *target = compat_netif->thread_doing_tx;
            irq_restore(irq_state);
            if (target) {
                thread_flags_set(target, THREAD_FLAG_LWIP_TX_DONE);
            }
        }
        break;
#endif
    case NETDEV_EVENT_RX_COMPLETE:
        DEBUG_PUTS("[lwip_netdev] NETDEV_EVENT_RX_COMPLETE");
        {
            struct pbuf *p = _get_recv_pkt(dev);
            if (p == NULL) {
                DEBUG("lwip_netdev: error receiving packet\n");
                return;
            }
            if (netif->input(p, netif) != ERR_OK) {
                DEBUG("lwip_netdev: error inputing packet\n");
                return;
            }
        }
        break;
    case NETDEV_EVENT_LINK_UP:
        DEBUG_PUTS("[lwip_netdev] NETDEV_EVENT_LINK_UP");
        /* Will wake up DHCP state machine */
        netifapi_netif_set_link_up(netif);
        break;
    case NETDEV_EVENT_LINK_DOWN:
        DEBUG_PUTS("[lwip_netdev] NETDEV_EVENT_LINK_DOWN");
        netifapi_netif_set_link_down(netif);
        break;
    default:
        break;
    }
}

static void _isr(event_t *ev)
{
    lwip_netif_t *compat_netif = container_of(ev, lwip_netif_t, ev_isr);
    netdev_t *dev = compat_netif->lwip_netif.state;
    dev->driver->isr(dev);
}

static void *_event_loop(void *arg)
{
    (void)arg;
    event_queue_claim(&lwip_event_queue);
    event_loop(&lwip_event_queue);

    /* this should never be reached */
    assert(0);
    return NULL;
}

/** @} */
