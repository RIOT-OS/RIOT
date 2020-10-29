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
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <assert.h>

#include "log.h"
#include "net/eui48.h"
#include "net/ieee802154.h"
#include "net/ipv6.h"
#include "net/netdev.h"

#include "net/l2util.h"

#if defined(MODULE_CC110X) || defined(MODULE_NRFMIN)
static void _create_eui64_from_short(const uint8_t *addr, size_t addr_len,
                                     eui64_t *eui64)
{
    const unsigned offset = sizeof(eui64_t) - addr_len;

    memset(eui64->uint8, 0, sizeof(eui64->uint8));
    eui64->uint8[3] = 0xff;
    eui64->uint8[4] = 0xfe;
    memcpy(&eui64->uint8[offset], addr, addr_len);
}
#endif /* defined(MODULE_CC110X) || defined(MODULE_NRFMIN) */

int l2util_eui64_from_addr(int dev_type, const uint8_t *addr, size_t addr_len,
                           eui64_t *eui64)
{
    switch (dev_type) {
#if defined(MODULE_NETDEV_ETH) || defined(MODULE_ESP_NOW) || \
    defined(MODULE_NIMBLE_NETIF)
        case NETDEV_TYPE_ETHERNET:
        case NETDEV_TYPE_ESP_NOW:
        case NETDEV_TYPE_BLE:
            if (addr_len == sizeof(eui48_t)) {
                eui48_to_eui64(eui64, (const eui48_t *)addr);
                return sizeof(eui64_t);
            }
            else {
                return -EINVAL;
            }
#endif  /* defined(MODULE_NETDEV_ETH) || defined(MODULE_ESP_NOW) \
           defined(MODULE_NIMBLE_NETIF) */
#if defined(MODULE_NETDEV_IEEE802154) || defined(MODULE_XBEE)
        case NETDEV_TYPE_IEEE802154:
            switch (addr_len) {
                /* EUI-64 can *not* be generated from the short address */
                case IEEE802154_LONG_ADDRESS_LEN:
                    memcpy(eui64, addr, addr_len);
                    return sizeof(eui64_t);
                default:
                    return -EINVAL;
            }
#endif  /* defined(MODULE_NETDEV_IEEE802154) || defined(MODULE_XBEE) */
#if defined(MODULE_CC110X) || defined(MODULE_NRFMIN)
        case NETDEV_TYPE_CC110X:
        case NETDEV_TYPE_NRFMIN:
            if (addr_len <= 3) {
                _create_eui64_from_short(addr, addr_len, eui64);
                return sizeof(eui64_t);
            }
            else {
                return -EINVAL;
            }
#endif  /* defined(MODULE_CC110X) || defined(MODULE_NRFMIN) */
        default:
            (void)addr;
            (void)addr_len;
            (void)eui64;
#ifdef DEVELHELP
            LOG_ERROR("l2util: can't convert hardware address to EUI-64 "
                      "for device type %d\n", dev_type);
#endif  /* DEVELHELP */
            assert(false);
            break;
    }
    return -ENOTSUP;
}

int l2util_ipv6_iid_from_addr(int dev_type,
                              const uint8_t *addr, size_t addr_len,
                              eui64_t *iid)
{
    switch (dev_type) {
#if defined(MODULE_NETDEV_IEEE802154) || defined(MODULE_XBEE)
        case NETDEV_TYPE_IEEE802154:
            if (ieee802154_get_iid(iid, addr, addr_len) != NULL) {
                return sizeof(eui64_t);
            }
            else {
                return -EINVAL;
            }
#endif  /* defined(MODULE_NETDEV_IEEE802154) || defined(MODULE_XBEE) */
#if defined(MODULE_CC110X) || defined(MODULE_NRFMIN)
        case NETDEV_TYPE_CC110X:
        case NETDEV_TYPE_NRFMIN:
            if (addr_len <= 3) {
                _create_eui64_from_short(addr, addr_len, iid);
                /* since this address conversion is based on the IEEE
                 * 802.15.4 address conversion for short addresses, the
                 * U/L bit doesn't need to be flipped.
                 * see https://tools.ietf.org/html/rfc6282#section-3.2.2 */
                return sizeof(eui64_t);
            }
            else {
                return -EINVAL;
            }
#endif  /* defined(MODULE_CC110X) || defined(MODULE_NRFMIN) */
#if defined(MODULE_NIMBLE_NETIF)
        case NETDEV_TYPE_BLE:
            /* for BLE we don't flip the universal/local flag... */
            return l2util_eui64_from_addr(dev_type, addr, addr_len, iid);
#endif  /* defined(MODULE_NIMBLE_NETIF) */
        default: {
            int res = l2util_eui64_from_addr(dev_type, addr, addr_len, iid);
            if (res == sizeof(eui64_t)) {
                iid->uint8[0] ^= 0x02;
            }
            return res;
        }
    }
    return -ENOTSUP;
}

int l2util_ipv6_iid_to_addr(int dev_type, const eui64_t *iid, uint8_t *addr)
{
    switch (dev_type) {
#if defined(MODULE_NETDEV_ETH) || defined(MODULE_ESP_NOW)
        case NETDEV_TYPE_ETHERNET:
        case NETDEV_TYPE_ESP_NOW:
            eui48_from_ipv6_iid((eui48_t *)addr, iid);
            return sizeof(eui48_t);
#endif  /* defined(MODULE_NETDEV_ETH) || defined(MODULE_ESP_NOW) */
#if defined(MODULE_NIMBLE_NETIF)
        case NETDEV_TYPE_BLE:
            addr[0] = iid->uint8[0];
            addr[1] = iid->uint8[1];
            addr[2] = iid->uint8[2];
            addr[3] = iid->uint8[5];
            addr[4] = iid->uint8[6];
            addr[5] = iid->uint8[7];
            return sizeof(eui48_t);
#endif  /* defined(MODULE_NIMBLE_NETIF) */
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
#ifdef MODULE_CC110X
        case NETDEV_TYPE_CC110X:
            addr[0] = iid->uint8[7];
            return sizeof(uint8_t);
#endif  /* MODULE_CC110X */
        default:
            (void)iid;
            (void)addr;
#ifdef DEVELHELP
            LOG_ERROR("l2util: can't convert IID to hardware address for "
                      "device type %d\n", dev_type);
#endif  /* DEVELHELP */
            assert(false);
            break;
    }
    return -ENOTSUP;
}

int l2util_ndp_addr_len_from_l2ao(int dev_type,
                                  const ndp_opt_t *opt)
{
    switch (dev_type) {
#ifdef MODULE_CC110X
        case NETDEV_TYPE_CC110X:
            (void)opt;
            return sizeof(uint8_t);
#endif  /* MODULE_CC110X */
#if defined(MODULE_NETDEV_ETH) || defined(MODULE_ESP_NOW) || \
    defined(MODULE_NIMBLE_NETIF)
        case NETDEV_TYPE_ETHERNET:
        case NETDEV_TYPE_ESP_NOW:
        case NETDEV_TYPE_BLE:
            /* see https://tools.ietf.org/html/rfc2464#section-6*/
            if (opt->len == 1U) {
                return sizeof(eui48_t);
            }
            else {
                return -EINVAL;
            }
#endif  /* defined(MODULE_NETDEV_ETH) || defined(MODULE_ESP_NOW) \
           defined(MODULE_NIMBLE_NETIF) */
#ifdef MODULE_NRFMIN
        case NETDEV_TYPE_NRFMIN:
            (void)opt;
            return sizeof(uint16_t);
#endif  /* MODULE_NRFMIN */
#if defined(MODULE_NETDEV_IEEE802154) || defined(MODULE_XBEE)
        case NETDEV_TYPE_IEEE802154:
            /* see https://tools.ietf.org/html/rfc4944#section-8 */
            switch (opt->len) {
                case 1U:
                    return IEEE802154_SHORT_ADDRESS_LEN;
                case 2U:
                    return IEEE802154_LONG_ADDRESS_LEN;
                default:
                    return -EINVAL;
            }
#endif  /* defined(MODULE_NETDEV_IEEE802154) || defined(MODULE_XBEE) */
        default:
            (void)opt;
#ifdef DEVELHELP
            LOG_ERROR("l2util: can't get address length from NDP link-layer "
                      "address option for device type %d\n", dev_type);
#endif
            assert(false);
            break;
    }
    return -ENOTSUP;
}

int l2util_ipv6_group_to_l2_group(int dev_type,
                                  const ipv6_addr_t *ipv6_group,
                                  uint8_t *l2_group)
{
    switch (dev_type) {
#if IS_USED(MODULE_NETDEV_ETH)
        case NETDEV_TYPE_ETHERNET:
            /* see https://tools.ietf.org/html/rfc2464#section-7 */
            l2_group[0] = 0x33;
            l2_group[1] = 0x33;
            l2_group[2] = ipv6_group->u8[12];
            l2_group[3] = ipv6_group->u8[13];
            l2_group[4] = ipv6_group->u8[14];
            l2_group[5] = ipv6_group->u8[15];
            return sizeof(eui48_t);
#endif
        default:
            (void)ipv6_group;
            (void)l2_group;
            return -ENOTSUP;
    }
}

/** @} */
