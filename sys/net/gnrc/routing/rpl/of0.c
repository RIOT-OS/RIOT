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

static uint16_t calc_rank(gnrc_rpl_dodag_t *, uint16_t);
static int parent_cmp(gnrc_rpl_parent_t *, gnrc_rpl_parent_t *);
static int which_dodag(gnrc_rpl_dodag_t *, gnrc_rpl_dio_t *);
static void reset(gnrc_rpl_dodag_t *);

static gnrc_rpl_of_t gnrc_rpl_of0 = {
    .ocp = 0x0,
    .calc_rank = calc_rank,
    .parent_cmp = parent_cmp,
    .which_dodag = which_dodag,
    .reset = reset,
    .parent_state_callback = NULL,
    .init = NULL,
    .process_dio = NULL
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

uint16_t calc_rank(gnrc_rpl_dodag_t *dodag, uint16_t base_rank)
{
    if (base_rank == 0) {
        if (dodag->parents == NULL) {
            return GNRC_RPL_INFINITE_RANK;
        }

        base_rank = dodag->parents->rank;
    }

    uint16_t add;

    if (dodag->parents != NULL) {
        add = dodag->instance->min_hop_rank_inc;
    }
    else {
        add = CONFIG_GNRC_RPL_DEFAULT_MIN_HOP_RANK_INCREASE;
    }

    if ((uint16_t)(base_rank + add) < base_rank) {
        return GNRC_RPL_INFINITE_RANK;
    }

    return base_rank + add;
}

int parent_cmp(gnrc_rpl_parent_t *parent1, gnrc_rpl_parent_t *parent2)
{
    if (parent1->rank < parent2->rank) {
        return -1;
    }
    else if (parent1->rank > parent2->rank) {
        return 1;
    }
    return 0;
}

int which_dodag(gnrc_rpl_dodag_t *d1, gnrc_rpl_dio_t *dio)
{
    /* RFC 6552, Section 4.2 */

    /* parent set must not be empty */
    if ((d1->node_status != GNRC_RPL_ROOT_NODE) && !d1->parents) {
        return 1;
    }

    /* prefer grounded dodag */
    int dio_grounded = dio->g_mop_prf >> GNRC_RPL_GROUNDED_SHIFT;
    if (d1->grounded > dio_grounded) {
        return -1;
    }
    else if (dio_grounded > d1->grounded) {
        return 1;
    }

    int dio_prf = dio->g_mop_prf & GNRC_RPL_PRF_MASK;

    /* prefer dodag with more preferable root */
    if (d1->prf > dio_prf) {
        return -1;
    }
    else if (dio_prf > d1->prf) {
        return 1;
    }

    /* prefer DODAG with more recent version */
    if (ipv6_addr_equal(&d1->dodag_id, &dio->dodag_id)) {
        if (GNRC_RPL_COUNTER_GREATER_THAN(d1->version, dio->version_number)) {
            return -1;
        }
        else if (GNRC_RPL_COUNTER_GREATER_THAN(dio->version_number, d1->version)) {
            return 1;
        }
    }

    /* prefer dodag with lesser resulting rank */
    /* TODO: calc rank properly */
    int d1_rank = d1->parents->rank;
    int d2_rank = byteorder_ntohs(dio->rank);
    if (d1_rank < d2_rank) {
        return -1;
    }
    else if (d2_rank < d1_rank) {
        return 1;
    }

    /* prefer DODAG for which there is an alternate parent */
    if (d1->parents->next) {
        return -1;
    }

    return 0;
}
