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

#include <errno.h>

#include "log.h"
#include "net/gnrc/netif.h"
#include "net/eui48.h"
#include "net/ieee802154.h"

#ifdef MODULE_GNRC_IPV6
#if defined(MODULE_CC110X) || defined(MODULE_NRFMIN)
static void _create_iid_from_short(const uint8_t *addr, size_t addr_len,
                                   eui64_t *iid)
{
    const unsigned offset = sizeof(eui64_t) - addr_len;

    memset(iid->uint8, 0, sizeof(iid->uint8));
    iid->uint8[3] = 0xff;
    iid->uint8[4] = 0xfe;
    memcpy(&iid->uint8[offset], addr, addr_len);
}
#endif /* defined(MODULE_CC110X) || defined(MODULE_NRFMIN) */

int gnrc_netif_ipv6_iid_from_addr(const gnrc_netif_t *netif,
                                  const uint8_t *addr, size_t addr_len,
                                  eui64_t *iid)
{
#if GNRC_NETIF_L2ADDR_MAXLEN > 0
    if (netif->flags & GNRC_NETIF_FLAGS_HAS_L2ADDR) {
        switch (netif->device_type) {
#if defined(MODULE_NETDEV_ETH) || defined(MODULE_ESP_NOW)
            case NETDEV_TYPE_ETHERNET:
            case NETDEV_TYPE_ESP_NOW:
                if (addr_len == sizeof(eui48_t)) {
                    eui48_to_ipv6_iid(iid, (const eui48_t *)addr);
                    return sizeof(eui64_t);
                }
                else {
                    return -EINVAL;
                }
#endif  /* defined(MODULE_NETDEV_ETH) || defined(MODULE_ESP_NOW) */
#if defined(MODULE_NETDEV_IEEE802154) || defined(MODULE_XBEE)
            case NETDEV_TYPE_IEEE802154:
                if (ieee802154_get_iid(iid, addr, addr_len) != NULL) {
                    return sizeof(eui64_t);
                }
                else {
                    return -EINVAL;
                }
#endif  /* defined(MODULE_NETDEV_IEEE802154) || defined(MODULE_XBEE) */
#ifdef MODULE_NORDIC_SOFTDEVICE_BLE
            case NETDEV_TYPE_BLE:
                if (addr_len == sizeof(eui64_t)) {
                    memcpy(iid, addr, sizeof(eui64_t));
                    iid->uint8[0] ^= 0x02;
                    return sizeof(eui64_t);
                }
                else {
                    return -EINVAL;
                }
#endif  /* MODULE_NORDIC_SOFTDEVICE_BLE */
#if defined(MODULE_CC110X) || defined(MODULE_NRFMIN)
            case NETDEV_TYPE_CC110X:
            case NETDEV_TYPE_NRFMIN:
                if (addr_len <= 3) {
                    _create_iid_from_short(addr, addr_len, iid);
                    return sizeof(eui64_t);
                }
                else {
                    return -EINVAL;
                }
#endif  /* defined(MODULE_CC110X) || defined(MODULE_NRFMIN) */
            default:
                (void)addr;
                (void)addr_len;
                (void)iid;
#ifdef DEVELHELP
                LOG_ERROR("gnrc_netif: can't convert hardware address to IID "
                          "on interface %u\n", netif->pid);
#endif  /* DEVELHELP */
                assert(false);
                break;
        }
    }
#endif /* GNRC_NETIF_L2ADDR_MAXLEN > 0 */
    return -ENOTSUP;
}

int gnrc_netif_ipv6_iid_to_addr(const gnrc_netif_t *netif, const eui64_t *iid,
                                uint8_t *addr)
{
    assert(netif->flags & GNRC_NETIF_FLAGS_HAS_L2ADDR);
    switch (netif->device_type) {
#if defined(MODULE_NETDEV_ETH) || defined(MODULE_ESP_NOW)
        case NETDEV_TYPE_ETHERNET:
        case NETDEV_TYPE_ESP_NOW:
            eui48_from_ipv6_iid((eui48_t *)addr, iid);
            return sizeof(eui48_t);
#endif  /* defined(MODULE_NETDEV_ETH) || defined(MODULE_ESP_NOW) */
#if defined(MODULE_NETDEV_IEEE802154) || defined(MODULE_XBEE)
        case NETDEV_TYPE_IEEE802154:
            /* assume address was based on EUI-64
             * (see https://tools.ietf.org/html/rfc6775#section-5.2) */
            memcpy(addr, iid, sizeof(eui64_t));
            addr[0] ^= 0x02;
            return sizeof(eui64_t);
#endif  /* defined(MODULE_NETDEV_IEEE802154) || defined(MODULE_XBEE) */
#ifdef MODULE_NRFMIN
        case NETDEV_TYPE_NRFMIN:
            addr[0] = iid->uint8[6];
            addr[1] = iid->uint8[7];
            return sizeof(uint16_t);
#endif  /* MODULE_NETDEV_IEEE802154 */
#ifdef MODULE_NORDIC_SOFTDEVICE_BLE
        case NETDEV_TYPE_BLE:
            memcpy(addr, iid, sizeof(eui64_t));
            addr[0] ^= 0x02;
            return sizeof(eui64_t);
#endif  /* MODULE_NORDIC_SOFTDEVICE_BLE */
#ifdef MODULE_CC110X
        case NETDEV_TYPE_CC110X:
            addr[0] = iid->uint8[7];
            return sizeof(uint8_t);
#endif  /* MODULE_CC110X */
        default:
            (void)iid;
            (void)addr;
#ifdef DEVELHELP
            LOG_ERROR("gnrc_netif: can't convert IID to hardware address "
                      "on interface %u\n", netif->pid);
#endif  /* DEVELHELP */
            assert(false);
            break;
    }
    return -ENOTSUP;
}
#endif /* MODULE_GNRC_IPV6 */

/** @} */
