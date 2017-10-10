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

#include "net/gnrc/ipv6/nib.h"

#include "_nib-6ln.h"
#include "_nib-6lr.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if GNRC_IPV6_NIB_CONF_6LN
#if ENABLE_DEBUG
static char addr_str[IPV6_ADDR_MAX_STR_LEN];
#endif

static bool _is_iface_eui64(kernel_pid_t iface, const eui64_t *eui64)
{
    eui64_t iface_eui64;

    /* XXX: this *should* return successful so don't test it ;-) */
    gnrc_netapi_get(iface, NETOPT_ADDRESS_LONG, 0,
                    &iface_eui64, sizeof(iface_eui64));
    return (memcmp(&iface_eui64, eui64, sizeof(iface_eui64)) != 0);
}

bool _resolve_addr_from_ipv6(const ipv6_addr_t *dst, kernel_pid_t iface,
                             gnrc_ipv6_nib_nc_t *nce)
{
    gnrc_ipv6_netif_t *netif = gnrc_ipv6_netif_get(iface);
    bool res = (netif != NULL) && _is_6ln(netif) &&
               ipv6_addr_is_link_local(dst);

    if (res) {
        memcpy(&nce->ipv6, dst, sizeof(nce->ipv6));
        memcpy(&nce->l2addr, &dst->u64[1], sizeof(dst->u64[1]));
        nce->l2addr[0] ^= 0x02;
        nce->info = 0;
        nce->info |= (iface << GNRC_IPV6_NIB_NC_INFO_IFACE_POS) &
                     GNRC_IPV6_NIB_NC_INFO_IFACE_MASK;
        nce->info |= GNRC_IPV6_NIB_NC_INFO_NUD_STATE_REACHABLE;
        nce->info |= GNRC_IPV6_NIB_NC_INFO_AR_STATE_REGISTERED;
        nce->l2addr_len = sizeof(dst->u64[1]);
    }
    return res;
}

uint8_t _handle_aro(kernel_pid_t iface, const ipv6_hdr_t *ipv6,
                    const icmpv6_hdr_t *icmpv6,
                    const sixlowpan_nd_opt_ar_t *aro, const ndp_opt_t *sl2ao,
                    _nib_onl_entry_t *nce)
{
    gnrc_ipv6_netif_t *netif = gnrc_ipv6_netif_get(iface);

#if !GNRC_IPV6_NIB_CONF_6LR
    (void)sl2ao;
#endif
    assert(netif != NULL);
    if (_is_6ln(netif) && (aro->len == SIXLOWPAN_ND_OPT_AR_LEN)) {
        DEBUG("nib: valid ARO received\n");
        DEBUG(" - length: %u\n", aro->len);
        DEBUG(" - status: %u\n", aro->status);
        DEBUG(" - registration lifetime: %u\n", byteorder_ntohs(aro->ltime));
        DEBUG(" - EUI-64: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
              aro->eui64.uint8[0], aro->eui64.uint8[1], aro->eui64.uint8[2],
              aro->eui64.uint8[3], aro->eui64.uint8[4], aro->eui64.uint8[5],
              aro->eui64.uint8[6], aro->eui64.uint8[7]);
        if (icmpv6->type == ICMPV6_NBR_ADV) {
            if (!_is_iface_eui64(iface, &aro->eui64)) {
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
                               "Scheduling re-registration in %ums\n",
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
                                           &((ndp_nbr_adv_t *)icmpv6)->tgt,
                                           sizeof(addr_str)),
                          iface);
                    gnrc_ipv6_netif_remove_addr(iface,
                                                &((ndp_nbr_adv_t *)icmpv6)->tgt);
                    /* TODO: generate new address */
                    break;
                case SIXLOWPAN_ND_STATUS_NC_FULL: {
                        DEBUG("nib: Router's neighbor cache is full. "
                                   "Searching new router for DAD\n");
                        _nib_dr_entry_t *dr = _nib_drl_get(&ipv6->src, iface);
                        assert(dr != NULL); /* otherwise we wouldn't be here */
                        _nib_drl_remove(dr);
                        if (_nib_drl_iter(NULL) == NULL) { /* no DRL left */
                            _nib_iface_t *nib_iface = _nib_iface_get(iface);
                            nib_iface->rs_sent = 0;
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
        else if (_is_6lr(netif) && (icmpv6->type == ICMPV6_NBR_SOL)) {
            return _reg_addr_upstream(iface, ipv6, icmpv6, aro, sl2ao);
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
