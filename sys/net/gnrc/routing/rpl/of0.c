/*
 * Copyright (C) 2014 Oliver Hahm <oliver.hahm@inria.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc_rpl
 * @{
 * @file
 * @brief       Objective Function Zero.
 *
 * Implementation of Objective Function Zero.
 *
 * @author      Eric Engel <eric.engel@fu-berlin.de>
 * @}
 */

#include <string.h>
#include "of0.h"
#include "net/gnrc/rpl.h"
#include "net/gnrc/rpl/structs.h"

static uint16_t calc_rank(gnrc_rpl_parent_t *, uint16_t);
static gnrc_rpl_parent_t *which_parent(gnrc_rpl_parent_t *, gnrc_rpl_parent_t *);
static gnrc_rpl_dodag_t *which_dodag(gnrc_rpl_dodag_t *, gnrc_rpl_dodag_t *);
static void reset(gnrc_rpl_dodag_t *);

static gnrc_rpl_of_t gnrc_rpl_of0 = {
    0x0,
    calc_rank,
    which_parent,
    which_dodag,
    reset,
    NULL,
    NULL,
    NULL
};

gnrc_rpl_of_t *gnrc_rpl_get_of0(void)
{
    return &gnrc_rpl_of0;
}

void reset(gnrc_rpl_dodag_t *dodag)
{
    /* Nothing to do in OF0 */
    (void) dodag;
}

uint16_t calc_rank(gnrc_rpl_parent_t *parent, uint16_t base_rank)
{
    if (base_rank == 0) {
        if (parent == NULL) {
            return GNRC_RPL_INFINITE_RANK;
        }

        base_rank = parent->rank;
    }

    uint16_t add;

    if (parent != NULL) {
        add = parent->dodag->instance->min_hop_rank_inc;
    }
    else {
        add = GNRC_RPL_DEFAULT_MIN_HOP_RANK_INCREASE;
    }

    if ((base_rank + add) < base_rank) {
        return GNRC_RPL_INFINITE_RANK;
    }

    return base_rank + add;
}

/* We simply return the Parent with lower rank */
gnrc_rpl_parent_t *which_parent(gnrc_rpl_parent_t *p1, gnrc_rpl_parent_t *p2)
{
    if (p1->rank <= p2->rank) {
        return p1;
    }

    return p2;
}

/* Not used yet, as the implementation only makes use of one dodag for now. */
gnrc_rpl_dodag_t *which_dodag(gnrc_rpl_dodag_t *d1, gnrc_rpl_dodag_t *d2)
{
    (void) d2;
    return d1;
}
