/*
 * Copyright (C) 2014-2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 * @author  René Kijewski <rene.kijewski@fu-berlin.de>
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 */

#include "net/ethernet.h"
#include "net/ipv6.h"
#ifdef MODULE_GNRC_IPV6_NIB
#include "net/gnrc/ipv6/nib.h"
#include "net/gnrc/ipv6.h"
#endif /* MODULE_GNRC_IPV6_NIB */
#include "net/gnrc/netif.h"
#include "net/gnrc/netif/internal.h"
#include "net/gnrc.h"
#include "net/netdev.h"

#include "log.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#ifdef MODULE_GNRC_NETIF_EVENTS
/**
 * @brief   Event type used for passing netdev pointers together with the event
 */
typedef struct {
    event_t super;
    netdev_t *dev;
} event_netdev_t;
#endif /* MODULE_GNRC_NETIF_EVENTS */

static void _configure_netdev(netdev_t *dev);
static void _event_cb(netdev_t *dev, netdev_event_t event);
static void _update_l2addr_from_dev(gnrc_netif_t *netif);

int gnrc_netif_get_from_netdev(gnrc_netif_t *netif, gnrc_netapi_opt_t *opt)
{
    int res = -ENOTSUP;

    gnrc_netif_acquire(netif);
    switch (opt->opt) {
        case NETOPT_6LO:
            assert(opt->data_len == sizeof(netopt_enable_t));
            *((netopt_enable_t *)opt->data) =
                    (netopt_enable_t)gnrc_netif_is_6lo(netif);
            res = sizeof(netopt_enable_t);
            break;
        case NETOPT_HOP_LIMIT:
            assert(opt->data_len == sizeof(uint8_t));
            *((uint8_t *)opt->data) = netif->cur_hl;
            res = sizeof(uint8_t);
            break;
        case NETOPT_STATS:
            /* XXX discussed this with Oleg, it's supposed to be a pointer */
            switch ((int16_t)opt->context) {
#if IS_USED(MODULE_NETSTATS_IPV6) && IS_USED(MODULE_GNRC_NETIF_IPV6)
                case NETSTATS_IPV6:
                    assert(opt->data_len == sizeof(netstats_t *));
                    *((netstats_t **)opt->data) = &netif->ipv6.stats;
                    res = sizeof(&netif->ipv6.stats);
                    break;
#endif
#ifdef MODULE_NETSTATS_L2
                case NETSTATS_LAYER2:
                    assert(opt->data_len == sizeof(netstats_t *));
                    *((netstats_t **)opt->data) = &netif->stats;
                    res = sizeof(&netif->stats);
                    break;
#endif
                default:
                    /* take from device */
                    break;
            }
            break;
#if IS_USED(MODULE_GNRC_NETIF_IPV6)
        case NETOPT_IPV6_ADDR: {
                assert(opt->data_len >= sizeof(ipv6_addr_t));
                ipv6_addr_t *tgt = opt->data;

                res = 0;
                for (unsigned i = 0;
                     (res < (int)opt->data_len) &&
                     (i < CONFIG_GNRC_NETIF_IPV6_ADDRS_NUMOF);
                     i++) {
                    if (netif->ipv6.addrs_flags[i] != 0) {
                        memcpy(tgt, &netif->ipv6.addrs[i], sizeof(ipv6_addr_t));
                        res += sizeof(ipv6_addr_t);
                        tgt++;
                    }
                }
            }
            break;
        case NETOPT_IPV6_ADDR_FLAGS: {
                assert(opt->data_len >= sizeof(uint8_t));
                uint8_t *tgt = opt->data;

                res = 0;
                for (unsigned i = 0;
                     (res < (int)opt->data_len) &&
                     (i < CONFIG_GNRC_NETIF_IPV6_ADDRS_NUMOF);
                     i++) {
                    if (netif->ipv6.addrs_flags[i] != 0) {
                        *tgt = netif->ipv6.addrs_flags[i];
                        res += sizeof(uint8_t);
                        tgt++;
                    }
                }
            }
            break;
        case NETOPT_IPV6_GROUP: {
                assert(opt->data_len >= sizeof(ipv6_addr_t));
                ipv6_addr_t *tgt = opt->data;

                res = 0;
                for (unsigned i = 0;
                     (res < (int)opt->data_len) &&
                     (i < GNRC_NETIF_IPV6_GROUPS_NUMOF);
                     i++) {
                    if (!ipv6_addr_is_unspecified(&netif->ipv6.groups[i])) {
                        memcpy(tgt, &netif->ipv6.groups[i],
                               sizeof(ipv6_addr_t));
                        res += sizeof(ipv6_addr_t);
                        tgt++;
                    }
                }
            }
            break;
        case NETOPT_IPV6_IID:
            assert(opt->data_len >= sizeof(eui64_t));
            res = gnrc_netif_ipv6_get_iid(netif, opt->data);
            break;
        case NETOPT_MAX_PDU_SIZE:
            if (opt->context == GNRC_NETTYPE_IPV6) {
                assert(opt->data_len == sizeof(uint16_t));
                *((uint16_t *)opt->data) = netif->ipv6.mtu;
                res = sizeof(uint16_t);
            }
            /* else ask device */
            break;
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_ROUTER)
        case NETOPT_IPV6_FORWARDING:
            assert(opt->data_len == sizeof(netopt_enable_t));
            *((netopt_enable_t *)opt->data) = (gnrc_netif_is_rtr(netif)) ?
                                              NETOPT_ENABLE : NETOPT_DISABLE;
            res = sizeof(netopt_enable_t);
            break;
        case NETOPT_IPV6_SND_RTR_ADV:
            assert(opt->data_len == sizeof(netopt_enable_t));
            *((netopt_enable_t *)opt->data) = (gnrc_netif_is_rtr_adv(netif)) ?
                                              NETOPT_ENABLE : NETOPT_DISABLE;
            res = sizeof(netopt_enable_t);
            break;
#endif  /* CONFIG_GNRC_IPV6_NIB_ROUTER */
#endif  /* IS_USED(MODULE_GNRC_NETIF_IPV6) */
#ifdef MODULE_GNRC_SIXLOWPAN_IPHC
        case NETOPT_6LO_IPHC:
            assert(opt->data_len == sizeof(netopt_enable_t));
            *((netopt_enable_t *)opt->data) = (netif->flags &
                                               GNRC_NETIF_FLAGS_6LO_HC)
                                            ? NETOPT_ENABLE : NETOPT_DISABLE;
            res = sizeof(netopt_enable_t);
            break;
#endif  /* MODULE_GNRC_SIXLOWPAN_IPHC */
        default:
            break;
    }
    if (res == -ENOTSUP) {
        netdev_t *dev = netif->context;
        res = dev->driver->get(dev, opt->opt, opt->data, opt->data_len);
    }
    gnrc_netif_release(netif);
    return res;
}

int gnrc_netif_set_from_netdev(gnrc_netif_t *netif,
                               const gnrc_netapi_opt_t *opt)
{
    int res = -ENOTSUP;
    netdev_t *dev = netif->context;

    gnrc_netif_acquire(netif);
    switch (opt->opt) {
        case NETOPT_HOP_LIMIT:
            assert(opt->data_len == sizeof(uint8_t));
            netif->cur_hl = *((uint8_t *)opt->data);
            res = sizeof(uint8_t);
            break;
#if IS_USED(MODULE_GNRC_NETIF_IPV6)
        case NETOPT_IPV6_ADDR: {
                assert(opt->data_len == sizeof(ipv6_addr_t));
                /* always assume manually added */
                uint8_t flags = ((((uint8_t)opt->context & 0xff) &
                                  ~GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_MASK) |
                                 GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID);
                uint8_t pfx_len = (uint8_t)(opt->context >> 8U);
                /* acquire locks a recursive mutex so we are safe calling this
                 * public function */
                res = gnrc_netif_ipv6_addr_add_internal(netif, opt->data,
                                                        pfx_len, flags);
                if (res >= 0) {
                    res = sizeof(ipv6_addr_t);
                }
            }
            break;
        case NETOPT_IPV6_ADDR_REMOVE:
            assert(opt->data_len == sizeof(ipv6_addr_t));
            /* acquire locks a recursive mutex so we are safe calling this
             * public function */
            gnrc_netif_ipv6_addr_remove_internal(netif, opt->data);
            res = sizeof(ipv6_addr_t);
            break;
        case NETOPT_IPV6_GROUP:
            assert(opt->data_len == sizeof(ipv6_addr_t));
            /* acquire locks a recursive mutex so we are safe calling this
             * public function */
            res = gnrc_netif_ipv6_group_join_internal(netif, opt->data);
            if (res >= 0) {
                res = sizeof(ipv6_addr_t);
            }
            break;
        case NETOPT_IPV6_GROUP_LEAVE:
            assert(opt->data_len == sizeof(ipv6_addr_t));
            /* acquire locks a recursive mutex so we are safe calling this
             * public function */
            gnrc_netif_ipv6_group_leave_internal(netif, opt->data);
            res = sizeof(ipv6_addr_t);
            break;
        case NETOPT_MAX_PDU_SIZE:
            if (opt->context == GNRC_NETTYPE_IPV6) {
                assert(opt->data_len == sizeof(uint16_t));
                netif->ipv6.mtu = *((uint16_t *)opt->data);
                res = sizeof(uint16_t);
            }
            /* else set device */
            break;
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_ROUTER)
        case NETOPT_IPV6_FORWARDING:
            assert(opt->data_len == sizeof(netopt_enable_t));
            if (*(((netopt_enable_t *)opt->data)) == NETOPT_ENABLE) {
                netif->flags |= GNRC_NETIF_FLAGS_IPV6_FORWARDING;
            }
            else {
                if (gnrc_netif_is_rtr_adv(netif)) {
                    gnrc_ipv6_nib_change_rtr_adv_iface(netif, false);
                }
                netif->flags &= ~GNRC_NETIF_FLAGS_IPV6_FORWARDING;
            }
            res = sizeof(netopt_enable_t);
            break;
        case NETOPT_IPV6_SND_RTR_ADV:
            assert(opt->data_len == sizeof(netopt_enable_t));
            gnrc_ipv6_nib_change_rtr_adv_iface(netif,
                    (*(((netopt_enable_t *)opt->data)) == NETOPT_ENABLE));
            res = sizeof(netopt_enable_t);
            break;
#endif  /* CONFIG_GNRC_IPV6_NIB_ROUTER */
#endif  /* IS_USED(MODULE_GNRC_NETIF_IPV6) */
#ifdef MODULE_GNRC_SIXLOWPAN_IPHC
        case NETOPT_6LO_IPHC:
            assert(opt->data_len == sizeof(netopt_enable_t));
            if (*(((netopt_enable_t *)opt->data)) == NETOPT_ENABLE) {
                netif->flags |= GNRC_NETIF_FLAGS_6LO_HC;
            }
            else {
                netif->flags &= ~GNRC_NETIF_FLAGS_6LO_HC;
            }
            res = sizeof(netopt_enable_t);
            break;
#endif  /* MODULE_GNRC_SIXLOWPAN_IPHC */
        case NETOPT_RAWMODE:
            if (*(((netopt_enable_t *)opt->data)) == NETOPT_ENABLE) {
                netif->flags |= GNRC_NETIF_FLAGS_RAWMODE;
            }
            else {
                netif->flags &= ~GNRC_NETIF_FLAGS_RAWMODE;
            }
            /* Also propagate to the netdev device */
            dev->driver->set(dev, NETOPT_RAWMODE, opt->data,
                                      opt->data_len);
            res = sizeof(netopt_enable_t);
            break;
        default:
            break;
    }
    if (res == -ENOTSUP) {
        res = dev->driver->set(dev, opt->opt, opt->data,
                                      opt->data_len);
        if (res > 0) {
            switch (opt->opt) {
                case NETOPT_ADDRESS:
                case NETOPT_ADDRESS_LONG:
                case NETOPT_ADDR_LEN:
                case NETOPT_SRC_LEN:
                    _update_l2addr_from_dev(netif);
                    break;
                case NETOPT_IEEE802154_PHY:
                    gnrc_netif_ipv6_init_mtu(netif);
                    break;
                case NETOPT_STATE:
                    if (*((netopt_state_t *)opt->data) == NETOPT_STATE_RESET) {
                        _configure_netdev(dev);
                    }
                    break;
                default:
                    break;
            }
        }
    }
    gnrc_netif_release(netif);
    return res;
}

static void _configure_netdev(netdev_t *dev)
{
    /* Enable RX- and TX-complete interrupts */
    static const netopt_enable_t enable = NETOPT_ENABLE;
    int res = dev->driver->set(dev, NETOPT_RX_END_IRQ, &enable, sizeof(enable));
    if (res < 0) {
        DEBUG("gnrc_netif: enable NETOPT_RX_END_IRQ failed: %d\n", res);
    }
    if (IS_USED(MODULE_NETSTATS_L2) || IS_USED(MODULE_GNRC_NETIF_PKTQ)) {
        res = dev->driver->set(dev, NETOPT_TX_END_IRQ, &enable, sizeof(enable));
        if (res < 0) {
            DEBUG("gnrc_netif: enable NETOPT_TX_END_IRQ failed: %d\n", res);
        }
    }
}

static void _init_from_device(gnrc_netif_t *netif)
{
    int res;
    netdev_t *dev = netif->context;
    uint16_t tmp;

    res = dev->driver->get(dev, NETOPT_DEVICE_TYPE, &tmp, sizeof(tmp));
    (void)res;
    assert(res == sizeof(tmp));
    netif->device_type = (uint8_t)tmp;
    gnrc_netif_ipv6_init_mtu(netif);
    _update_l2addr_from_dev(netif);
}

static void _update_l2addr_from_dev(gnrc_netif_t *netif)
{
    netdev_t *dev = netif->context;
    int res;
    netopt_t opt = gnrc_netif_get_l2addr_opt(netif);

    res = dev->driver->get(dev, opt, netif->l2addr,
                           sizeof(netif->l2addr));
    if (res != -ENOTSUP) {
        netif->flags |= GNRC_NETIF_FLAGS_HAS_L2ADDR;
    }
    else {
        /* If no address is provided but still an address length given above,
         * we are in an invalid state */
        assert(netif->l2addr_len == 0);
    }
    if (res > 0) {
        netif->l2addr_len = res;
    }
}

bool gnrc_netif_dev_is_6lo(const gnrc_netif_t *netif)
{
    switch (netif->device_type) {
#ifdef MODULE_GNRC_SIXLOENC
        case NETDEV_TYPE_ETHERNET:
            return (netif->flags & GNRC_NETIF_FLAGS_6LO);
#endif
        case NETDEV_TYPE_IEEE802154:
        case NETDEV_TYPE_CC110X:
        case NETDEV_TYPE_BLE:
        case NETDEV_TYPE_NRFMIN:
        case NETDEV_TYPE_ESP_NOW:
            return true;
        default:
            return false;
    }
}

#ifdef DEVELHELP
bool options_tested = false;

/* checks if a device supports all required options and functions */
static void _test_options(gnrc_netif_t *netif)
{
    uint8_t dummy_addr[GNRC_NETIF_L2ADDR_MAXLEN] = { 0 };
    ndp_opt_t dummy_opt = { .len = 1U };
    uint64_t tmp64 = 0ULL;
    netdev_t *dev = netif->context;

    (void)dummy_addr;
    (void)dummy_opt;
    (void)tmp64;
    (void)dev;
#if (GNRC_NETIF_L2ADDR_MAXLEN > 0)
    /* check if address was set in _update_l2addr_from_dev()
     * (NETOPT_DEVICE_TYPE already tested in _configure_netdev()) and
     * if MTU and max. fragment size was set properly by
     * gnrc_netif_ipv6_init_mtu()
     * all checked types below have link-layer addresses so we don't need to
     * check `GNRC_NETIF_FLAGS_HAS_L2ADDR` */
    switch (netif->device_type) {
#ifdef TEST_SUITES
        case NETDEV_TYPE_TEST:
            /* make no assumptions about test devices */
            break;
#endif
        case NETDEV_TYPE_BLE:
        case NETDEV_TYPE_ETHERNET:
        case NETDEV_TYPE_ESP_NOW:
            assert(netif->flags & GNRC_NETIF_FLAGS_HAS_L2ADDR);
            assert(ETHERNET_ADDR_LEN == netif->l2addr_len);
#if IS_USED(MODULE_GNRC_NETIF_IPV6)
            switch (netif->device_type) {
                case NETDEV_TYPE_BLE:
                    assert(netif->ipv6.mtu == IPV6_MIN_MTU);
                    break;
                case NETDEV_TYPE_ETHERNET:
                    assert(netif->ipv6.mtu == ETHERNET_DATA_LEN);
                    break;
                case NETDEV_TYPE_ESP_NOW:
                    assert(netif->ipv6.mtu <= ETHERNET_DATA_LEN);
            }
#endif  /* IS_USED(MODULE GNRC_NETIF_IPV6) */
            break;
        case NETDEV_TYPE_IEEE802154:
        case NETDEV_TYPE_NRFMIN: {
            gnrc_nettype_t tmp;

            /* in case assert() evaluates to NOP */
            (void)tmp;
            assert(netif->flags & GNRC_NETIF_FLAGS_HAS_L2ADDR);
            assert((IEEE802154_SHORT_ADDRESS_LEN == netif->l2addr_len) ||
                   (IEEE802154_LONG_ADDRESS_LEN == netif->l2addr_len));
#if IS_USED(MODULE_GNRC_NETIF_IPV6)
#if IS_USED(MODULE_GNRC_NETIF_6LO)
#if IS_ACTIVE(CONFIG_GNRC_NETIF_NONSTANDARD_6LO_MTU)
            assert(netif->ipv6.mtu >= IPV6_MIN_MTU);
#else /* IS_ACTIVE(CONFIG_GNRC_NETIF_NONSTANDARD_6LO_MTU) */
            assert(netif->ipv6.mtu == IPV6_MIN_MTU);
#endif /* IS_ACTIVE(CONFIG_GNRC_NETIF_NONSTANDARD_6LO_MTU) */
            assert(-ENOTSUP != dev->driver->get(dev, NETOPT_PROTO,
                                                &tmp, sizeof(tmp)));
#else   /* IS_USED(MODULE_GNRC_NETIF_6LO) */
            assert(netif->ipv6.mtu < UINT16_MAX);
#endif  /* IS_USED(MODULE_GNRC_NETIF_6LO) */
#endif  /* IS_USED(MODULE_GNRC_NETIF_IPV6) */
#ifdef MODULE_GNRC_SIXLOWPAN_ND
            assert((netif->device_type != NETDEV_TYPE_IEEE802154) ||
                   (-ENOTSUP != dev->driver->get(dev,
                                                        NETOPT_ADDRESS_LONG,
                                                        &dummy_addr,
                                                        sizeof(dummy_addr))));
#endif  /* MODULE_GNRC_SIXLOWPAN_ND */
            break;
        }
        case NETDEV_TYPE_CC110X:
            assert(netif->flags & GNRC_NETIF_FLAGS_HAS_L2ADDR);
            assert(1U == netif->l2addr_len);
#if IS_USED(MODULE_GNRC_NETIF_IPV6)
            assert(netif->ipv6.mtu < UINT16_MAX);
#endif  /* IS_USED(MODULE_GNRC_NETIF_IPV6) */
            break;
        case NETDEV_TYPE_LORA: /* LoRa doesn't provide L2 ADDR */
        case NETDEV_TYPE_SLIP:
            assert(!(netif->flags & GNRC_NETIF_FLAGS_HAS_L2ADDR));
            assert(0U == netif->l2addr_len);
            /* don't check MTU here for now since I'm not sure the current
             * one is correct ^^" "*/
            break;
        default:
            /* device type not supported yet, please amend case above when
             * porting new device type */
            assert(false);
    }
    /* These functions only apply to network devices having link-layers */
    if (netif->flags & GNRC_NETIF_FLAGS_HAS_L2ADDR) {
#if IS_USED(MODULE_GNRC_NETIF_IPV6)
        assert(-ENOTSUP != gnrc_netif_ipv6_get_iid(netif, (eui64_t *)&tmp64));
        assert(-ENOTSUP != gnrc_netif_ndp_addr_len_from_l2ao(netif,
                                                             &dummy_opt));
#endif  /* IS_USED(MODULE_GNRC_NETIF_IPV6) */
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_6LN)
        assert(-ENOTSUP != gnrc_netif_ipv6_iid_to_addr(netif, (eui64_t *)&tmp64,
                                                       dummy_addr));
#endif  /* CONFIG_GNRC_IPV6_NIB_6LN */
    }
#endif /* (GNRC_NETIF_L2ADDR_MAXLEN > 0) */
    options_tested = true;
}
#endif /* DEVELHELP */

#if IS_USED(MODULE_GNRC_NETIF_EVENTS)
/**
 * @brief   Call the ISR handler from an event
 *
 * @param[in]   evp     pointer to the event
 */
static void _event_handler_isr(event_t *evp)
{
    gnrc_netif_t *netif = container_of(evp, gnrc_netif_t, event_isr);
    netif->dev->driver->isr(netif->dev);
}
#endif

int gnrc_netif_default_init_netdev(gnrc_netif_t *netif)
{
    gnrc_netif_acquire(netif);
    netdev_t *dev = netif->context;
    netif->pid = sched_active_pid;

#if IS_USED(MODULE_GNRC_NETIF_EVENTS)
    netif->event_isr.handler = _event_handler_isr,
#endif /* MODULE_GNRC_NETIF_EVENTS */

    /* register the event callback with the device driver */
    dev->event_callback = _event_cb;
    dev->context = netif;
    /* initialize low-level driver */
    int res = dev->driver->init(dev);
    if (res < 0) {
        LOG_ERROR("gnrc_netif: netdev init failed: %d\n", res);
        /* unregister this netif instance */
        dev->event_callback = NULL;
        dev->context = NULL;
        return -EINVAL;
    }
    _configure_netdev(dev);
    _init_from_device(netif);
#ifdef DEVELHELP
    _test_options(netif);
#endif
    netif->cur_hl = CONFIG_GNRC_NETIF_DEFAULT_HL;
#ifdef MODULE_GNRC_IPV6_NIB
    gnrc_ipv6_nib_init_iface(netif);
#endif
#ifdef MODULE_NETSTATS_L2
    memset(&netif->stats, 0, sizeof(netstats_t));
#endif
    /* now let rest of GNRC use the interface */
    gnrc_netif_release(netif);
    return 0;
}


void gnrc_netif_msg_handler_netdev(gnrc_netif_t *netif, msg_t *msg)
{
    netdev_t *dev = netif->context;
    switch(msg->type) {
        case NETDEV_MSG_TYPE_EVENT:
            DEBUG("gnrc_netif: GNRC_NETDEV_MSG_TYPE_EVENT received\n");
            dev->driver->isr(dev);
            break;
        default:
            break;
    }
}

static void _pass_on_packet(gnrc_pktsnip_t *pkt)
{
    /* throw away packet if no one is interested */
    if (!gnrc_netapi_dispatch_receive(pkt->type, GNRC_NETREG_DEMUX_CTX_ALL, pkt)) {
        DEBUG("gnrc_netif: unable to forward packet of type %i\n", pkt->type);
        gnrc_pktbuf_release(pkt);
        return;
    }
}

static void _event_cb(netdev_t *dev, netdev_event_t event)
{
    gnrc_netif_t *netif = (gnrc_netif_t *) dev->context;

    if (event == NETDEV_EVENT_ISR) {
        if (IS_USED(MODULE_GNRC_NETIF_EVENTS)) {
            gnrc_netif_event_isr(netif);
        }
        else {
            msg_t msg = { .type = NETDEV_MSG_TYPE_EVENT,
                          .content = { .ptr = netif } };

            if (msg_send(&msg, netif->pid) <= 0) {
                puts("gnrc_netif: possibly lost interrupt.");
            }
        }
    }
    else {
        DEBUG("gnrc_netif: event triggered -> %i\n", event);
        gnrc_pktsnip_t *pkt = NULL;
        switch (event) {
            case NETDEV_EVENT_RX_COMPLETE:
                pkt = netif->ops->recv(netif);
                /* send packet previously queued within netif due to the lower
                 * layer being busy.
                 * Further packets will be sent on later TX_COMPLETE */
                gnrc_netif_send_queued_pkt(netif);
                if (pkt) {
                    _pass_on_packet(pkt);
                }
                break;
#if IS_USED(MODULE_NETSTATS_L2) || IS_USED(MODULE_GNRC_NETIF_PKTQ)
            case NETDEV_EVENT_TX_COMPLETE:
                /* send packet previously queued within netif due to the lower
                 * layer being busy.
                 * Further packets will be sent on later TX_COMPLETE or
                 * TX_MEDIUM_BUSY */
                gnrc_netif_send_queued_pkt(netif);
#if IS_USED(MODULE_NETSTATS_L2)
                /* we are the only ones supposed to touch this variable,
                 * so no acquire necessary */
                netif->stats.tx_success++;
#endif  /* IS_USED(MODULE_NETSTATS_L2) */
                break;
#endif  /* IS_USED(MODULE_NETSTATS_L2) || IS_USED(MODULE_GNRC_NETIF_PKTQ) */
#if IS_USED(MODULE_NETSTATS_L2) || IS_USED(MODULE_GNRC_NETIF_PKTQ)
            case NETDEV_EVENT_TX_MEDIUM_BUSY:
                /* send packet previously queued within netif due to the lower
                 * layer being busy.
                 * Further packets will be sent on later TX_COMPLETE or
                 * TX_MEDIUM_BUSY */
                gnrc_netif_send_queued_pkt(netif);
#if IS_USED(MODULE_NETSTATS_L2)
                /* we are the only ones supposed to touch this variable,
                 * so no acquire necessary */
                netif->stats.tx_failed++;
#endif  /* IS_USED(MODULE_NETSTATS_L2) */
                break;
#endif  /* IS_USED(MODULE_NETSTATS_L2) || IS_USED(MODULE_GNRC_NETIF_PKTQ) */
            default:
                DEBUG("gnrc_netif: warning: unhandled event %u.\n", event);
        }
    }
}
