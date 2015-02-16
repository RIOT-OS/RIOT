/*
 * Copyright (C) 2014 Oliver Hahm <oliver.hahm@inria.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     rpl
 * @{
 * @file        of0.c
 * @brief       Objective Function Zero.
 *
 * Implementation of Objective Function Zero.
 *
 * @author      Eric Engel <eric.engel@fu-berlin.de>
 * @}
 */

#include <string.h>
#include "of0.h"

//Function Prototypes
static network_uint16_t calc_rank(rpl_parent_t *, network_uint16_t);
static rpl_parent_t *which_parent(rpl_parent_t *, rpl_parent_t *);
static rpl_dodag_t *which_dodag(rpl_dodag_t *, rpl_dodag_t *);
static void reset(rpl_dodag_t *);

static rpl_of_t rpl_of0 = {
    {0x0000},
    calc_rank,
    which_parent,
    which_dodag,
    reset,
    NULL,
    NULL,
    NULL
};

rpl_of_t *rpl_get_of0(void)
{
    return &rpl_of0;
}

void reset(rpl_dodag_t *dodag)
{
    /* Nothing to do in OF0 */
    (void) dodag;
}

network_uint16_t calc_rank(rpl_parent_t *parent, network_uint16_t base_rank)
{
    network_uint16_t net_inc_rank = byteorder_htons(0x0);

    if ( byteorder_ntohs(base_rank) == 0x0) {
        if (parent == NULL) {
            return byteorder_htons(INFINITE_RANK);
        }

        base_rank = parent->rank;
    }

    network_uint16_t add;

    if (parent != NULL) {
        add = parent->dodag->minhoprankincrease;
    }
    else {
        add = byteorder_htons(DEFAULT_MIN_HOP_RANK_INCREASE);
    }

    net_inc_rank = byteorder_htons(byteorder_ntohs(base_rank) + byteorder_ntohs(add));

    if ( byteorder_ntohs(net_inc_rank) < byteorder_ntohs(base_rank) ) {
        return byteorder_htons(INFINITE_RANK);
    }

    return net_inc_rank;
}

/* We simply return the Parent with lower rank */
rpl_parent_t *which_parent(rpl_parent_t *p1, rpl_parent_t *p2)
{
    if ( byteorder_ntohs(p1->rank) < byteorder_ntohs(p2->rank) ) {
        return p1;
    }

    return p2;
}

/* Not used yet, as the implementation only makes use of one dodag for now. */
rpl_dodag_t *which_dodag(rpl_dodag_t *d1, rpl_dodag_t *d2)
{
    (void) d2;
    return d1;
}
