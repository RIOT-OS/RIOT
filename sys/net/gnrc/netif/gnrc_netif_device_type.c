/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief   Functions that are based around a network interface's device type.
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <assert.h>
#include <errno.h>
#include <kernel_defines.h>

#include "log.h"
#include "macros/utils.h"
#include "net/ethernet.h"
#include "net/eui48.h"
#include "net/gnrc/netif.h"
#include "net/ieee802154.h"
#include "net/l2util.h"

#if IS_USED(MODULE_GNRC_NETIF_IPV6)
#include "net/ipv6.h"
#endif

#if IS_USED(MODULE_GNRC_NETIF_6LO)
#include "net/sixlowpan.h"
#endif

netopt_t gnrc_netif_get_l2addr_opt(const gnrc_netif_t *netif)
{
    netopt_t res = NETOPT_ADDRESS;

    switch (netif->device_type) {
#if defined(MODULE_NETDEV_IEEE802154) || defined(MODULE_XBEE) || \
    defined(MODULE_NIMBLE_NETIF)
        case NETDEV_TYPE_IEEE802154:
        case NETDEV_TYPE_BLE: {
                netdev_t *dev = netif->dev;
                int r;
                uint16_t tmp;

                r = dev->driver->get(dev, NETOPT_SRC_LEN, &tmp, sizeof(tmp));
                assert(r == sizeof(tmp));
                assert(r <= ((int)UINT8_MAX));
                (void)r;
                if (tmp == IEEE802154_LONG_ADDRESS_LEN) {
                    res = NETOPT_ADDRESS_LONG;
                }
            }
            break;
#endif
#if defined(MODULE_SLIPMUX_NET_L2ADDR)
        case NETDEV_TYPE_SLIP:
            res = NETOPT_ADDRESS_LONG;
            break;
#endif
        default:
            break;
    }
    return res;
}

int gnrc_netif_eui64_from_addr(const gnrc_netif_t *netif,
                               const uint8_t *addr, size_t addr_len,
                               eui64_t *eui64)
{
#if GNRC_NETIF_L2ADDR_MAXLEN > 0
    if (netif->flags & GNRC_NETIF_FLAGS_HAS_L2ADDR) {
        switch (netif->device_type) {
#if defined(MODULE_NETDEV_IEEE802154) || defined(MODULE_XBEE)
            case NETDEV_TYPE_IEEE802154:
                /* try getting EUI-64 from device if short address is
                 * provided */
                switch (addr_len) {
                    case IEEE802154_SHORT_ADDRESS_LEN: {
                        netdev_t *dev = netif->dev;
                        return dev->driver->get(dev, NETOPT_ADDRESS_LONG, eui64,
                                                sizeof(eui64_t));
                    }
                    default:
                        break;
                }
#endif  /* defined(MODULE_NETDEV_IEEE802154) || defined(MODULE_XBEE) */
                /* Intentionally falls through */
            default:
                return l2util_eui64_from_addr(netif->device_type, addr,
                                              addr_len, eui64);
        }
    }
#endif /* GNRC_NETIF_L2ADDR_MAXLEN > 0 */
    return -ENOTSUP;
}

void gnrc_netif_init_6ln(gnrc_netif_t *netif)
{
    switch (netif->device_type) {
        case NETDEV_TYPE_IEEE802154: {
            /* see https://tools.ietf.org/html/rfc6775#section-5.2 */
            uint16_t src_len = IEEE802154_LONG_ADDRESS_LEN;
            gnrc_netapi_opt_t opt = { .opt = NETOPT_SRC_LEN,
                                      .data = &src_len,
                                      .data_len = sizeof(src_len) };

            /* XXX we are supposed to be in interface context here, so use driver
             * directly everything else would deadlock anyway */
            netif->ops->set(netif, &opt);
        }
        /* intentionally falls through */
        case NETDEV_TYPE_BLE:
#ifdef MODULE_GNRC_SIXLOENC
        case NETDEV_TYPE_ETHERNET:
#endif
#ifdef MODULE_CC110X
        case NETDEV_TYPE_CC110X:
#endif
#ifdef MODULE_ESP_NOW
        case NETDEV_TYPE_ESP_NOW:
#endif
#ifdef MODULE_NRF24L01P_NG
        case NETDEV_TYPE_NRF24L01P_NG:
#endif
        case NETDEV_TYPE_NRFMIN:
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_6LN)
            netif->flags |= GNRC_NETIF_FLAGS_6LN;
#endif  /* CONFIG_GNRC_IPV6_NIB_6LN */
            /* intentionally falls through */
        default:
            break;
    }
}

#if IS_USED(MODULE_GNRC_NETIF_IPV6)
void gnrc_netif_ipv6_init_mtu(gnrc_netif_t *netif)
{
    netdev_t *dev = netif->dev;
    int res;
    uint16_t tmp;

    switch (netif->device_type) {
#if defined(MODULE_NETDEV_IEEE802154) || defined(MODULE_NRFMIN) || \
    defined(MODULE_XBEE) || defined(MODULE_ESP_NOW) || \
    defined(MODULE_GNRC_SIXLOENC) || defined(MODULE_CC110X) || \
    defined(MODULE_NRF24L01P_NG)
        case NETDEV_TYPE_IEEE802154:
        case NETDEV_TYPE_NRFMIN:
        case NETDEV_TYPE_CC110X:
        case NETDEV_TYPE_NRF24L01P_NG:
#ifdef MODULE_GNRC_SIXLOWPAN_IPHC
            netif->flags |= GNRC_NETIF_FLAGS_6LO_HC;
#endif
            /* intentionally falls through */
        case NETDEV_TYPE_ESP_NOW:
            res = dev->driver->get(dev, NETOPT_MAX_PDU_SIZE,
                                   &tmp, sizeof(tmp));
            assert(res == sizeof(tmp));
#if IS_USED(MODULE_GNRC_NETIF_6LO)
#if IS_ACTIVE(CONFIG_GNRC_NETIF_NONSTANDARD_6LO_MTU)
            netif->ipv6.mtu = MAX(IPV6_MIN_MTU, tmp);
#else /* IS_ACTIVE(CONFIG_GNRC_NETIF_NONSTANDARD_6LO_MTU) */
            netif->ipv6.mtu = IPV6_MIN_MTU;
#endif /* IS_ACTIVE(CONFIG_GNRC_NETIF_NONSTANDARD_6LO_MTU) */
            if (tmp >= netif->ipv6.mtu) {
                /* When the L2-PDU is higher or equal to the IPv6 MTU, disable
                 * 6Lo fragmentation, this generally applies to 802.15.4g
                 * devices with a big L2-PDU */
                netif->sixlo.max_frag_size = 0;
            }
            else {
                netif->sixlo.max_frag_size = MIN(SIXLOWPAN_FRAG_MAX_LEN, tmp);
            }
#else   /* IS_USED(MODULE_GNRC_NETIF_6LO) */
            netif->ipv6.mtu = tmp;
#endif  /* IS_USED(MODULE_GNRC_NETIF_6LO) */
            break;
#endif  /* defined(MODULE_NETDEV_IEEE802154) || defined(MODULE_NRFMIN) || \
         * defined(MODULE_XBEE) || defined(MODULE_ESP_NOW) */
#ifdef MODULE_NETDEV_ETH
        case NETDEV_TYPE_ETHERNET:
            netif->ipv6.mtu = ETHERNET_DATA_LEN;
#ifdef MODULE_GNRC_SIXLOENC
            netif->flags |= GNRC_NETIF_FLAGS_6LO;
#ifdef MODULE_GNRC_SIXLOWPAN_IPHC
            netif->flags |= GNRC_NETIF_FLAGS_6LO_HC;
#endif
#endif
            break;
#endif
#if defined(MODULE_NIMBLE_NETIF)
        case NETDEV_TYPE_BLE:
            netif->ipv6.mtu = IPV6_MIN_MTU;
#ifdef MODULE_GNRC_SIXLOWPAN_IPHC
            netif->flags |= GNRC_NETIF_FLAGS_6LO_HC;
#endif
            break;
#endif
        default:
#ifdef DEVELHELP
            LOG_DEBUG("gnrc_netif: getting MTU from device for interface %i\n",
                      netif->pid);
#endif
            res = dev->driver->get(dev, NETOPT_MAX_PDU_SIZE,
                                   &tmp, sizeof(tmp));
            if (res < 0) {
                /* assume maximum possible transition unit */
                netif->ipv6.mtu = UINT16_MAX;
            }
            else {
                netif->ipv6.mtu = tmp;
            }
            break;
    }
}

int gnrc_netif_ipv6_iid_from_addr(const gnrc_netif_t *netif,
                                  const uint8_t *addr, size_t addr_len,
                                  eui64_t *iid)
{
#if GNRC_NETIF_L2ADDR_MAXLEN > 0
    if (netif->flags & GNRC_NETIF_FLAGS_HAS_L2ADDR) {
        return l2util_ipv6_iid_from_addr(netif->device_type,
                                         addr, addr_len, iid);
    }
#endif /* GNRC_NETIF_L2ADDR_MAXLEN > 0 */
    return -ENOTSUP;
}

#endif /* IS_USED(MODULE_GNRC_NETIF_IPV6) */

/** @} */
