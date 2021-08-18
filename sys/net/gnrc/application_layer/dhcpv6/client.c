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
#include "net/arp.h"
#include "net/dhcpv6.h"
#include "net/gnrc/dhcpv6/client/simple_pd.h"
#include "net/gnrc/ipv6/nib/pl.h"
#include "net/gnrc/sixlowpan/ctx.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/rpl.h"
#include "net/sock.h"
#include "timex.h"
#include "evtimer.h"

#include "net/dhcpv6/client.h"

#define ENABLE_DEBUG 0
#include "debug.h"

unsigned dhcpv6_client_get_duid_l2(unsigned iface, dhcpv6_duid_l2_t *duid)
{
    gnrc_netif_t *netif;
    uint8_t *l2addr = ((uint8_t *)(duid)) + sizeof(dhcpv6_duid_l2_t);
    int res;

    duid->type = byteorder_htons(DHCPV6_DUID_TYPE_L2);
    /* TODO make GNRC-independent */
    if (iface == SOCK_ADDR_ANY_NETIF) {
        netif = gnrc_netif_iter(NULL);
    }
    else {
        netif = gnrc_netif_get_by_pid(iface);
    }
    assert(netif != NULL);
    if ((res = gnrc_netapi_get(netif->pid, NETOPT_ADDRESS_LONG, 0,
                               l2addr, GNRC_NETIF_L2ADDR_MAXLEN)) > 0) {
        duid->l2type = byteorder_htons(ARP_HWTYPE_EUI64);
    }
    else {
        switch (netif->device_type) {
            case NETDEV_TYPE_SLIP:
                duid->l2type = byteorder_htons(ARP_HWTYPE_SERIAL);
                /* L2 address length is 0 */
                res = 0;
                break;
            case NETDEV_TYPE_ETHERNET:
            case NETDEV_TYPE_BLE:
            case NETDEV_TYPE_ESP_NOW:
                if ((res = gnrc_netapi_get(netif->pid,
                                           NETOPT_ADDRESS,
                                           0, l2addr,
                                           GNRC_NETIF_L2ADDR_MAXLEN)) > 0) {
                    duid->l2type = byteorder_htons(ARP_HWTYPE_ETHERNET);
                    break;
                }
                /* intentionally falls through */
            default:
                LOG_ERROR("DHCPv6 client: Link-layer type of interface %u not supported "
                          "for DUID creation\n", netif->pid);
                return 0;
        }
    }
    return (uint8_t)res + sizeof(dhcpv6_duid_l2_t);
}

void dhcpv6_client_conf_prefix(unsigned iface, const ipv6_addr_t *pfx,
                               unsigned pfx_len, uint32_t valid,
                               uint32_t pref)
{
    gnrc_netif_t *netif = gnrc_netif_get_by_pid(iface);
    int idx = gnrc_netif_ipv6_add_prefix(netif, pfx, pfx_len, valid, pref);
    if ((idx >= 0) && (pfx_len != IPV6_ADDR_BIT_LEN)) {
        /* start advertising subnet obtained via DHCPv6 */
        gnrc_ipv6_nib_change_rtr_adv_iface(netif, true);
        /* configure this router as RPL root */
        gnrc_rpl_configure_root(netif, &netif->ipv6.addrs[idx]);
    }
}

bool dhcpv6_client_check_ia_na(unsigned iface)
{
    gnrc_netif_t *netif = gnrc_netif_get_by_pid(iface);

    return netif->ipv6.aac_mode & GNRC_NETIF_AAC_DHCP;
}

uint32_t dhcpv6_client_prefix_valid_until(unsigned netif,
                                          const ipv6_addr_t *pfx,
                                          unsigned pfx_len)
{
    gnrc_ipv6_nib_pl_t ple;
    void *state = NULL;
    uint32_t max_valid = 0;
    uint32_t now = evtimer_now_msec();

    while (gnrc_ipv6_nib_pl_iter(netif, &state, &ple)) {
        if ((ple.pfx_len == pfx_len) &&
            (((ple.valid_until - now) / MS_PER_SEC) > max_valid) &&
            (ipv6_addr_match_prefix(&ple.pfx,
                                    pfx) >= ple.pfx_len)) {
            max_valid = (ple.valid_until - now) / MS_PER_SEC;
        }
    }
    return max_valid;
}

/** @} */
