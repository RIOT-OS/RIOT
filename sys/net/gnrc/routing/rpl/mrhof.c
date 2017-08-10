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
static int which_parent(gnrc_rpl_parent_t *, gnrc_rpl_parent_t *);
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

/**
 * Retrieve parent statistics from the netstats neighbor module (TODO: should be in the neighbor module)
 */
static netstats_nb_t *_mrhof_get_stats(gnrc_rpl_parent_t *parent, netstats_nb_t *stats)
{
    for (netstats_nb_t *entry = stats;
        entry != NULL;
        entry = netstats_nb_get_next(stats, entry)) { 
        /* TODO: Not blindly assume that the mac address is the last 8 bytes of
         * the link local ipv6-address, works for 8 byte long mac addresses */
        if(memcmp(&(parent->addr.u8[8]), entry->l2_addr, 8) == 0) {
            return entry;
        }
    }
    return NULL;
}

/**
 * Retrieve parent etx from the netstats neighbor module
 */
static uint16_t _mrhof_get_etx(netstats_nb_t *stats)
{
    if (stats == NULL) {
        return MRHOF_MAX_PATH_COST;
    }
    return stats->etx;
}

/**
 * Retrieve the full path cost of a parent
 */
static uint16_t _mrhof_get_path_cost(gnrc_rpl_parent_t *parent, netstats_nb_t *stats)
{
    uint16_t etx = _mrhof_get_etx(stats);
    if (etx == MRHOF_MAX_PATH_COST) {
        return etx;
    }
    return parent->rank + etx;
}

/**
 * Check if a parent is within MAX_LINK_METRIC and MAX_PATH_COST
 */
static bool _mrhof_is_acceptable(gnrc_rpl_parent_t *parent, netstats_nb_t *stats)
{
    uint16_t etx = _mrhof_get_etx(stats);
    if (etx < MRHOF_MAX_LINK_METRIC &&\
        _mrhof_get_path_cost(parent, stats) < MRHOF_MAX_PATH_COST) {
        return true;
    }
    return false;
}

/**
 * Check if a parent is fresh
 */
static bool _mrhof_isfresh(netstats_nb_t *stats)
{
    /* No statistics is not fresh */
    if( stats == NULL) {
        return false;
    }
    return netstats_nb_isfresh(stats);
}

/* Compare acceptability of two parents */
static int _mrhof_cmp_acceptable(gnrc_rpl_parent_t *p1, netstats_nb_t *p1_stats,
                                 gnrc_rpl_parent_t *p2, netstats_nb_t *p2_stats)
{
    bool p1_acceptable = _mrhof_is_acceptable(p1, p1_stats);
    bool p2_acceptable = _mrhof_is_acceptable(p2, p2_stats);
    if (p1_acceptable != p2_acceptable) {
    /* One of the two parents is not acceptable */
        if (p1_acceptable == false) {
            /* p2 is acceptable and p1 not */
            return 1;
        }
        if (p2_acceptable == false) {
            /* p1 is acceptable and p2 not */
            return -1;
        }
    }
    return 0;
}

/* Compare freshness of two parents */
static int _mrhof_cmp_fresh(netstats_nb_t *p1_stats,
                            netstats_nb_t *p2_stats)
{
    bool p1_isfresh = _mrhof_isfresh(p1_stats);
    bool p2_isfresh = _mrhof_isfresh(p2_stats);
    if (p1_isfresh != p2_isfresh) {
    /* One of the two parents is not acceptable */
        if (p1_isfresh == false) {
            /* p2 is acceptable and p1 not */
            return 1;
        }
        if (p2_isfresh == false) {
            /* p1 is acceptable and p2 not */
            return -1;
        }
    }
    return 0;
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
    gnrc_rpl_dodag_t *dodag;
    /* TODO: consider a parent set of more than 1 parent */
    if (parent == NULL) {
        return GNRC_RPL_INFINITE_RANK;
    } else {
        dodag = parent->dodag;
        gnrc_rpl_parent_t *elt;
        netstats_nb_t *nb, *elt_stats;
        uint8_t cnt = 0;
        uint8_t max_dagrank = 0;
        uint16_t minhoprankincr = parent->dodag->instance->min_hop_rank_inc;
        int res = gnrc_netapi_get(parent->dodag->iface, NETOPT_STATS_NEIGHBOR, 0, &nb, sizeof(&nb));
        assert(res != -ENOTSUP);

        /* Determine the path cost through the preferred parent */
        elt_stats = _mrhof_get_stats(parent, nb);
        uint16_t cost_rank = _mrhof_get_path_cost(parent, elt_stats);
        /* Determine the largest dagrank in the parent set */
        LL_FOREACH(dodag->parents, elt) {
            if (cnt >= MRHOF_PARENT_SET_SIZE) {
                break;
            }
            elt_stats = _mrhof_get_stats(elt, nb);
            /* Always include the preferred parent in this calculation */
            if (_mrhof_is_acceptable(elt, elt_stats)) {
                uint8_t new_dagrank = DAGRANK(elt->rank, minhoprankincr);
                if (max_dagrank < new_dagrank) {
                    max_dagrank = new_dagrank;
                }
            } else {
                /* Break when parents are no longer acceptable */
                break;
            }
            cnt++;
        }
        uint16_t monotonic_rank = minhoprankincr * (max_dagrank + 1);
        return (cost_rank > monotonic_rank) ? cost_rank : monotonic_rank;
    }
}

/**
 * Decision based on
 * * If one parent is not acceptable, the other is better
 * * If one parent is not fresh, the other is better
 * * ETX
 */
int which_parent(gnrc_rpl_parent_t *p1, gnrc_rpl_parent_t *p2)
{
    /* Only return p2 if the rank (full etx path) is better than p1 and better
     * than the preferred parent full path etx by PARENT_SWITCH_THRESHOLD */
    netstats_nb_t *nb;
    int cmp;
    int res = gnrc_netapi_get(p1->dodag->iface, NETOPT_STATS_NEIGHBOR, 0, &nb, sizeof(&nb)); 
    assert(res != -ENOTSUP);
    netstats_nb_t *p1_stats = _mrhof_get_stats(p1, nb);
    netstats_nb_t *p2_stats = _mrhof_get_stats(p2, nb);

    /* Compare acceptability of parents */
    cmp = _mrhof_cmp_acceptable(p1, p1_stats, p2, p2_stats);
    if (cmp != 0) {
        return cmp;
    }

    /* Compare freshness of parents */
    cmp = _mrhof_cmp_fresh(p1_stats, p2_stats);
    if (cmp != 0) {
        return cmp;
    }
    uint16_t p1_path_cost = _mrhof_get_path_cost(p1, p1_stats);
    uint16_t p2_path_cost = _mrhof_get_path_cost(p2, p2_stats);

    /* Compare ETX of parents */
    if (p1_path_cost > p2_path_cost) {
        if (p1 == p1->dodag->parents) {
            /* p1 is the preferred parent */
            if (p2_path_cost + MRHOF_PARENT_SWITCH_THRESHOLD < p1_path_cost) {
                return 1;
            } else {
                return -1;
            }
        } else {
            return 1;
        }
    }
    else if (p1_path_cost == p2_path_cost) {
        return 0;
    }
    return -1;
}

/* Prefer d2 if d2 is grounded and d1 is not */
gnrc_rpl_dodag_t *which_dodag(gnrc_rpl_dodag_t *d1, gnrc_rpl_dodag_t *d2)
{
    if ( !(d1->grounded) && d2->grounded) {
        return d2;
    }
    return d1;
}
