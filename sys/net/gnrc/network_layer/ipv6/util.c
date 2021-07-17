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
#include "net/gnrc/ipv6/nib/pl.h"
#include "net/gnrc/sixlowpan/ctx.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/rpl.h"
#include "net/sock.h"
#include "timex.h"

#include "net/gnrc/netif/internal.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static char addr_str[IPV6_ADDR_MAX_STR_LEN];

static bool _ctx_match(const gnrc_sixlowpan_ctx_t *ctx,
                       const ipv6_addr_t *prefix, uint8_t prefix_len)
{
    return (ctx != NULL) &&
           (ctx->prefix_len == prefix_len) &&
           (ipv6_addr_match_prefix(&ctx->prefix, prefix) >= prefix_len);
}

static void _update_6ctx(const ipv6_addr_t *prefix, uint8_t prefix_len,
                         uint32_t valid)
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
        DEBUG("GNRC util: add compression context %u for prefix %s/%u\n", cid,
              ipv6_addr_to_str(addr_str, prefix, sizeof(addr_str)),
              prefix_len);
        gnrc_sixlowpan_ctx_update(cid, (ipv6_addr_t *)prefix, prefix_len,
                                  valid / (60 * MS_PER_SEC),
                                  true);
    }
}

void gnrc_util_conf_prefix(gnrc_netif_t *netif, const ipv6_addr_t *pfx,
                           unsigned pfx_len, uint32_t valid, uint32_t pref)
{
    eui64_t iid;
    ipv6_addr_t addr;

    assert(netif != NULL);
    DEBUG("GNRC util: (re-)configure prefix %s/%d\n",
          ipv6_addr_to_str(addr_str, pfx, sizeof(addr_str)), pfx_len);
    if (gnrc_netapi_get(netif->pid, NETOPT_IPV6_IID, 0, &iid,
                        sizeof(eui64_t)) >= 0) {
        ipv6_addr_set_aiid(&addr, iid.uint8);
    }
    else {
        LOG_WARNING("GNRC util: cannot get IID of netif %u\n", netif->pid);
        return;
    }
    ipv6_addr_init_prefix(&addr, pfx, pfx_len);
    /* add address as tentative */
    if (gnrc_netif_ipv6_addr_add_internal(netif, &addr, pfx_len,
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
                _update_6ctx(pfx, pfx_len, valid);
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

/** @} */
