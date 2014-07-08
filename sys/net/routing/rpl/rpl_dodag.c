/**
 * RPL dodag implementation
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup rpl
 * @{
 * @file    rpl_dodag.c
 * @brief   RPL dodag functions
 * @author  Eric Engel <eric.engel@fu-berlin.de>
 * @}
 */

#include <string.h>
#include <string.h>
#include <stdio.h>

#include "rpl_dodag.h"
#include "trickle.h"
#include "rpl.h"

#define ENABLE_DEBUG (0)
#if ENABLE_DEBUG
char addr_str[IPV6_MAX_ADDR_STR_LEN];
#endif
#include "debug.h"

rpl_instance_t instances[RPL_MAX_INSTANCES];
rpl_dodag_t dodags[RPL_MAX_DODAGS];
rpl_parent_t parents[RPL_MAX_PARENTS];

void rpl_instances_init(void)
{
    memset(instances, 0, sizeof(rpl_instance_t) * RPL_MAX_INSTANCES);
}

rpl_instance_t *rpl_new_instance(uint8_t instanceid)
{
    rpl_instance_t *inst;
    rpl_instance_t *end ;

    for (inst = &instances[0], end = inst + RPL_MAX_INSTANCES; inst < end; inst++) {
        if (inst->used == 0) {
            memset(inst, 0, sizeof(*inst));
            inst->used = 1;
            inst->id = instanceid;
            return inst;
        }
    }

    return NULL;
}
rpl_instance_t *rpl_get_instance(uint8_t instanceid)
{
    for (int i = 0; i < RPL_MAX_INSTANCES; i++) {
        if (instances[i].used && (instances[i].id == instanceid)) {
            return &instances[i];
        }
    }

    return NULL;
}

rpl_instance_t *rpl_get_my_instance(void)
{
    for (int i = 0; i < RPL_MAX_INSTANCES; i++) {
        if (instances[i].joined) {
            return &instances[i];
        }
    }

    return NULL;
}

rpl_dodag_t *rpl_new_dodag(uint8_t instanceid, ipv6_addr_t *dodagid)
{
    rpl_instance_t *inst;
    inst = rpl_get_instance(instanceid);

    if (inst == NULL) {
        printf("Error - No instance found for id %d. This should not happen\n",
               instanceid);
        return NULL;
    }

    rpl_dodag_t *dodag;
    rpl_dodag_t *end;

    for (dodag = &dodags[0], end = dodag + RPL_MAX_DODAGS; dodag < end; dodag++) {
        if (dodag->used == 0) {
            memset(dodag, 0, sizeof(*dodag));
            dodag->instance = inst;
            dodag->my_rank = INFINITE_RANK;
            dodag->used = 1;
            memcpy(&dodag->dodag_id, dodagid, sizeof(*dodagid));
            return dodag;
        }
    }

    return NULL;

}

rpl_dodag_t *rpl_get_dodag(ipv6_addr_t *id)
{
    for (int i = 0; i < RPL_MAX_DODAGS; i++) {
        if (dodags[i].used && (rpl_equal_id(&dodags[i].dodag_id, id))) {
            return &dodags[i];
        }
    }

    return NULL;
}
rpl_dodag_t *rpl_get_my_dodag(void)
{
    for (int i = 0; i < RPL_MAX_DODAGS; i++) {
        if (dodags[i].joined) {
            return &dodags[i];
        }
    }

    return NULL;
}
void rpl_del_dodag(rpl_dodag_t *dodag)
{
    memset(dodag, 0, sizeof(*dodag));
}

void rpl_leave_dodag(rpl_dodag_t *dodag)
{
    dodag->joined = 0;
    dodag->my_preferred_parent = NULL;
    rpl_delete_all_parents();
}

bool rpl_equal_id(ipv6_addr_t *id1, ipv6_addr_t *id2)
{
    for (uint8_t i = 0; i < 4; i++) {
        if (id1->uint32[i] != id2->uint32[i]) {
            return false;
        }
    }

    return true;

}

rpl_parent_t *rpl_new_parent(rpl_dodag_t *dodag, ipv6_addr_t *address, uint16_t rank)
{
    rpl_parent_t *parent;
    rpl_parent_t *end;

    for (parent = &parents[0], end = parents + RPL_MAX_PARENTS; parent < end; parent++) {
        if (parent->used == 0) {
            memset(parent, 0, sizeof(*parent));
            parent->used = 1;
            parent->addr = *address;
            parent->rank = rank;
            parent->dodag = dodag;
            parent->lifetime = dodag->default_lifetime * dodag->lifetime_unit;
            /* dtsn is set at the end of recv_dio function */
            parent->dtsn = 0;
            return parent;
        }
    }

    rpl_delete_worst_parent();
    return rpl_new_parent(dodag, address, rank);
}

rpl_parent_t *rpl_find_parent(ipv6_addr_t *address)
{
    rpl_parent_t *parent;
    rpl_parent_t *end;

    for (parent = &parents[0], end = parents + RPL_MAX_PARENTS; parent < end; parent++) {
        if ((parent->used) && (rpl_equal_id(address, &parent->addr))) {
            return parent;
        }
    }

    return NULL;
}

void rpl_delete_parent(rpl_parent_t *parent)
{
    rpl_dodag_t *my_dodag = rpl_get_my_dodag();

    if ((my_dodag != NULL) && rpl_equal_id(&my_dodag->my_preferred_parent->addr,
                                           &parent->addr)) {
        my_dodag->my_preferred_parent = NULL;
    }

    memset(parent, 0, sizeof(*parent));
}

void rpl_delete_worst_parent(void)
{
    uint8_t worst = 0xFF;
    uint16_t max_rank = 0x0000;

    for (int i = 0; i < RPL_MAX_PARENTS; i++) {
        if (parents[i].rank > max_rank) {
            worst = i;
            max_rank = parents[i].rank;
        }
    }

    if (worst == 0xFF) {
        /* Fehler, keine parents -> sollte nicht passieren */
        return;
    }

    rpl_delete_parent(&parents[worst]);

}

void rpl_delete_all_parents(void)
{
    rpl_dodag_t *my_dodag = rpl_get_my_dodag();

    if (my_dodag != NULL) {
        my_dodag->my_preferred_parent = NULL;
    }

    for (int i = 0; i < RPL_MAX_PARENTS; i++) {
        memset(&parents[i], 0, sizeof(parents[i]));
    }
}

rpl_parent_t *rpl_find_preferred_parent(void)
{
    rpl_parent_t *best = NULL;
    rpl_dodag_t *my_dodag = rpl_get_my_dodag();

    if (my_dodag == NULL) {
        DEBUG("Not part of a dodag\n");
        return NULL;
    }

    for (uint8_t i = 0; i < RPL_MAX_PARENTS; i++) {
        if (parents[i].used) {
            if ((parents[i].rank == INFINITE_RANK) || (parents[i].lifetime <= 1)) {
                DEBUG("Infinite rank, bad parent\n");
                continue;
            }
            else if (best == NULL) {
                DEBUG("possible parent\n");
                best = &parents[i];
            }
            else {
                best = my_dodag->of->which_parent(best, &parents[i]);
            }
        }
    }

    if (best == NULL) {
        return NULL;
    }

    if (my_dodag->my_preferred_parent == NULL) {
        my_dodag->my_preferred_parent = best;
    }

    if (!rpl_equal_id(&my_dodag->my_preferred_parent->addr, &best->addr)) {
        if (my_dodag->mop != RPL_NO_DOWNWARD_ROUTES) {
            /* send DAO with ZERO_LIFETIME to old parent */
            send_DAO(&my_dodag->my_preferred_parent->addr, 0, false, 0);
        }

        my_dodag->my_preferred_parent = best;

        if (my_dodag->mop != RPL_NO_DOWNWARD_ROUTES) {
            delay_dao();
        }

        reset_trickletimer();
    }

    return best;
}

void rpl_parent_update(rpl_parent_t *parent)
{
    rpl_dodag_t *my_dodag = rpl_get_my_dodag();
    uint16_t old_rank;

    if (my_dodag == NULL) {
        DEBUG("Not part of a dodag - this should not happen");
        return;
    }

    old_rank = my_dodag->my_rank;

    /* update Parent lifetime */
    if (parent != NULL) {
        parent->lifetime = my_dodag->default_lifetime * my_dodag->lifetime_unit;
    }

    if (rpl_find_preferred_parent() == NULL) {
        rpl_local_repair();
    }

    if (rpl_calc_rank(old_rank, my_dodag->minhoprankincrease) !=
        rpl_calc_rank(my_dodag->my_rank, my_dodag->minhoprankincrease)) {
        if (my_dodag->my_rank < my_dodag->min_rank) {
            my_dodag->min_rank = my_dodag->my_rank;
        }

        reset_trickletimer();
    }
}

void rpl_join_dodag(rpl_dodag_t *dodag, ipv6_addr_t *parent, uint16_t parent_rank)
{
    rpl_dodag_t *my_dodag;
    rpl_parent_t *preferred_parent;
    my_dodag = rpl_new_dodag(dodag->instance->id, &dodag->dodag_id);

    if (my_dodag == NULL) {
        return;
    }

    preferred_parent = rpl_new_parent(dodag, parent, parent_rank);

    if (preferred_parent == NULL) {
        rpl_del_dodag(my_dodag);
        return;
    }

    my_dodag->instance->joined = 1;
    my_dodag->of = dodag->of;
    my_dodag->mop = dodag->mop;
    my_dodag->dtsn = dodag->dtsn;
    my_dodag->prf = dodag->prf;
    my_dodag->dio_interval_doubling = dodag->dio_interval_doubling;
    my_dodag->dio_min = dodag->dio_min;
    my_dodag->dio_redundancy = dodag->dio_redundancy;
    my_dodag->maxrankincrease = dodag->maxrankincrease;
    my_dodag->minhoprankincrease = dodag->minhoprankincrease;
    my_dodag->default_lifetime = dodag->default_lifetime;
    my_dodag->lifetime_unit = dodag->lifetime_unit;
    my_dodag->version = dodag->version;
    my_dodag->grounded = dodag->grounded;
    my_dodag->joined = 1;
    my_dodag->my_preferred_parent = preferred_parent;
    my_dodag->node_status = (uint8_t) NORMAL_NODE;
    my_dodag->my_rank = dodag->of->calc_rank(preferred_parent, dodag->my_rank);
    my_dodag->dao_seq = RPL_COUNTER_INIT;
    my_dodag->min_rank = my_dodag->my_rank;
    DEBUG("Joint DODAG:\n");
    DEBUG("\tMOP:\t%02X\n", my_dodag->mop);
    DEBUG("\tminhoprankincrease :\t%04X\n", my_dodag->minhoprankincrease);
    DEBUG("\tdefault_lifetime:\t%02X\n", my_dodag->default_lifetime);
    DEBUG("\tgrounded:\t%02X\n", my_dodag->grounded);
    DEBUG("\tmy_preferred_parent:\t%s\n",
          ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN,
                           &my_dodag->my_preferred_parent->addr));
    DEBUG("\tmy_preferred_parent rank\t%02X\n", my_dodag->my_preferred_parent->rank);
    DEBUG("\tmy_preferred_parent lifetime\t%04X\n", my_dodag->my_preferred_parent->lifetime);

    start_trickle(my_dodag->dio_min, my_dodag->dio_interval_doubling, my_dodag->dio_redundancy);
    delay_dao();
}

void rpl_global_repair(rpl_dodag_t *dodag, ipv6_addr_t *p_addr, uint16_t rank)
{
    puts("[INFO] Global repair started");
    rpl_dodag_t *my_dodag = rpl_get_my_dodag();

    if (my_dodag == NULL) {
        printf("[Error] - no global repair possible, if not part of a DODAG\n");
        return;
    }

    rpl_delete_all_parents();
    my_dodag->version = dodag->version;
    my_dodag->dtsn++;
    my_dodag->my_preferred_parent = rpl_new_parent(my_dodag, p_addr, rank);

    if (my_dodag->my_preferred_parent == NULL) {
        printf("[Error] no more parent after global repair\n");
        my_dodag->my_rank = INFINITE_RANK;
    }
    else {
        /* Calc new Rank */
        my_dodag->my_rank = my_dodag->of->calc_rank(my_dodag->my_preferred_parent,
                            my_dodag->my_rank);
        my_dodag->min_rank = my_dodag->my_rank;
        reset_trickletimer();
        delay_dao();
    }

    printf("Migrated to DODAG Version %d. My new Rank: %d\n", my_dodag->version,
           my_dodag->my_rank);
}

void rpl_local_repair(void)
{
    puts("[INFO] Local Repair started");
    rpl_dodag_t *my_dodag = rpl_get_my_dodag();

    if (my_dodag == NULL) {
        printf("[Error] - no local repair possible, if not part of a DODAG\n");
        return;
    }

    my_dodag->my_rank = INFINITE_RANK;
    my_dodag->dtsn++;
    rpl_delete_all_parents();
    reset_trickletimer();

}

ipv6_addr_t *rpl_get_my_preferred_parent(void)
{
    rpl_dodag_t *my_dodag = rpl_get_my_dodag();

    if (my_dodag == NULL) {
        return NULL;
    }

    return &my_dodag->my_preferred_parent->addr;
}

uint16_t rpl_calc_rank(uint16_t abs_rank, uint16_t minhoprankincrease)
{
    return abs_rank / minhoprankincrease;
}
