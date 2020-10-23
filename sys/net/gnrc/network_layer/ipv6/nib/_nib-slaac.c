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

#include <kernel_defines.h>
#include <stdbool.h>

#include "log.h"
#include "luid.h"
#include "net/gnrc/ipv6/nib.h"
#include "net/gnrc/netif/internal.h"

#include "_nib-6ln.h"
#include "_nib-arsm.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_6LN) || IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_SLAAC)
static char addr_str[IPV6_ADDR_MAX_STR_LEN];

void _auto_configure_addr(gnrc_netif_t *netif, const ipv6_addr_t *pfx,
                          uint8_t pfx_len)
{
    ipv6_addr_t addr = IPV6_ADDR_UNSPECIFIED;
    int idx;
    uint8_t flags = GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_TENTATIVE;

#if !IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_SLAAC)
    if (!gnrc_netif_is_6ln(netif)) {
        LOG_WARNING("SLAAC not activated; will not auto-configure IPv6 address "
                         "for interface %u.\n"
                    "    Use CONFIG_GNRC_IPV6_NIB_SLAAC=1 to activate.\n",
                    netif->pid);
        return;
    }
#endif
    if (!(netif->flags & GNRC_NETIF_FLAGS_HAS_L2ADDR)) {
        DEBUG("nib: interface %i has no link-layer addresses\n", netif->pid);
        return;
    }
    DEBUG("nib: add address based on %s/%u automatically to interface %u\n",
          ipv6_addr_to_str(addr_str, pfx, sizeof(addr_str)),
          pfx_len, netif->pid);
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_6LN)
    bool new_address = false;
#endif  /* CONFIG_GNRC_IPV6_NIB_6LN */
    gnrc_netif_ipv6_get_iid(netif, (eui64_t *)&addr.u64[1]);
    ipv6_addr_init_prefix(&addr, pfx, pfx_len);
    if ((idx = gnrc_netif_ipv6_addr_idx(netif, &addr)) < 0) {
        if ((idx = gnrc_netif_ipv6_addr_add_internal(netif, &addr, pfx_len,
                                                     flags)) < 0) {
            DEBUG("nib: Can't add link-local address on interface %u\n",
                  netif->pid);
            return;
        }
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_6LN)
        new_address = true;
#endif  /* CONFIG_GNRC_IPV6_NIB_6LN */
    }

#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_6LN)
    /* mark link-local addresses as valid on 6LN */
    if (gnrc_netif_is_6ln(netif) && ipv6_addr_is_link_local(pfx)) {
        /* don't do this beforehand or risk a deadlock:
         *  - gnrc_netif_ipv6_addr_add_internal() adds VALID (i.e. manually configured
         *    addresses to the prefix list locking the NIB's mutex which is already
         *    locked here) */
        netif->ipv6.addrs_flags[idx] &= ~GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_MASK;
        netif->ipv6.addrs_flags[idx] |= GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID;
        gnrc_netif_ipv6_bus_post(netif, GNRC_IPV6_EVENT_ADDR_VALID, &netif->ipv6.addrs[idx]);
    }
#endif  /* CONFIG_GNRC_IPV6_NIB_6LN */
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_6LN)
    if (new_address && gnrc_netif_is_6ln(netif) &&
        !gnrc_netif_is_6lbr(netif)) {
        _handle_rereg_address(&netif->ipv6.addrs[idx]);
    }
#else   /* CONFIG_GNRC_IPV6_NIB_6LN */
    (void)idx;
#endif  /* CONFIG_GNRC_IPV6_NIB_6LN */
}
#endif  /* CONFIG_GNRC_IPV6_NIB_6LN || CONFIG_GNRC_IPV6_NIB_SLAAC */

#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_SLAAC)
static bool _try_l2addr_reconfiguration(gnrc_netif_t *netif)
{
    uint8_t hwaddr[GNRC_NETIF_L2ADDR_MAXLEN];
    uint16_t hwaddr_len;

    if (gnrc_netapi_get(netif->pid, NETOPT_SRC_LEN, 0, &hwaddr_len,
                        sizeof(hwaddr_len)) < 0) {
        return false;
    }
    luid_get(hwaddr, hwaddr_len);
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_6LN)
    if (hwaddr_len == IEEE802154_LONG_ADDRESS_LEN) {
        if (gnrc_netapi_set(netif->pid, NETOPT_ADDRESS_LONG, 0, hwaddr,
                            hwaddr_len) < 0) {
            return false;
        }
    }
    else
#endif
    if (gnrc_netapi_set(netif->pid, NETOPT_ADDRESS, 0, hwaddr,
                        hwaddr_len) < 0) {
        return false;
    }
    return true;
}

static bool _try_addr_reconfiguration(gnrc_netif_t *netif)
{
    eui64_t orig_iid;
    bool remove_old = false, hwaddr_reconf;

    if (gnrc_netif_ipv6_get_iid(netif, &orig_iid) > 0) {
        remove_old = true;
    }
    /* seize netif to netif thread since _try_l2addr_reconfiguration uses
     * gnrc_netapi_get()/gnrc_netapi_set(). Since these are synchronous this is
     * safe */
    gnrc_netif_release(netif);
    /* reacquire netif for IPv6 address reconfiguraton */
    hwaddr_reconf = _try_l2addr_reconfiguration(netif);
    gnrc_netif_acquire(netif);
    if (hwaddr_reconf) {
        if (remove_old) {
            for (unsigned i = 0; i < CONFIG_GNRC_NETIF_IPV6_ADDRS_NUMOF; i++) {
                ipv6_addr_t *addr = &netif->ipv6.addrs[i];
                if (addr->u64[1].u64 == orig_iid.uint64.u64) {
                    gnrc_netif_ipv6_addr_remove_internal(netif, addr);
                }
            }
        }
        DEBUG("nib: Changed hardware address, due to DAD\n");
        _auto_configure_addr(netif, &ipv6_addr_link_local_prefix, 64U);
    }
    return hwaddr_reconf;
}

void _remove_tentative_addr(gnrc_netif_t *netif, const ipv6_addr_t *addr)
{
    DEBUG("nib: other node has TENTATIVE address %s assigned "
          "=> removing that address\n",
          ipv6_addr_to_str(addr_str, addr, sizeof(addr_str)));
    gnrc_netif_ipv6_addr_remove_internal(netif, addr);

    if (!ipv6_addr_is_link_local(addr) ||
        !_try_addr_reconfiguration(netif)) {
        /* Cannot use target address as personal address and can
         * not change hardware address to retry SLAAC => use purely
         * DHCPv6 instead */
        /* TODO: implement IA_NA for DHCPv6 */
        /* then => tgt_netif->aac_mode = GNRC_NETIF_AAC_DHCP; */
        DEBUG("nib: would set interface %i to DHCPv6, "
              "but is not implemented yet", netif->pid);
    }
}

static int _get_netif_state(gnrc_netif_t **netif, const ipv6_addr_t *addr)
{
    *netif = gnrc_netif_get_by_ipv6_addr(addr);
    if (*netif != NULL) {
        int idx;

        gnrc_netif_acquire(*netif);
        idx = gnrc_netif_ipv6_addr_idx(*netif, addr);
        return ((idx >= 0) && gnrc_netif_ipv6_addr_dad_trans(*netif, idx)) ?
               idx : -1;
    }
    return -1;
}

void _handle_dad(const ipv6_addr_t *addr)
{
    ipv6_addr_t sol_nodes;
    gnrc_netif_t *netif = NULL;
    int idx = _get_netif_state(&netif, addr);
    if (idx >= 0) {
        ipv6_addr_set_solicited_nodes(&sol_nodes, addr);
        _snd_ns(addr, netif, &ipv6_addr_unspecified, &sol_nodes);
        _evtimer_add((void *)&netif->ipv6.addrs[idx],
                     GNRC_IPV6_NIB_VALID_ADDR,
                     &netif->ipv6.addrs_timers[idx],
                     netif->ipv6.retrans_time);
    }
    if (netif != NULL) {
        /* was acquired in `_get_netif_state()` */
        gnrc_netif_release(netif);
    }
}

void _handle_valid_addr(const ipv6_addr_t *addr)
{
    gnrc_netif_t *netif = NULL;
    int idx = _get_netif_state(&netif, addr);

    DEBUG("nib: validating address %s (idx: %d, netif: %d)\n",
          ipv6_addr_to_str(addr_str, addr, sizeof(addr_str)), idx,
          (netif != NULL) ? netif->pid : 0);
    if (idx >= 0) {
        netif->ipv6.addrs_flags[idx] &= ~GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_MASK;
        netif->ipv6.addrs_flags[idx] |= GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID;
        gnrc_netif_ipv6_bus_post(netif, GNRC_IPV6_EVENT_ADDR_VALID, &netif->ipv6.addrs[idx]);
    }
    if (netif != NULL) {
        /* was acquired in `_get_netif_state()` */
        gnrc_netif_release(netif);
    }
}
#else  /* CONFIG_GNRC_IPV6_NIB_SLAAC */
typedef int dont_be_pedantic;
#endif /* CONFIG_GNRC_IPV6_NIB_SLAAC */

/** @} */
