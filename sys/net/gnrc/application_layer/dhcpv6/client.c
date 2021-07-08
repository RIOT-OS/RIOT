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

#include "net/dhcpv6/client.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static char addr_str[IPV6_ADDR_MAX_STR_LEN];

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

static bool _ctx_match(const gnrc_sixlowpan_ctx_t *ctx,
                       const ipv6_addr_t *prefix, uint8_t prefix_len)
{
    return (ctx != NULL) &&
           (ctx->prefix_len == prefix_len) &&
           (ipv6_addr_match_prefix(&ctx->prefix, prefix) >= prefix_len);
}

static void _update_6ctx(const ipv6_addr_t *prefix, uint8_t prefix_len)
{
    gnrc_sixlowpan_ctx_t *ctx = gnrc_sixlowpan_ctx_lookup_addr(prefix);
    uint8_t cid = 0;

    if (!_ctx_match(ctx, prefix, prefix_len)) {
        /* While the context is a prefix match, the defined prefix within the
         * context does not match => use new context */
        ctx = NULL;
    }
    else {
        cid = ctx->flags_id & GNRC_SIXLOWPAN_CTX_FLAGS_CID_MASK;
    }
    /* find first free context ID */
    if (ctx == NULL) {
        while (((ctx = gnrc_sixlowpan_ctx_lookup_id(cid)) != NULL) &&
               !_ctx_match(ctx, prefix, prefix_len)) {
            cid++;
        }
    }
    if (cid < GNRC_SIXLOWPAN_CTX_SIZE) {
        DEBUG("DHCP client: add compression context %u for prefix %s/%u\n", cid,
              ipv6_addr_to_str(addr_str, prefix, sizeof(addr_str)),
              prefix_len);
        gnrc_sixlowpan_ctx_update(cid, (ipv6_addr_t *)prefix, prefix_len,
                                  CONFIG_GNRC_DHCPV6_CLIENT_6LBR_6LO_CTX_MIN,
                                  true);
    }
}

void dhcpv6_client_conf_prefix(unsigned iface, const ipv6_addr_t *pfx,
                               unsigned pfx_len, uint32_t valid,
                               uint32_t pref)
{
    gnrc_netif_t *netif = gnrc_netif_get_by_pid(iface);
    eui64_t iid;
    ipv6_addr_t addr;

    assert(netif != NULL);
    DEBUG("GNRC DHCPv6 client: (re-)configure prefix %s/%d\n",
          ipv6_addr_to_str(addr_str, pfx, sizeof(addr_str)), pfx_len);
    if (gnrc_netapi_get(netif->pid, NETOPT_IPV6_IID, 0, &iid,
                        sizeof(eui64_t)) >= 0) {
        ipv6_addr_set_aiid(&addr, iid.uint8);
    }
    else {
        LOG_WARNING("GNRC DHCPv6 client: cannot get IID of netif %u\n", netif->pid);
        return;
    }
    ipv6_addr_init_prefix(&addr, pfx, pfx_len);
    /* add address as tentative */
    if (gnrc_netif_ipv6_addr_add(netif, &addr, pfx_len,
            GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_TENTATIVE & 0x1) > 0) {
        /* update lifetime */
        if (valid < UINT32_MAX) { /* UINT32_MAX means infinite lifetime */
            /* the valid lifetime is given in seconds, but the NIB's timers work
             * in microseconds, so we have to scale down to the smallest
             * possible value (UINT32_MAX - 1). */
            valid = (valid > (UINT32_MAX / MS_PER_SEC)) ?
                          (UINT32_MAX - 1) : valid * MS_PER_SEC;
        }
        if (pref < UINT32_MAX) { /* UINT32_MAX means infinite lifetime */
            /* same treatment for pref */
            pref = (pref > (UINT32_MAX / MS_PER_SEC)) ?
                         (UINT32_MAX - 1) : pref * MS_PER_SEC;
        }
        gnrc_ipv6_nib_pl_set(netif->pid, pfx, pfx_len, valid, pref);
        if (IS_USED(MODULE_GNRC_IPV6_NIB) &&
            IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_6LBR) &&
            IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_MULTIHOP_P6C) &&
            gnrc_netif_is_6ln(netif)) {
            if (IS_USED(MODULE_GNRC_SIXLOWPAN_CTX)) {
                _update_6ctx(pfx, pfx_len);
            }
            (void)gnrc_ipv6_nib_abr_add(&addr);
        }
        if (IS_USED(MODULE_GNRC_RPL)) {
            gnrc_rpl_init(netif->pid);
            gnrc_rpl_instance_t *inst = gnrc_rpl_instance_get(
                    CONFIG_GNRC_RPL_DEFAULT_INSTANCE
                );
            if (inst) {
                gnrc_rpl_instance_remove(inst);
            }
            gnrc_rpl_root_init(CONFIG_GNRC_RPL_DEFAULT_INSTANCE, &addr, false, false);
        }
    }
}

uint32_t dhcpv6_client_prefix_valid_until(unsigned netif,
                                          const ipv6_addr_t *pfx,
                                          unsigned pfx_len)
{
    gnrc_ipv6_nib_pl_t ple;
    void *state = NULL;
    uint32_t max_valid = 0;

    while (gnrc_ipv6_nib_pl_iter(netif, &state, &ple)) {
        if ((ple.pfx_len == pfx_len) &&
            ((ple.valid_until / MS_PER_SEC) > max_valid) &&
            (ipv6_addr_match_prefix(&ple.pfx,
                                    pfx) >= ple.pfx_len)) {
            max_valid = ple.valid_until / MS_PER_SEC;
        }
    }
    return max_valid;
}

/** @} */
