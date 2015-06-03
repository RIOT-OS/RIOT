/**
 * Copyright (C) 2013, 2014  INRIA.
 * Copyright (C) 2015 Cenk Gündoğan <cnkgndgn@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author      Eric Engel <eric.engel@fu-berlin.de>
 * @author      Cenk Gündoğan <cnkgndgn@gmail.com>
 */

#include <stdbool.h>
#include "net/af.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/ipv6/netif.h"
#include "net/gnrc/rpl/dodag.h"
#include "net/gnrc/rpl/structs.h"
#include "utlist.h"

#include "net/gnrc/rpl.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if ENABLE_DEBUG
static char addr_str[IPV6_ADDR_MAX_STR_LEN];
#endif

static gnrc_rpl_parent_t *_gnrc_rpl_find_preferred_parent(gnrc_rpl_dodag_t *dodag);
static void _rpl_trickle_send_dio(void *args);

static void _rpl_trickle_send_dio(void *args)
{
    gnrc_rpl_instance_t *inst = (gnrc_rpl_instance_t *) args;
    gnrc_rpl_dodag_t *dodag = &inst->dodag;
    ipv6_addr_t all_RPL_nodes = GNRC_RPL_ALL_NODES_ADDR;

    /* a leaf node does not send DIOs periodically */
    if (dodag->node_status == GNRC_RPL_LEAF_NODE) {
        trickle_stop(&dodag->trickle);
        return;
    }

    gnrc_rpl_send_DIO(inst, &all_RPL_nodes);
    DEBUG("trickle callback: Instance (%d) | DODAG: (%s)\n", inst->id,
          ipv6_addr_to_str(addr_str,&dodag->dodag_id, sizeof(addr_str)));
}

bool gnrc_rpl_instance_add(uint8_t instance_id, gnrc_rpl_instance_t **inst)
{
    *inst = NULL;
    bool first = true;
    for (uint8_t i = 0; i < GNRC_RPL_INSTANCES_NUMOF; ++i) {
        /* save position to the first unused instance */
        if ((gnrc_rpl_instances[i].state == 0) && first) {
            *inst = &gnrc_rpl_instances[i];
            first = false;
            continue;
        }
        else if ((gnrc_rpl_instances[i].state != 0) && (gnrc_rpl_instances[i].id == instance_id)) {
            DEBUG("Instance with id %d exists\n", instance_id);
            *inst = &gnrc_rpl_instances[i];
            return false;
        }
    }

    if (*inst != NULL) {
        (*inst)->id = instance_id;
        (*inst)->state = 1;
        (*inst)->max_rank_inc = GNRC_RPL_DEFAULT_MAX_RANK_INCREASE;
        (*inst)->min_hop_rank_inc = GNRC_RPL_DEFAULT_MIN_HOP_RANK_INCREASE;
        (*inst)->dodag.parents = NULL;
        return true;
    }

    /* no space available to allocate a new instance */
    DEBUG("Could not allocate a new RPL instance\n");
    *inst = NULL;
    return false;
}

bool gnrc_rpl_instance_remove_by_id(uint8_t instance_id)
{
    for(uint8_t i = 0; i < GNRC_RPL_INSTANCES_NUMOF; ++i) {
        if (gnrc_rpl_instances[i].id == instance_id) {
            return gnrc_rpl_instance_remove(&gnrc_rpl_instances[i]);
        }
    }
    return false;
}

bool gnrc_rpl_instance_remove(gnrc_rpl_instance_t *inst)
{
    gnrc_rpl_dodag_t *dodag = &inst->dodag;
    gnrc_rpl_dodag_remove_all_parents(dodag);
    trickle_stop(&dodag->trickle);
    xtimer_remove(&dodag->dao_timer);
    xtimer_remove(&dodag->cleanup_timer);
    memset(inst, 0, sizeof(gnrc_rpl_instance_t));
    return true;
}

gnrc_rpl_instance_t *gnrc_rpl_instance_get(uint8_t instance_id)
{
    for (uint8_t i = 0; i < GNRC_RPL_INSTANCES_NUMOF; ++i) {
        if ((gnrc_rpl_instances[i].state != 0) && (gnrc_rpl_instances[i].id == instance_id)) {
            return &gnrc_rpl_instances[i];
        }
    }
    return NULL;
}

bool gnrc_rpl_dodag_init(gnrc_rpl_instance_t *instance, ipv6_addr_t *dodag_id)
{
    gnrc_rpl_dodag_t *dodag = NULL;

    if ((instance == NULL) || instance->state == 0) {
        DEBUG("Instance is NULL or unused\n");
        return false;
    }

    dodag = &instance->dodag;

    dodag->dodag_id = *dodag_id;
    dodag->prefix_len = GNRC_RPL_DEFAULT_PREFIX_LEN;
    dodag->addr_preferred = GNRC_RPL_DEFAULT_PREFIX_LIFETIME;
    dodag->addr_valid = GNRC_RPL_DEFAULT_PREFIX_LIFETIME;
    dodag->my_rank = GNRC_RPL_INFINITE_RANK;
    dodag->trickle.callback.func = &_rpl_trickle_send_dio;
    dodag->trickle.callback.args = instance;
    dodag->dio_interval_doubl = GNRC_RPL_DEFAULT_DIO_INTERVAL_DOUBLINGS;
    dodag->dio_min = GNRC_RPL_DEFAULT_DIO_INTERVAL_MIN;
    dodag->dio_redun = GNRC_RPL_DEFAULT_DIO_REDUNDANCY_CONSTANT;
    dodag->default_lifetime = GNRC_RPL_DEFAULT_LIFETIME;
    dodag->lifetime_unit = GNRC_RPL_LIFETIME_UNIT;
    dodag->node_status = GNRC_RPL_NORMAL_NODE;
    dodag->dao_seq = GNRC_RPL_COUNTER_INIT;
    dodag->dtsn = 0;
    dodag->dao_ack_received = false;
    dodag->dao_counter = 0;
    dodag->dao_msg.type = GNRC_RPL_MSG_TYPE_DAO_HANDLE;
    dodag->dao_msg.content.ptr = (char *) instance;
    dodag->cleanup_time = GNRC_RPL_CLEANUP_TIME * SEC_IN_USEC;
    dodag->cleanup_msg.type = GNRC_RPL_MSG_TYPE_CLEANUP_HANDLE;
    dodag->cleanup_msg.content.ptr = (char *) instance;
    dodag->instance = instance;

    return true;
}

void gnrc_rpl_dodag_remove_all_parents(gnrc_rpl_dodag_t *dodag)
{
    gnrc_rpl_parent_t *elt, *tmp;
    LL_FOREACH_SAFE(dodag->parents, elt, tmp) {
        gnrc_rpl_parent_remove(elt);
    }
    xtimer_set_msg(&dodag->cleanup_timer, dodag->cleanup_time, &dodag->cleanup_msg, gnrc_rpl_pid);
}

bool gnrc_rpl_parent_add_by_addr(gnrc_rpl_dodag_t *dodag, ipv6_addr_t *addr,
                                 gnrc_rpl_parent_t **parent)
{
    *parent = NULL;
    bool first = true;
    for (uint8_t i = 0; i < GNRC_RPL_PARENTS_NUMOF; ++i) {
        /* save position to the first unused parent */
        if ((gnrc_rpl_parents[i].state == 0) && first) {
            *parent = &gnrc_rpl_parents[i];
            first = false;
            continue;
        }
        /* return false if parent exists */
        else if ((gnrc_rpl_parents[i].state != 0) && (gnrc_rpl_parents[i].dodag == dodag) &&
                 ipv6_addr_equal(&gnrc_rpl_parents[i].addr, addr)) {
            DEBUG("parent (%s) exists\n", ipv6_addr_to_str(addr_str, addr, sizeof(addr_str)));
            *parent = &gnrc_rpl_parents[i];
            return false;
        }
    }

    if (*parent != NULL) {
        (*parent)->dodag = dodag;
        LL_APPEND(dodag->parents, *parent);
        (*parent)->state = 1;
        (*parent)->addr = *addr;
        return true;
    }

    /* no space available to allocate a new parent */
    DEBUG("Could not allocate a new parent\n");
    *parent = NULL;
    return false;
}

bool gnrc_rpl_parent_remove(gnrc_rpl_parent_t *parent)
{
    if (parent == parent->dodag->parents) {
        ipv6_addr_t def = IPV6_ADDR_UNSPECIFIED;
        fib_remove_entry(&gnrc_ipv6_fib_table, def.u8, sizeof(ipv6_addr_t));
    }
    LL_DELETE(parent->dodag->parents, parent);
    memset(parent, 0, sizeof(gnrc_rpl_parent_t));
    return true;
}

void gnrc_rpl_local_repair(gnrc_rpl_dodag_t *dodag)
{
    DEBUG("RPL: [INFO] Local Repair started\n");

    dodag->dtsn++;

    if (dodag->parents) {
        gnrc_rpl_dodag_remove_all_parents(dodag);
        ipv6_addr_t def = IPV6_ADDR_UNSPECIFIED;
        fib_remove_entry(&gnrc_ipv6_fib_table, def.u8, sizeof(ipv6_addr_t));
    }

    if (dodag->my_rank != GNRC_RPL_INFINITE_RANK) {
        dodag->my_rank = GNRC_RPL_INFINITE_RANK;
        trickle_reset_timer(&dodag->trickle);
        xtimer_set_msg(&dodag->cleanup_timer, dodag->cleanup_time, &dodag->cleanup_msg,
                       gnrc_rpl_pid);
    }
}

void gnrc_rpl_parent_update(gnrc_rpl_dodag_t *dodag, gnrc_rpl_parent_t *parent)
{
    uint16_t old_rank = dodag->my_rank;
    uint64_t now = xtimer_now64();
    ipv6_addr_t def = IPV6_ADDR_UNSPECIFIED;

    /* update Parent lifetime */
    if (parent != NULL) {
        parent->lifetime = now + ((dodag->default_lifetime * dodag->lifetime_unit) * SEC_IN_USEC);
        if (parent == dodag->parents) {
            ipv6_addr_t all_RPL_nodes = GNRC_RPL_ALL_NODES_ADDR;
            kernel_pid_t if_id;
            if ((if_id = gnrc_ipv6_netif_find_by_addr(NULL, &all_RPL_nodes)) != KERNEL_PID_UNDEF) {
                fib_add_entry(&gnrc_ipv6_fib_table, if_id, def.u8, sizeof(ipv6_addr_t),
                              (FIB_FLAG_NET_PREFIX | 0x0),
                              parent->addr.u8, sizeof(ipv6_addr_t), FIB_FLAG_RPL_ROUTE,
                              (dodag->default_lifetime * dodag->lifetime_unit) * SEC_IN_MS);
            }
        }
    }

    if (_gnrc_rpl_find_preferred_parent(dodag) == NULL) {
        gnrc_rpl_local_repair(dodag);
    }

    if (dodag->parents && (old_rank != dodag->my_rank)) {
        trickle_reset_timer(&dodag->trickle);
    }
}

/**
 * @brief   Find the parent with the lowest rank and update the DODAG's preferred parent
 *
 * @param[in] dodag     Pointer to the DODAG
 *
 * @return  Pointer to the preferred parent, on success.
 * @return  NULL, otherwise.
 */
static gnrc_rpl_parent_t *_gnrc_rpl_find_preferred_parent(gnrc_rpl_dodag_t *dodag)
{
    ipv6_addr_t def = IPV6_ADDR_UNSPECIFIED;
    gnrc_rpl_parent_t *old_best = dodag->parents;
    gnrc_rpl_parent_t *new_best = old_best;
    uint16_t old_rank = dodag->my_rank;
    gnrc_rpl_parent_t *elt = NULL, *tmp = NULL;

    if (dodag->parents == NULL) {
        return NULL;
    }

    LL_FOREACH_SAFE(dodag->parents, elt, tmp) {
        new_best = dodag->instance->of->which_parent(new_best, elt);
    }

    if (new_best->rank == GNRC_RPL_INFINITE_RANK) {
        return NULL;
    }

    if (new_best != old_best) {
        LL_DELETE(dodag->parents, new_best);
        LL_PREPEND(dodag->parents, new_best);
        if (dodag->instance->mop != GNRC_RPL_MOP_NO_DOWNWARD_ROUTES) {
            gnrc_rpl_send_DAO(dodag->instance, &old_best->addr, 0);
            gnrc_rpl_delay_dao(dodag);
        }
        fib_remove_entry(&gnrc_ipv6_fib_table, def.u8, sizeof(ipv6_addr_t));
        ipv6_addr_t all_RPL_nodes = GNRC_RPL_ALL_NODES_ADDR;

        kernel_pid_t if_id = gnrc_ipv6_netif_find_by_addr(NULL, &all_RPL_nodes);

        if (if_id == KERNEL_PID_UNDEF) {
            DEBUG("RPL: no interface found for the parent address\n");
            return NULL;
        }

        fib_add_entry(&gnrc_ipv6_fib_table, if_id, def.u8, sizeof(ipv6_addr_t),
                      (FIB_FLAG_NET_PREFIX | 0x0), dodag->parents->addr.u8, sizeof(ipv6_addr_t),
                      FIB_FLAG_RPL_ROUTE, (dodag->default_lifetime * dodag->lifetime_unit)
                      * SEC_IN_MS);
    }

    dodag->my_rank = dodag->instance->of->calc_rank(dodag->parents, 0);
    if (dodag->my_rank != old_rank) {
        trickle_reset_timer(&dodag->trickle);
    }

    elt = NULL; tmp = NULL;
    LL_FOREACH_SAFE(dodag->parents, elt, tmp) {
        if (DAGRANK(dodag->my_rank, dodag->instance->min_hop_rank_inc)
            <= DAGRANK(elt->rank, dodag->instance->min_hop_rank_inc)) {
            gnrc_rpl_parent_remove(elt);
        }
    }

    return dodag->parents;
}

gnrc_rpl_instance_t *gnrc_rpl_root_instance_init(uint8_t instance_id, ipv6_addr_t *dodag_id,
                                                 uint8_t mop)
{
    if (gnrc_rpl_pid == KERNEL_PID_UNDEF) {
        DEBUG("RPL: RPL thread not started\n");
        return NULL;
    }

    ipv6_addr_t *configured_addr;
    gnrc_ipv6_netif_addr_t *netif_addr = NULL;
    gnrc_rpl_instance_t *inst = NULL;
    gnrc_rpl_dodag_t *dodag = NULL;

    if (!(ipv6_addr_is_global(dodag_id) || ipv6_addr_is_unique_local_unicast(dodag_id))) {
        DEBUG("RPL: dodag id (%s) must be a global or unique local IPv6 address\n",
              ipv6_addr_to_str(addr_str, dodag_id, sizeof(addr_str)));
        return NULL;
    }

    if (gnrc_ipv6_netif_find_by_addr(&configured_addr, dodag_id) == KERNEL_PID_UNDEF) {
        DEBUG("RPL: no IPv6 address configured to match the given dodag id: %s\n",
              ipv6_addr_to_str(addr_str, dodag_id, sizeof(addr_str)));
        return NULL;
    }

    if ((netif_addr = gnrc_ipv6_netif_addr_get(configured_addr)) == NULL) {
        DEBUG("RPL: no netif address found for %s\n", ipv6_addr_to_str(addr_str, configured_addr,
                sizeof(addr_str)));
        return NULL;
    }

    if (gnrc_rpl_instance_add(instance_id, &inst)) {
        inst->of = (gnrc_rpl_of_t *) gnrc_rpl_get_of_for_ocp(GNRC_RPL_DEFAULT_OCP);
        inst->mop = mop;
        inst->min_hop_rank_inc = GNRC_RPL_DEFAULT_MIN_HOP_RANK_INCREASE;
        inst->max_rank_inc = GNRC_RPL_DEFAULT_MAX_RANK_INCREASE;
    }
    else if (inst == NULL) {
        DEBUG("RPL: could not allocate memory for a new instance with id %d", instance_id);
        return NULL;
    }
    else {
        DEBUG("RPL: instance (%d) exists", instance_id);
        return NULL;
    }

    if (!gnrc_rpl_dodag_init(inst, dodag_id)) {
        DEBUG("RPL: could not initialize DODAG");
        return NULL;
    }

    dodag = &inst->dodag;
    dodag->prefix_len = netif_addr->prefix_len;
    dodag->addr_preferred = netif_addr->preferred;
    dodag->addr_valid = netif_addr->valid;
    dodag->instance = inst;

    return inst;
}

void gnrc_rpl_leaf_operation(gnrc_rpl_dodag_t *dodag)
{
    dodag->node_status = GNRC_RPL_LEAF_NODE;
    /* send INFINITE_RANK DIO to current children */
    gnrc_rpl_send_DIO(dodag->instance, NULL);
}

void gnrc_rpl_router_operation(gnrc_rpl_dodag_t *dodag)
{
    dodag->node_status = GNRC_RPL_NORMAL_NODE;
    /* announce presence to neighborhood */
    trickle_reset_timer(&dodag->trickle);
}
/**
 * @}
 */
