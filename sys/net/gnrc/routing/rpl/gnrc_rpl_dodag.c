/**
 * Copyright (C) 2018       HAW Hamburg
 * Copyright (C) 2015–2017  Cenk Gündoğan <cenk.guendogan@haw-hamburg.de>
 * Copyright (C) 2013–2014  INRIA.
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
 * @author      Cenk Gündoğan <cenk.guendogan@haw-hamburg.de>
 */

#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "net/af.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/netif/internal.h"
#include "net/gnrc/rpl/dodag.h"
#include "net/gnrc/rpl/structs.h"
#include "gnrc_rpl_internal/globals.h"
#include "utlist.h"

#include "net/gnrc/rpl.h"
#include "net/gnrc/rpl/rpble.h"
#ifdef MODULE_GNRC_RPL_P2P
#include "net/gnrc/rpl/p2p.h"
#include "net/gnrc/rpl/p2p_dodag.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

static char addr_str[IPV6_ADDR_MAX_STR_LEN];

static gnrc_rpl_parent_t *_gnrc_rpl_find_preferred_parent(gnrc_rpl_dodag_t *dodag);

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

/* The lifetime of the default route should exceed the parent timeout interval
 * by the time we allow the node to probe its parent */
static uint16_t _dflt_route_lifetime_sec(gnrc_rpl_dodag_t *dodag)
{
    return (dodag->default_lifetime * dodag->lifetime_unit) +
           (GNRC_RPL_PARENT_TIMEOUT *
            (GNRC_RPL_PARENT_PROBE_INTERVAL / MS_PER_SEC));
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
        (*inst)->max_rank_inc = CONFIG_GNRC_RPL_DEFAULT_MAX_RANK_INCREASE;
        (*inst)->min_hop_rank_inc = CONFIG_GNRC_RPL_DEFAULT_MIN_HOP_RANK_INCREASE;
        (*inst)->dodag.parents = NULL;
        (*inst)->cleanup_event.msg.content.ptr = (*inst);
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
    evtimer_del(&gnrc_rpl_evtimer, (evtimer_event_t *)&dodag->dao_event);
    evtimer_del(&gnrc_rpl_evtimer, (evtimer_event_t *)&inst->cleanup_event);
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

bool gnrc_rpl_dodag_init(gnrc_rpl_instance_t *instance, const ipv6_addr_t *dodag_id,
                         kernel_pid_t iface)
{
    assert(instance && (instance->state > 0));

    gnrc_rpl_dodag_t *dodag = &instance->dodag;
    gnrc_netif_t *netif = gnrc_netif_get_by_pid(iface);

    dodag->dodag_id = *dodag_id;
    dodag->my_rank = GNRC_RPL_INFINITE_RANK;
    dodag->trickle.callback.func = &_rpl_trickle_send_dio;
    dodag->trickle.callback.args = instance;
    dodag->dio_interval_doubl = CONFIG_GNRC_RPL_DEFAULT_DIO_INTERVAL_DOUBLINGS;
    dodag->dio_min = CONFIG_GNRC_RPL_DEFAULT_DIO_INTERVAL_MIN;
    dodag->dio_redun = CONFIG_GNRC_RPL_DEFAULT_DIO_REDUNDANCY_CONSTANT;
    dodag->default_lifetime = CONFIG_GNRC_RPL_DEFAULT_LIFETIME;
    dodag->lifetime_unit = CONFIG_GNRC_RPL_LIFETIME_UNIT;
    dodag->node_status = GNRC_RPL_NORMAL_NODE;
    dodag->dao_seq = GNRC_RPL_COUNTER_INIT;
    dodag->dtsn = 0;
    dodag->dao_ack_received = false;
    dodag->dao_counter = 0;
    dodag->instance = instance;
    dodag->iface = iface;
    dodag->dao_event.msg.content.ptr = instance;
    dodag->dao_event.msg.type = GNRC_RPL_MSG_TYPE_DODAG_DAO_TX;

    if ((netif != NULL) && !(netif->flags & GNRC_NETIF_FLAGS_IPV6_FORWARDING)) {
        gnrc_rpl_leaf_operation(dodag);
    }
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
    gnrc_rpl_parent_t *elt = NULL;
    gnrc_rpl_parent_t *tmp = NULL;
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
        (*parent)->state = GNRC_RPL_PARENT_ACTIVE;
        (*parent)->addr = *addr;
        (*parent)->rank = GNRC_RPL_INFINITE_RANK;
        evtimer_del((evtimer_t *)(&gnrc_rpl_evtimer), (evtimer_event_t *)(&(*parent)->timeout_event));
        ((evtimer_event_t *)(&(*parent)->timeout_event))->next = NULL;
        (*parent)->timeout_event.msg.type = GNRC_RPL_MSG_TYPE_PARENT_TIMEOUT;
        (*parent)->timeout_event.msg.content.ptr = (*parent);
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
        gnrc_ipv6_nib_ft_del(NULL, 0);

        /* set the default route to the next parent for now */
        if (parent->next) {
            gnrc_ipv6_nib_ft_add(NULL, 0, &parent->next->addr, dodag->iface,
                                 _dflt_route_lifetime_sec(dodag));
        }
    }
    LL_DELETE(dodag->parents, parent);
    evtimer_del((evtimer_t *)(&gnrc_rpl_evtimer), (evtimer_event_t *)&parent->timeout_event);
    memset(parent, 0, sizeof(gnrc_rpl_parent_t));
    return true;
}

void gnrc_rpl_cleanup_start(gnrc_rpl_dodag_t *dodag)
{
    evtimer_del((evtimer_t *)(&gnrc_rpl_evtimer), (evtimer_event_t *)&dodag->instance->cleanup_event);
    ((evtimer_event_t *)&(dodag->instance->cleanup_event))->offset = CONFIG_GNRC_RPL_CLEANUP_TIME;
    dodag->instance->cleanup_event.msg.type = GNRC_RPL_MSG_TYPE_INSTANCE_CLEANUP;
    evtimer_add_msg(&gnrc_rpl_evtimer, &dodag->instance->cleanup_event, gnrc_rpl_pid);
}

void gnrc_rpl_local_repair(gnrc_rpl_dodag_t *dodag)
{
    DEBUG("RPL: [INFO] Local Repair started\n");

    dodag->dtsn++;

    if (dodag->parents) {
        gnrc_rpl_dodag_remove_all_parents(dodag);
        gnrc_ipv6_nib_ft_del(NULL, 0);
    }

    if (dodag->my_rank != GNRC_RPL_INFINITE_RANK) {
        dodag->my_rank = GNRC_RPL_INFINITE_RANK;
        trickle_reset_timer(&dodag->trickle);
        gnrc_rpl_cleanup_start(dodag);
    }
}

void gnrc_rpl_parent_update(gnrc_rpl_dodag_t *dodag, gnrc_rpl_parent_t *parent)
{
    /* update Parent lifetime */
    if ((parent != NULL) && (parent->state != GNRC_RPL_PARENT_UNUSED)) {
        parent->state = GNRC_RPL_PARENT_ACTIVE;
        evtimer_del((evtimer_t *)(&gnrc_rpl_evtimer), (evtimer_event_t *)&parent->timeout_event);
        ((evtimer_event_t *)&(parent->timeout_event))->offset = (dodag->default_lifetime - 1) * dodag->lifetime_unit * MS_PER_SEC;
        parent->timeout_event.msg.type = GNRC_RPL_MSG_TYPE_PARENT_TIMEOUT;
        evtimer_add_msg(&gnrc_rpl_evtimer, &parent->timeout_event, gnrc_rpl_pid);
#ifdef MODULE_GNRC_RPL_P2P
        if (dodag->instance->mop != GNRC_RPL_P2P_MOP) {
#endif
        if (parent == dodag->parents) {
            gnrc_ipv6_nib_ft_del(NULL, 0);
            gnrc_ipv6_nib_ft_add(NULL, 0, &parent->addr, dodag->iface,
                                 _dflt_route_lifetime_sec(dodag));
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
    gnrc_rpl_parent_t *new_best;
    uint16_t old_rank = dodag->my_rank;
    gnrc_rpl_parent_t *elt = NULL;
    gnrc_rpl_parent_t *tmp = NULL;

    if (dodag->parents == NULL) {
        return NULL;
    }

    LL_SORT(dodag->parents, dodag->instance->of->parent_cmp);

    assert(dodag->parents);
    new_best = dodag->parents;

    if (new_best->rank == GNRC_RPL_INFINITE_RANK) {
        return NULL;
    }

    if (new_best != old_best) {
        /* no-path DAOs only for the storing mode */
        if ((dodag->instance->mop == GNRC_RPL_MOP_STORING_MODE_NO_MC) ||
            (dodag->instance->mop == GNRC_RPL_MOP_STORING_MODE_MC)) {
            gnrc_rpl_send_DAO(dodag->instance, &old_best->addr, 0);
            gnrc_rpl_delay_dao(dodag);
        }

#ifdef MODULE_GNRC_RPL_P2P
    if (dodag->instance->mop != GNRC_RPL_P2P_MOP) {
#endif
        gnrc_ipv6_nib_ft_del(NULL, 0);
        gnrc_ipv6_nib_ft_add(NULL, 0, &dodag->parents->addr, dodag->iface,
                             dodag->default_lifetime * dodag->lifetime_unit);
#ifdef MODULE_GNRC_RPL_P2P
    }
#endif

    }

    dodag->my_rank = dodag->instance->of->calc_rank(dodag, 0);
    if (dodag->my_rank != old_rank) {
        trickle_reset_timer(&dodag->trickle);
        gnrc_rpl_rpble_update(dodag);
    }

    LL_FOREACH_SAFE(dodag->parents, elt, tmp) {
        if (DAGRANK(dodag->my_rank, dodag->instance->min_hop_rank_inc)
            <= DAGRANK(elt->rank, dodag->instance->min_hop_rank_inc)) {
            gnrc_rpl_parent_remove(elt);
        }
    }

    return dodag->parents;
}

gnrc_rpl_instance_t *gnrc_rpl_root_instance_init(uint8_t instance_id, const ipv6_addr_t *dodag_id,
                                                 uint8_t mop)
{
    if (gnrc_rpl_pid == KERNEL_PID_UNDEF) {
        DEBUG("RPL: RPL thread not started\n");
        return NULL;
    }

    gnrc_netif_t *netif;
    gnrc_rpl_instance_t *inst = NULL;
    gnrc_rpl_dodag_t *dodag = NULL;

    if (!(ipv6_addr_is_global(dodag_id) || ipv6_addr_is_unique_local_unicast(dodag_id))) {
        DEBUG("RPL: dodag id (%s) must be a global or unique local IPv6 address\n",
              ipv6_addr_to_str(addr_str, dodag_id, sizeof(addr_str)));
        return NULL;
    }

    if ((netif = gnrc_netif_get_by_ipv6_addr(dodag_id)) == NULL) {
        DEBUG("RPL: no IPv6 address configured to match the given dodag id: %s\n",
              ipv6_addr_to_str(addr_str, dodag_id, sizeof(addr_str)));
        return NULL;
    }

    if (gnrc_rpl_instance_add(instance_id, &inst)) {
        inst->of = (gnrc_rpl_of_t *) gnrc_rpl_get_of_for_ocp(GNRC_RPL_DEFAULT_OCP);
        inst->mop = mop;
        inst->min_hop_rank_inc = CONFIG_GNRC_RPL_DEFAULT_MIN_HOP_RANK_INCREASE;
        inst->max_rank_inc = CONFIG_GNRC_RPL_DEFAULT_MAX_RANK_INCREASE;
    }
    else if (inst == NULL) {
        DEBUG("RPL: could not allocate memory for a new instance with id %d\n", instance_id);
        return NULL;
    }
    else {
        DEBUG("RPL: instance (%d) exists\n", instance_id);
        return NULL;
    }

    if (!gnrc_rpl_dodag_init(inst, dodag_id, netif->pid)) {
        DEBUG("RPL: could not initialize DODAG\n");
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
