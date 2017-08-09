/*
 * Copyright (C) 2017 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc_rpl
 * @{
 * @file
 * @brief       Minimum Rank with Hysteresis Objective Function
 *
 * Header-file, which defines all functions for the implementation of Minimum Rank with Hysteresis Objective Function.
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @}
 */

#include <string.h>
#include "mrhof.h"
#include "net/gnrc/rpl.h"
#include "net/gnrc/rpl/structs.h"
#include "net/netstats.h"
#include "net/netstats/neighbor.h"

static uint16_t calc_rank(gnrc_rpl_parent_t *, uint16_t);
static gnrc_rpl_parent_t *which_parent(gnrc_rpl_parent_t *, gnrc_rpl_parent_t *);
static gnrc_rpl_dodag_t *which_dodag(gnrc_rpl_dodag_t *, gnrc_rpl_dodag_t *);
static void reset(gnrc_rpl_dodag_t *);

static gnrc_rpl_of_t gnrc_rpl_mrhof = {
    0x1,
    calc_rank,
    which_parent,
    which_dodag,
    reset,
    NULL,
    NULL,
    NULL
};

/* Retrieve parent statistics from the netstats neighbor module */
static uint16_t _mrhof_get_etx(gnrc_rpl_parent_t *parent, netstats_nb_t *stats)
{
    for (netstats_nb_t *entry = stats;
        entry != NULL;
        entry = netstats_nb_get_next(stats, entry)) { 
        /* TODO: Not blindly assume that the mac address is the last 8 bytes of
         * the link local ipv6-address, works for 8 byte long mac addresses */
        if(memcmp(&(parent->addr.u8[8]), entry->l2_addr, 8) == 0) {
            return entry->etx;
        }
    }
    return 65535;
}

static uint16_t _mrhof_get_path_cost(gnrc_rpl_parent_t *parent)
{
    netstats_nb_t *nb;
    gnrc_netapi_get(parent->dodag->iface, NETOPT_STATS_NEIGHBOR, 0, &nb, sizeof(&nb)); 
    return parent->rank + _mrhof_get_etx(parent, nb);
}

gnrc_rpl_of_t *gnrc_rpl_get_of_mrhof(void)
{
    return &gnrc_rpl_mrhof;
}

void reset(gnrc_rpl_dodag_t *dodag)
{
    /* Nothing to do in MRHOF */
    (void) dodag;
}

/**
 * Calculate rank additive based on the rank of the parent and the etx to the parent
 * computed via MAX(pref_parent->rank, 1 + floor(MAX(parents->rank)/MinHopRankIncrease), 
 */
uint16_t calc_rank(gnrc_rpl_parent_t *parent, uint16_t base_rank)
{
    (void) base_rank;
    /* TODO: consider a parent set of more than 1 parent */
    if (parent == NULL) {
        return GNRC_RPL_INFINITE_RANK;
    } else {
        uint16_t minhoprankincr = parent->dodag->instance->min_hop_rank_inc;
        uint16_t cost_rank = _mrhof_get_path_cost(parent);
        uint16_t monotonic_rank = minhoprankincr * (parent->rank/minhoprankincr + 1);
        return (cost_rank > monotonic_rank) ? cost_rank : monotonic_rank;
    }

}

gnrc_rpl_parent_t *which_parent(gnrc_rpl_parent_t *p1, gnrc_rpl_parent_t *p2)
{
    /* Only return p2 if the rank (full etx path) is better than p1 and better
     * than the preferred parent full path etx by PARENT_SWITCH_THRESHOLD */
    netstats_nb_t *nb;
    int res = gnrc_netapi_get(p1->dodag->iface, NETOPT_STATS_NEIGHBOR, 0, &nb, sizeof(&nb)); 
    assert(res != -ENOTSUP);
    uint16_t p1_totalrank = p1->rank + _mrhof_get_etx(p1, nb);
    uint16_t p2_totalrank = p2->rank + _mrhof_get_etx(p2, nb);

    /* TODO: Test if we only want to switch if p2 is fresh */
    if (p1_totalrank > p2_totalrank) {
        gnrc_rpl_parent_t *preferred = p1->dodag->parents;
        uint16_t preferred_totalrank = preferred->rank + _mrhof_get_etx(preferred, nb);
        if (p2_totalrank + MRHOF_PARENT_SWITCH_THRESHOLD < preferred_totalrank) {
            return p2;
        }
    }

    return p1;
}

/* Prefer d2 if d2 is grounded and d1 is not */
gnrc_rpl_dodag_t *which_dodag(gnrc_rpl_dodag_t *d1, gnrc_rpl_dodag_t *d2)
{
    if ( !(d1->grounded) && d2->grounded) {
        return d2;
    }
    return d1;
}
