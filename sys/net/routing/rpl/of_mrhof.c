/**
 * OF MRHOF implementation
 *
 * Copyright (C) 2013 Stephan Arndt <arndtste@zedat.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup rpl
 * @{
 * @file
 * @author  Stephan Arndt <arndtste@zedat.fu-berlin.de>
 * @}
 */

#include <string.h>
#include <stdio.h>
#include "of_mrhof.h"

#include "etx_beaconing.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

// Function Prototypes
static uint16_t calc_rank(rpl_parent_t *, uint16_t);
static rpl_parent_t *which_parent(rpl_parent_t *, rpl_parent_t *);
static rpl_dodag_t *which_dodag(rpl_dodag_t *, rpl_dodag_t *);
static void reset(rpl_dodag_t *);
static uint16_t calc_path_cost(rpl_parent_t *parent);

uint16_t cur_min_path_cost = MAX_PATH_COST;
rpl_parent_t *cur_preferred_parent = NULL;

rpl_of_t rpl_of_mrhof = {
    0x1,
    calc_rank,
    which_parent,
    which_dodag,
    reset,
    NULL,
    NULL,
    NULL
};

rpl_of_t *rpl_get_of_mrhof(void)
{
    return &rpl_of_mrhof;
}

void reset(rpl_dodag_t *dodag)
{
    (void) dodag;
}

static uint16_t calc_path_cost(rpl_parent_t *parent)
{
    DEBUGF("calc_pathcost\n");

    /*
     * Calculates the path cost through the parent, for now, only for ETX
     */
    if (parent == NULL) {
        // Shouldn't ever happen since this function is supposed to be always
        // run with a parent. If it does happen, we can assume a root called it.
        DEBUGF("[WARNING] calc_path_cost called without parent!\n");
        return DEFAULT_MIN_HOP_RANK_INCREASE;
    }

    double etx_value = etx_get_metric(&(parent->addr));
    DEBUGF("Metric for parent returned: %f\n", etx_value);

    if (etx_value != 0) {
        /*
         * (ETX_for_link_to_neighbor * 128) + Rank_of_that_neighbor
         *
         * This means I get the rank of that neighbor (which is the etx
         * of the whole path from him to the root node) plus my ETX to
         * that neighbor*128, which would be the 'rank' of the single link
         * from me to that neighbor
         *
         */
        if (etx_value * ETX_RANK_MULTIPLIER > MAX_LINK_METRIC) {
            // Disallow links with an estimated ETX of 4 or higher
            return MAX_PATH_COST;
        }

        if (etx_value * ETX_RANK_MULTIPLIER + parent->rank
            < parent->rank) {
            //Overflow
            return MAX_PATH_COST;
        }

        //TODO runden
        return etx_value * ETX_RANK_MULTIPLIER
               + parent->rank;
    }
    else {
        // IMPLEMENT HANDLING OF OTHER METRICS HERE
        // if it is 0, it hasn't been computed, thus we cannot compute a path
        // cost
        return MAX_PATH_COST;
    }
}

static uint16_t calc_rank(rpl_parent_t *parent, uint16_t base_rank)
{
    DEBUGF("calc_rank\n");

    /*
     * Return the rank for this node.
     *
     * For now, there is no metric-selection or specification, so the rank com-
     * putation will always be assumed to be done for the ETX metric.
     * Baserank is pretty much only used to find out if a node is a root or not.
     */
    if (parent == NULL) {
        if (base_rank == 0) {
            //No parent, no rank, a root node would have a rank != 0
            return INFINITE_RANK;
        }

        /*
         * No parent, base_rank != 0 means this is a root node or a node which
         * is recalculating.
         * Since a recalculating node must have a parent in this implementation
         * (see rpl.c, function global_repair), we can assume this node is root.
         */
        return DEFAULT_MIN_HOP_RANK_INCREASE;
    }
    else {
        /*
         * We have a parent and are a non-root node, calculate the path cost for
         * the parent and choose the maximum of that value and the advertised
         * rank of the parent + minhoprankincrease for our rank.
         */
        uint16_t calculated_pcost = calc_path_cost(parent);

        if (calculated_pcost < MAX_PATH_COST) {
            if ((parent->rank + parent->dodag->minhoprankincrease)
                > calculated_pcost) {
                return parent->rank + parent->dodag->minhoprankincrease;
            }
            else {
                return calculated_pcost;
            }
        }
        else {
            //Path costs are greater than allowed
            return INFINITE_RANK;
        }
    }
}

static rpl_parent_t *which_parent(rpl_parent_t *p1, rpl_parent_t *p2)
{
    DEBUGF("which_parent\n");
    /*
     * Return the parent with the lowest path cost.
     * Before returning any of the two given parents, make sure that a switch is
     * desirable.
     *
     */
    uint16_t path_p1    = calc_path_cost(p1);
    uint16_t path_p2    = calc_path_cost(p2);

    if (cur_preferred_parent != NULL) {
        //test if the parent from which we got this path is still active
        if (cur_preferred_parent->used != 0) {
            // Test, if the current best path is better than both parents given
            if (cur_min_path_cost < path_p1 + PARENT_SWITCH_THRESHOLD
                && cur_min_path_cost < path_p2 + PARENT_SWITCH_THRESHOLD) {
                return cur_preferred_parent;
            }
        }
    }

    if (path_p1 < path_p2) {
        /*
         * Return the current best parent, and set it as current best parent
         */
        cur_min_path_cost = path_p1;
        cur_preferred_parent = p1;
        return p1;
    }

    cur_min_path_cost = path_p2;
    cur_preferred_parent = p2;
    return p2;
}

//Not used yet, as the implementation only makes use of one dodag for now.
static rpl_dodag_t *which_dodag(rpl_dodag_t *d1, rpl_dodag_t *d2)
{
    (void) d2;
    return d1;
}
