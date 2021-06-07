/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_ipv6_auto_subnets Simple-Subnet auto-configuration
 * @ingroup     net_gnrc
 * @brief       Automatic configuration for cascading subnets
 *
 * About
 * =====
 *
 * This module provides an automatic configuration for networks with a simple
 * tree topology.
 *
 * If a sufficiently large IPv6 prefix (> /64) is provided via Router Advertisements,
 * a routing node with this module will automatically configure subnets from it
 * by dividing it into sub-prefixes for each downstream interface.
 *
 * There can only be a single routing node on each level of the network but an
 * arbitrary number of leaf nodes.
 *
 * ![Example Topology](gnrc_ipv6_auto_subnets_simple.svg)
 *
 * The downstream network(s) receive the sub-prefix via Router Advertisements
 * and the process repeats until the bits of the prefix are exhausted.
 * The smallest subnet must still have a /64 prefix.
 *
 * The new subnet must no longer be considered on-link by the hosts in the
 * parent network.
 * Therefore the downstream router will send a router advertisement with only
 * a Route Information Option included to the upstream network.
 * The Route Information Option contains the prefix of the downstream network
 * so that upstream routers will no longer consider hosts in this subnet on-link
 * but instead will use the downstream router to route to the new subnet.
 *
 * Usage
 * =====
 *
 * Simply add the `gnrc_ipv6_auto_subnets` module to the code of the nodes that
 * should act as routers in the cascading network.
 * The upstream network will be automatically chosen as the one that first
 * receives a router advertisement.
 *
 * @{
 *
 * @file
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include "net/gnrc/ipv6/nib.h"
#include "net/gnrc/ndp.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static char addr_str[IPV6_ADDR_MAX_STR_LEN];

static void _init_sub_prefix(ipv6_addr_t *out,
                             const ipv6_addr_t *prefix, uint8_t bits,
                             uint8_t idx, uint8_t idx_bits)
{
    uint8_t bytes = bits / 8;
    uint8_t rem   = bits % 8;
    int8_t shift  = 8 - rem - idx_bits;

    /* first copy old prefix */
    memset(out, 0, sizeof(*out));
    ipv6_addr_init_prefix(out, prefix, bits);

    /* if new bits are between bytes, first copy over the most significant bits */
    if (shift < 0) {
        out->u8[bytes] |= idx >> -shift;
        out->u8[++bytes] = 0;
        shift += 8;
    }

    /* shift remaining bits at the end of the prefix */
    out->u8[bytes] |= idx << shift;
}

static bool _remove_old_prefix(gnrc_netif_t *netif,
                               const ipv6_addr_t *pfx, uint8_t pfx_len,
                               gnrc_pktsnip_t **ext_opts)
{
    gnrc_ipv6_nib_pl_t entry;
    gnrc_pktsnip_t *tmp;
    void *state = NULL;
    ipv6_addr_t old_pfx;
    uint8_t old_pfx_len = 0;

    /* iterate prefix list to see if the prefix already exists */
    while (gnrc_ipv6_nib_pl_iter(netif->pid, &state, &entry)) {
        uint8_t match_len = ipv6_addr_match_prefix(&entry.pfx, pfx);

        /* The prefix did not change - nothing to do here */
        if (match_len >= pfx_len && pfx_len == entry.pfx_len) {
            return true;
        }

        /* find prefix that is closest to the new prefix */
        if (match_len > old_pfx_len) {
            old_pfx_len = entry.pfx_len;
            old_pfx = entry.pfx;
        }
    }

    /* no prefix found */
    if (old_pfx_len == 0) {
        return false;
    }

    DEBUG("auto_subnets: remove old prefix %s/%u\n",
          ipv6_addr_to_str(addr_str, &old_pfx, sizeof(addr_str)), old_pfx_len);

    /* invalidate old prefix in RIO */
    tmp = gnrc_ndp_opt_ri_build(&old_pfx, old_pfx_len, 0,
                                NDP_OPT_RI_FLAGS_PRF_NONE, *ext_opts);
    if (tmp) {
        *ext_opts = tmp;
    }

    /* remove the prefix */
    gnrc_ipv6_nib_pl_del(netif->pid, &old_pfx, old_pfx_len);

    return false;
}

static void _configure_subnets(uint8_t subnets, gnrc_netif_t *upstream,
                               const ndp_opt_pi_t *pio)
{
    gnrc_netif_t *downstream = NULL;
    gnrc_pktsnip_t *ext_opts = NULL;
    const ipv6_addr_t *prefix = &pio->prefix;
    uint32_t valid_ltime = byteorder_ntohl(pio->valid_ltime);
    uint32_t pref_ltime = byteorder_ntohl(pio->pref_ltime);
    const uint8_t prefix_len = pio->prefix_len;
    uint8_t new_prefix_len, subnet_len;

    DEBUG("auto_subnets: create %u subnets\n", subnets);

    /* Calculate remaining prefix length.
     * For n subnets we consume floor(log_2 n) + 1 bits.
     * To calculate floor(log_2 n) quickly, find the position of the
     * most significant set bit by counting leading zeros.
     */
    subnet_len = 32 - __builtin_clz(subnets);
    new_prefix_len = prefix_len + subnet_len;

    if (new_prefix_len > 64) {
        DEBUG("auto_subnets: can't split /%u into %u subnets\n", prefix_len, subnets);
        return;
    }

    while ((downstream = gnrc_netif_iter(downstream))) {
        gnrc_pktsnip_t *tmp;
        ipv6_addr_t new_prefix;

        if (downstream == upstream) {
            continue;
        }

        /* create subnet from upstream prefix */
        _init_sub_prefix(&new_prefix, prefix, prefix_len, subnets--, subnet_len);

        DEBUG("auto_subnets: configure prefix %s/%u on %u\n",
              ipv6_addr_to_str(addr_str, &new_prefix, sizeof(addr_str)),
              new_prefix_len, downstream->pid);

        /* first remove old prefix if the prefix changed */
        _remove_old_prefix(downstream, &new_prefix, new_prefix_len, &ext_opts);

        /* configure subnet on downstream interface */
        gnrc_netif_ipv6_add_prefix(downstream, &new_prefix, new_prefix_len,
                                   valid_ltime, pref_ltime);

        /* start advertising subnet */
        gnrc_ipv6_nib_change_rtr_adv_iface(downstream, true);

        /* add route information option with new subnet */
        tmp = gnrc_ndp_opt_ri_build(&new_prefix, new_prefix_len, valid_ltime,
                                    NDP_OPT_RI_FLAGS_PRF_NONE, ext_opts);
        if (tmp == NULL) {
            DEBUG("auto_subnets: No space left in packet buffer. Not adding RIO\n");
        } else {
            ext_opts = tmp;
        }
    }

    /* immediately send an RA with RIO */
    if (ext_opts) {
        gnrc_ndp_rtr_adv_send(upstream, NULL,
                              &ipv6_addr_all_nodes_link_local, true, ext_opts);
    } else {
        DEBUG("auto_subnets: Options empty, not sending RA\n");
    }
}

void gnrc_ipv6_nib_rtr_adv_pio_cb(gnrc_netif_t *upstream, const ndp_opt_pi_t *pio)
{
    /* create a subnet for each downstream interface */
    unsigned subnets = gnrc_netif_numof() - 1;

    if (subnets == 0) {
        return;
    }

    if (pio->valid_ltime.u32 == 0) {
        return;
    }

    _configure_subnets(subnets, upstream, pio);
}
/** @} */
