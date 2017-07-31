/*
 * Copyright (C) 2017 Freie Universität Berlin
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

#include "net/gnrc/netif2/internal.h"
#include "net/gnrc/ipv6/nib.h"

#include "_nib-6ln.h"
#include "_nib-6lr.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if GNRC_IPV6_NIB_CONF_6LN
#if ENABLE_DEBUG
static char addr_str[IPV6_ADDR_MAX_STR_LEN];
#endif

static inline bool _is_iface_eui64(gnrc_netif2_t *netif, const eui64_t *eui64)
{
    /* TODO: adapt for short addresses */
    return (netif->l2addr_len == sizeof(eui64_t)) &&
            (memcmp(&netif->l2addr, eui64, netif->l2addr_len) == 0);
}

static inline uint8_t _reverse_iid(const ipv6_addr_t *dst,
                                   const gnrc_netif2_t *netif, uint8_t *l2addr)
{
    switch (netif->device_type) {
#ifdef MODULE_NETDEV_ETH
        case NETDEV_TYPE_ETHERNET:
            l2addr[0] = dst->u8[8] ^ 0x02;
            l2addr[1] = dst->u8[9];
            l2addr[2] = dst->u8[10];
            l2addr[3] = dst->u8[13];
            l2addr[4] = dst->u8[14];
            l2addr[5] = dst->u8[15];
            return ETHERNET_ADDR_LEN;
#endif
#ifdef MODULE_NETDEV_IEEE802154
        case NETDEV_TYPE_IEEE802154:
            /* assume address was based on EUI-64
             * (see https://tools.ietf.org/html/rfc6775#section-5.2) */
            memcpy(l2addr, &dst->u64[1], sizeof(dst->u64[1]));
            l2addr[0] ^= 0x02;
            return sizeof(dst->u64[1]);
#endif
#ifdef MODULE_CC110X
        case NETDEV_TYPE_CC110X:
            l2addr[0] = dst->u8[15];
            return sizeof(uint8_t);
#endif
        default:
            (void)dst;
            (void)l2addr;
            return 0;
    }
}

bool _resolve_addr_from_ipv6(const ipv6_addr_t *dst, gnrc_netif2_t *netif,
                             gnrc_ipv6_nib_nc_t *nce)
{
    bool res = (netif != NULL) && gnrc_netif2_is_6ln(netif) &&
               ipv6_addr_is_link_local(dst);

    if (res) {
        uint8_t l2addr_len;

        if ((l2addr_len = _reverse_iid(dst, netif, nce->l2addr)) > 0) {
            DEBUG("nib: resolve address %s%%%u by reverse translating to ",
                  ipv6_addr_to_str(addr_str, dst, sizeof(addr_str)),
                  (unsigned)netif->pid);
            nce->l2addr_len = l2addr_len;
            DEBUG("%s\n",
                  gnrc_netif2_addr_to_str(nce->l2addr, nce->l2addr_len,
                                          addr_str));
            memcpy(&nce->ipv6, dst, sizeof(nce->ipv6));
            nce->info = 0;
            nce->info |= (netif->pid << GNRC_IPV6_NIB_NC_INFO_IFACE_POS) &
                         GNRC_IPV6_NIB_NC_INFO_IFACE_MASK;
            nce->info |= GNRC_IPV6_NIB_NC_INFO_NUD_STATE_REACHABLE;
            nce->info |= GNRC_IPV6_NIB_NC_INFO_AR_STATE_REGISTERED;
        }
        else {
            res = false;
        }
    }
    return res;
}

uint8_t _handle_aro(gnrc_netif2_t *netif, const ipv6_hdr_t *ipv6,
                    const icmpv6_hdr_t *icmpv6,
                    const sixlowpan_nd_opt_ar_t *aro, const ndp_opt_t *sl2ao,
                    _nib_onl_entry_t *nce)
{
#if !GNRC_IPV6_NIB_CONF_6LR
    (void)sl2ao;
#endif
    assert(netif != NULL);
    if (gnrc_netif2_is_6ln(netif) && (aro->len == SIXLOWPAN_ND_OPT_AR_LEN)) {
        DEBUG("nib: valid ARO received\n");
        DEBUG(" - length: %u\n", aro->len);
        DEBUG(" - status: %u\n", aro->status);
        DEBUG(" - registration lifetime: %u\n", byteorder_ntohs(aro->ltime));
        DEBUG(" - EUI-64: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
              aro->eui64.uint8[0], aro->eui64.uint8[1], aro->eui64.uint8[2],
              aro->eui64.uint8[3], aro->eui64.uint8[4], aro->eui64.uint8[5],
              aro->eui64.uint8[6], aro->eui64.uint8[7]);
        if (icmpv6->type == ICMPV6_NBR_ADV) {
            if (!_is_iface_eui64(netif, &aro->eui64)) {
                DEBUG("nib: ARO EUI-64 is not mine, ignoring ARO\n");
                return _ADDR_REG_STATUS_IGNORE;
            }
            switch (aro->status) {
                case SIXLOWPAN_ND_STATUS_SUCCESS: {
                    uint16_t ltime = byteorder_ntohs(aro->ltime);
                    uint32_t next_ns;
                    /* if ltime 1min, reschedule NS in 30sec, otherwise 1min
                     * before timeout */
                    next_ns = (ltime == 1U) ? (30 * MS_PER_SEC) :
                                (byteorder_ntohs(aro->ltime) - 1U) *
                                SEC_PER_MIN * MS_PER_SEC;
                    DEBUG("nib: Address registration successful. "
                               "Scheduling re-registration in %" PRIu32 "ms\n",
                          next_ns);
                    assert(nce != NULL);
                    _evtimer_add(nce, GNRC_IPV6_NIB_SND_UC_NS, &nce->nud_timeout,
                                 next_ns);
                    break;
                }
                case SIXLOWPAN_ND_STATUS_DUP:
                    DEBUG("nib: Address registration reports duplicate. "
                               "Removing address %s%%%u\n",
                          ipv6_addr_to_str(addr_str,
                                           &ipv6->dst,
                                           sizeof(addr_str)), netif->pid);
                    gnrc_netif2_ipv6_addr_remove(netif, &ipv6->dst);
                    /* TODO: generate new address */
                    break;
                case SIXLOWPAN_ND_STATUS_NC_FULL: {
                        DEBUG("nib: Router's neighbor cache is full. "
                                   "Searching new router for DAD\n");
                        _nib_dr_entry_t *dr = _nib_drl_get(&ipv6->src, netif->pid);
                        assert(dr != NULL); /* otherwise we wouldn't be here */
                        _nib_drl_remove(dr);
                        if (_nib_drl_iter(NULL) == NULL) { /* no DRL left */
                            netif->ipv6.rs_sent = 0;
                            /* TODO: search new router */
                        }
                        else {
                            assert(dr->next_hop != NULL);
                            _snd_uc_ns(dr->next_hop, true);
                        }
                    }
                    break;
            }
            return aro->status;
        }
#if GNRC_IPV6_NIB_CONF_6LR
        else if (gnrc_netif2_is_6lr(netif) &&
                 (icmpv6->type == ICMPV6_NBR_SOL)) {
            return _reg_addr_upstream(netif, ipv6, icmpv6, aro, sl2ao);
        }
#endif
    }
#if ENABLE_DEBUG
    else if (aro->len != SIXLOWPAN_ND_OPT_AR_LEN) {
        DEBUG("nib: ARO of unexpected length %u, ignoring ARO\n", aro->len);
    }
#endif
    return _ADDR_REG_STATUS_IGNORE;
}
#else  /* GNRC_IPV6_NIB_CONF_6LN */
typedef int dont_be_pedantic;
#endif /* GNRC_IPV6_NIB_CONF_6LN */


/** @} */
