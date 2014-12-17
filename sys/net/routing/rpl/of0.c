/**
 * Objective function 0 for RPL implementation
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup rpl
 * @{
 * @file    of0.c
 * @brief   RPL objective function 0
 * @author  Eric Engel <eric.engel@fu-berlin.de>
 * @}
 */

#include <string.h>
#include "of0.h"

//Function Prototypes
static uint16_t calc_rank(rpl_parent_t *, uint16_t);
static rpl_parent_t *which_parent(rpl_parent_t *, rpl_parent_t *);
static rpl_dodag_t *which_dodag(rpl_dodag_t *, rpl_dodag_t *);
static void reset(rpl_dodag_t *);

rpl_of_t rpl_of0 = {
    0x0,
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

uint16_t calc_rank(rpl_parent_t *parent, uint16_t base_rank)
{
    if (base_rank == 0) {
        if (parent == NULL) {
            return INFINITE_RANK;
        }

        base_rank = parent->rank;
    }

    uint16_t add;

    if (parent != NULL) {
        add = parent->dodag->minhoprankincrease;
    }
    else {
        add = DEFAULT_MIN_HOP_RANK_INCREASE;
    }

    if (base_rank + add < base_rank) {
        return INFINITE_RANK;
    }

    return base_rank + add;
}

/* We simply return the Parent with lower rank */
rpl_parent_t *which_parent(rpl_parent_t *p1, rpl_parent_t *p2)
{
    if (p1->rank < p2->rank) {
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
