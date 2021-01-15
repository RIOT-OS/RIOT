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
 * Defines all functions for the implementation of Minimum Rank
 * with Hysteresis Objective Function. (RFC 6719)
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Benjamin Valentin <benpicco@beuth-hochschule.de>
 * @}
 */

#include <string.h>
#include "mrhof.h"
#include "net/gnrc/rpl.h"
#include "net/gnrc/rpl/structs.h"
#include "net/netstats.h"
#include "net/netstats/neighbor.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/**
 * Retrieve parent statistics from the netstats neighbor module
 */
static bool _mrhof_get_stats(gnrc_rpl_parent_t *parent, netstats_nb_t *out)
{
    gnrc_ipv6_nib_nc_t nce;

    gnrc_netif_t *iface = gnrc_netif_get_by_pid(parent->dodag->iface);

    if (gnrc_ipv6_nib_get_next_hop_l2addr(&parent->addr, iface, NULL, &nce)) {
        return false;
    }

    return netstats_nb_get(&iface->netif, nce.l2addr, nce.l2addr_len, out);
}

/**
 * Select a metric to use as a link metric
 */
static inline uint16_t _link_metric(netstats_nb_t *stats)
{
    if (stats == NULL) {
        return MRHOF_MAX_PATH_COST;
    }

#if IS_USED(MODULE_GNRC_RPL_MRHOF_ETX)
    return stats->etx;
#elif IS_USED(MODULE_GNRC_RPL_MRHOF_LQI)
    /* 255 = best, 0 = worst LQI; map best to ETX = 1, worst to ETX = 7 */
    return 3 * (0xFF - stats->lqi) + NETSTATS_NB_ETX_DIVISOR;
#else
    return MRHOF_MAX_PATH_COST;
#endif
}

/**
 * Retrieve the full path cost of a parent
 */
static uint16_t _mrhof_get_path_cost(gnrc_rpl_parent_t *parent, netstats_nb_t *stats)
{
    return parent->rank + _link_metric(stats);
}

/**
 * Check if a parent is within MAX_LINK_METRIC and MAX_PATH_COST
 */
static bool _mrhof_is_acceptable(gnrc_rpl_parent_t *parent, netstats_nb_t *stats)
{
    return (_link_metric(stats) < MRHOF_MAX_LINK_METRIC) &&
           (_mrhof_get_path_cost(parent, stats) < MRHOF_MAX_PATH_COST);
}

/**
 * Check if a parent is fresh
 */
static bool _mrhof_isfresh(netif_t *netif, netstats_nb_t *stats)
{
    /* No statistics is not fresh */
    if (stats == NULL) {
        return false;
    }
    return netstats_nb_isfresh(netif, stats);
}

/* Compare acceptability of two parents */
static int _mrhof_cmp_acceptable(gnrc_rpl_parent_t *p1, netstats_nb_t *p1_stats,
                                 gnrc_rpl_parent_t *p2, netstats_nb_t *p2_stats)
{
    bool p1_acceptable = _mrhof_is_acceptable(p1, p1_stats);
    bool p2_acceptable = _mrhof_is_acceptable(p2, p2_stats);

    if (p1_acceptable == p2_acceptable) {
        return 0;
    }

    /* One of the two parents is not acceptable */
    if (p1_acceptable == false) {
        /* p2 is acceptable and p1 not */
        return 1;
    }

    if (p2_acceptable == false) {
        /* p1 is acceptable and p2 not */
        return -1;
    }

    return 0;
}

/* Compare freshness of two parents */
static int _mrhof_cmp_fresh(netif_t *netif,
                            netstats_nb_t *p1_stats,
                            netstats_nb_t *p2_stats)
{
    bool p1_isfresh = _mrhof_isfresh(netif, p1_stats);
    bool p2_isfresh = _mrhof_isfresh(netif, p2_stats);

    if (p1_isfresh == p2_isfresh) {
        return 0;
    }

    /* One of the two parents is not acceptable */
    if (!p1_isfresh) {
        /* p2 is acceptable and p1 not */
        return 1;
    }

    if (!p2_isfresh) {
        /* p1 is acceptable and p2 not */
        return -1;
    }

    return 0;
}

static void reset(gnrc_rpl_dodag_t *dodag)
{
    /* Nothing to do in MRHOF */
    (void) dodag;
}

/**
 * Calculate rank additive based on the rank of the parent and the etx to the parent
 * computed via MAX(pref_parent->rank, 1 + floor(MAX(parents->rank)/MinHopRankIncrease),
 */
static uint16_t calc_rank(gnrc_rpl_dodag_t *dodag, uint16_t base_rank)
{
    (void) base_rank;
    DEBUG("MRHOF: Calculating rank\n");

    if (dodag == NULL) {
        DEBUG("MRHOF: No dodag, assuming max rank\n");
        return GNRC_RPL_INFINITE_RANK;
    }

    gnrc_rpl_parent_t *elt = NULL;
    netstats_nb_t elt_stats;
    uint8_t cnt = 0;
    uint8_t max_dagrank = 0;
    uint16_t cost_rank, minhoprankincr = dodag->instance->min_hop_rank_inc;

    /* Determine the path cost through the preferred parent */
    if (!_mrhof_get_stats(dodag->parents, &elt_stats)) {
        DEBUG("MRHOF: No stats for parent, assuming max rank\n");
        return GNRC_RPL_INFINITE_RANK;
    }

    /* calculate rank for path through the preferred parent */
    cost_rank = _mrhof_get_path_cost(dodag->parents, &elt_stats);

    /* Determine the Rank of the member of the parent set with the highest
     * advertised Rank, rounded to the next higher integral Rank, i.e.,
     * to MinHopRankIncrease * (1 + floor(Rank/MinHopRankIncrease)).
     */
    LL_FOREACH(dodag->parents, elt) {
        if (cnt >= MRHOF_PARENT_SET_SIZE) {
            break;
        }
        _mrhof_get_stats(elt, &elt_stats);
        /* Only include parent in the parent set if the statistics are acceptable
         * and the path cost is not significantly worse than the current preferred parent */
        if (_mrhof_is_acceptable(elt, &elt_stats) && \
            (_mrhof_get_path_cost(elt, &elt_stats) <= cost_rank + MRHOF_PARENT_SWITCH_THRESHOLD)) {
            uint8_t new_dagrank = elt->rank / minhoprankincr;
            if (max_dagrank < new_dagrank) {
                max_dagrank = new_dagrank;
            }
        }
        else {
            /* Break when parents are no longer acceptable */
            break;
        }
        cnt++;
    }
    uint16_t monotonic_rank = minhoprankincr * (max_dagrank + 1);

    DEBUG("MRHOF: Path cost: %u, monotonic cost: %u\n", cost_rank, monotonic_rank);
    return (cost_rank > monotonic_rank) ? cost_rank : monotonic_rank;
}

/**
 * Decision based on
 * * If one parent is not acceptable, the other is better
 * * If one parent is not fresh, the other is better
 * * ETX
 */
static int which_parent(gnrc_rpl_parent_t *p1, gnrc_rpl_parent_t *p2)
{
    /* Only return p2 if the rank (full etx path) is better than p1 and better
     * than the preferred parent full path etx by PARENT_SWITCH_THRESHOLD */
    int cmp;
    netstats_nb_t p1_stats, p2_stats;

    assert(p1->dodag->iface == p2->dodag->iface);
    gnrc_netif_t *netif = gnrc_netif_get_by_pid(p1->dodag->iface);

    if (!_mrhof_get_stats(p1, &p1_stats) || !_mrhof_get_stats(p2, &p2_stats)) {
        return 0;
    }

    /* Compare acceptability of parents */
    cmp = _mrhof_cmp_acceptable(p1, &p1_stats, p2, &p2_stats);
    if (cmp != 0) {
        return cmp;
    }

    /* Compare freshness of parents */
    cmp = _mrhof_cmp_fresh(&netif->netif, &p1_stats, &p2_stats);
    if (cmp != 0) {
        return cmp;
    }

    uint16_t p1_path_cost = _mrhof_get_path_cost(p1, &p1_stats);
    uint16_t p2_path_cost = _mrhof_get_path_cost(p2, &p2_stats);

    /* Compare ETX of parents */
    if (p1_path_cost > p2_path_cost) {
        if (p1 == p1->dodag->parents) {
            /* p1 is the preferred parent */
            if (p2_path_cost + MRHOF_PARENT_SWITCH_THRESHOLD < p1_path_cost) {
                return 1;
            }
            else {
                return -1;
            }
        }
        else {
            return 1;
        }
    }
    else if (p1_path_cost == p2_path_cost) {
        return 0;
    }
    return -1;
}

/* Prefer d2 if d2 is grounded and d1 is not */
static gnrc_rpl_dodag_t *which_dodag(gnrc_rpl_dodag_t *d1, gnrc_rpl_dodag_t *d2)
{
    if (!(d1->grounded) && d2->grounded) {
        return d2;
    }
    return d1;
}

static gnrc_rpl_of_t gnrc_rpl_mrhof = {
    .ocp         = 0x1,
    .calc_rank   = calc_rank,
    .parent_cmp  = which_parent,
    .which_dodag = which_dodag,
    .reset       = reset,
    .parent_state_callback = NULL,
    .init        = NULL,
    .process_dio = NULL
};

gnrc_rpl_of_t *gnrc_rpl_get_of_mrhof(void)
{
    return &gnrc_rpl_mrhof;
}
