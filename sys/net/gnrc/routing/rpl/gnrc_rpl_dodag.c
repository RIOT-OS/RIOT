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
#ifdef MODULE_GNRC_RPL_P2P
#include "net/gnrc/rpl/p2p.h"
#include "net/gnrc/rpl/p2p_dodag.h"
#endif

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

    /* a leaf node does not send DIOs periodically */
    if (dodag->node_status == GNRC_RPL_LEAF_NODE) {
        trickle_stop(&dodag->trickle);
        return;
    }

#ifdef MODULE_GNRC_RPL_P2P
    if (dodag->instance->mop == GNRC_RPL_P2P_MOP) {
        gnrc_rpl_p2p_ext_t *p2p_ext = gnrc_rpl_p2p_ext_get(dodag);
        if (p2p_ext && (p2p_ext->for_me || ((p2p_ext->lifetime_sec <= 0) || p2p_ext->stop))) {
            trickle_stop(&dodag->trickle);
            return;
        }
    }
#endif

    gnrc_rpl_send_DIO(inst, (ipv6_addr_t *) &ipv6_addr_all_rpl_nodes);
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
#ifdef MODULE_GNRC_RPL_P2P
    gnrc_rpl_p2p_ext_remove(dodag);
#endif
    gnrc_rpl_dodag_remove_all_parents(dodag);
    trickle_stop(&dodag->trickle);
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

bool gnrc_rpl_dodag_init(gnrc_rpl_instance_t *instance, ipv6_addr_t *dodag_id, kernel_pid_t iface,
                         gnrc_ipv6_netif_addr_t *netif_addr)
{
    /* TODO: check if netif_addr belongs to iface */

    assert(instance && (instance->state > 0));

    gnrc_rpl_dodag_t *dodag = &instance->dodag;

    dodag->dodag_id = *dodag_id;
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
    dodag->instance = instance;
    dodag->iface = iface;
    dodag->netif_addr = netif_addr;

#ifdef MODULE_GNRC_RPL_P2P
    if ((instance->mop == GNRC_RPL_P2P_MOP) && (gnrc_rpl_p2p_ext_new(dodag) == NULL)) {
        DEBUG("RPL: could not allocate new P2P-RPL DODAG extension. Remove DODAG\n");
        gnrc_rpl_instance_remove(instance);
        return false;
    }
#endif

    return true;
}

void gnrc_rpl_dodag_remove_all_parents(gnrc_rpl_dodag_t *dodag)
{
    gnrc_rpl_parent_t *elt, *tmp;
    LL_FOREACH_SAFE(dodag->parents, elt, tmp) {
        gnrc_rpl_parent_remove(elt);
    }
    dodag->my_rank = GNRC_RPL_INFINITE_RANK;
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
    assert(parent != NULL);

    gnrc_rpl_dodag_t *dodag = parent->dodag;

    if (parent == dodag->parents) {
        fib_remove_entry(&gnrc_ipv6_fib_table,
                         (uint8_t *) ipv6_addr_unspecified.u8,
                         sizeof(ipv6_addr_t));

        /* set the default route to the next parent for now */
        if (parent->next) {
            uint32_t now = xtimer_now() / SEC_IN_USEC;
            fib_add_entry(&gnrc_ipv6_fib_table,
                          dodag->iface,
                          (uint8_t *) ipv6_addr_unspecified.u8,
                          sizeof(ipv6_addr_t),
                          0x0,
                          parent->next->addr.u8,
                          sizeof(ipv6_addr_t),
                          FIB_FLAG_RPL_ROUTE,
                          (parent->next->lifetime - now) * SEC_IN_MS);
        }
    }
    LL_DELETE(dodag->parents, parent);
    memset(parent, 0, sizeof(gnrc_rpl_parent_t));
    return true;
}

void gnrc_rpl_local_repair(gnrc_rpl_dodag_t *dodag)
{
    DEBUG("RPL: [INFO] Local Repair started\n");

    dodag->dtsn++;

    if (dodag->parents) {
        gnrc_rpl_dodag_remove_all_parents(dodag);
        fib_remove_entry(&gnrc_ipv6_fib_table,
                         (uint8_t *) ipv6_addr_unspecified.u8,
                         sizeof(ipv6_addr_t));
    }

    if (dodag->my_rank != GNRC_RPL_INFINITE_RANK) {
        dodag->my_rank = GNRC_RPL_INFINITE_RANK;
        trickle_reset_timer(&dodag->trickle);
        dodag->instance->cleanup = GNRC_RPL_CLEANUP_TIME;
    }
}

void gnrc_rpl_parent_update(gnrc_rpl_dodag_t *dodag, gnrc_rpl_parent_t *parent)
{
    uint32_t now = xtimer_now();

    /* update Parent lifetime */
    if (parent != NULL) {
        parent->lifetime = (now / SEC_IN_USEC) + ((dodag->default_lifetime * dodag->lifetime_unit));
#ifdef MODULE_GNRC_RPL_P2P
        if (dodag->instance->mop != GNRC_RPL_P2P_MOP) {
#endif
        if (parent == dodag->parents) {
            fib_add_entry(&gnrc_ipv6_fib_table,
                          dodag->iface,
                          (uint8_t *) ipv6_addr_unspecified.u8,
                          sizeof(ipv6_addr_t),
                          0x00,
                          parent->addr.u8,
                          sizeof(ipv6_addr_t),
                          FIB_FLAG_RPL_ROUTE,
                          (dodag->default_lifetime * dodag->lifetime_unit) * SEC_IN_MS);
        }
#ifdef MODULE_GNRC_RPL_P2P
        }
#endif
    }

    if (_gnrc_rpl_find_preferred_parent(dodag) == NULL) {
        gnrc_rpl_local_repair(dodag);
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
    gnrc_rpl_parent_t *old_best = dodag->parents;
    gnrc_rpl_parent_t *new_best = old_best;
    uint16_t old_rank = dodag->my_rank;
    gnrc_rpl_parent_t *elt, *tmp;

    if (dodag->parents == NULL) {
        return NULL;
    }

    LL_FOREACH(dodag->parents, elt) {
        new_best = dodag->instance->of->which_parent(new_best, elt);
    }

    if (new_best->rank == GNRC_RPL_INFINITE_RANK) {
        return NULL;
    }

    if (new_best != old_best) {
        LL_DELETE(dodag->parents, new_best);
        LL_PREPEND(dodag->parents, new_best);
        /* no-path DAOs only for the storing mode */
        if ((dodag->instance->mop == GNRC_RPL_MOP_STORING_MODE_NO_MC) ||
            (dodag->instance->mop == GNRC_RPL_MOP_STORING_MODE_MC)) {
            gnrc_rpl_send_DAO(dodag->instance, &old_best->addr, 0);
            gnrc_rpl_delay_dao(dodag);
        }

#ifdef MODULE_GNRC_RPL_P2P
    if (dodag->instance->mop != GNRC_RPL_P2P_MOP) {
#endif
        fib_add_entry(&gnrc_ipv6_fib_table,
                      dodag->iface,
                      (uint8_t *) ipv6_addr_unspecified.u8,
                      sizeof(ipv6_addr_t),
                      0x00,
                      dodag->parents->addr.u8,
                      sizeof(ipv6_addr_t),
                      FIB_FLAG_RPL_ROUTE,
                      (dodag->default_lifetime * dodag->lifetime_unit) * SEC_IN_MS);
#ifdef MODULE_GNRC_RPL_P2P
    }
#endif

    }

    dodag->my_rank = dodag->instance->of->calc_rank(dodag->parents, 0);
    if (dodag->my_rank != old_rank) {
        trickle_reset_timer(&dodag->trickle);
    }

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
    kernel_pid_t iface;

    if (!(ipv6_addr_is_global(dodag_id) || ipv6_addr_is_unique_local_unicast(dodag_id))) {
        DEBUG("RPL: dodag id (%s) must be a global or unique local IPv6 address\n",
              ipv6_addr_to_str(addr_str, dodag_id, sizeof(addr_str)));
        return NULL;
    }

    if ((iface = gnrc_ipv6_netif_find_by_addr(&configured_addr, dodag_id)) == KERNEL_PID_UNDEF) {
        DEBUG("RPL: no IPv6 address configured to match the given dodag id: %s\n",
              ipv6_addr_to_str(addr_str, dodag_id, sizeof(addr_str)));
        return NULL;
    }

    if ((netif_addr = gnrc_ipv6_netif_addr_get(configured_addr)) == NULL) {
        DEBUG("RPL: no netif address found for %s\n",
              ipv6_addr_to_str(addr_str, configured_addr, sizeof(addr_str)));
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

    if (!gnrc_rpl_dodag_init(inst, dodag_id, iface, netif_addr)) {
        DEBUG("RPL: could not initialize DODAG");
        gnrc_rpl_instance_remove(inst);
        return NULL;
    }

    dodag = &inst->dodag;
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
