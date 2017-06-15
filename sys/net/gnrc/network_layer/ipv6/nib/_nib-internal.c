/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <stdbool.h>
#include <string.h>

#include "net/gnrc/ipv6.h"
#include "net/gnrc/ipv6/nib/conf.h"
#include "net/gnrc/ipv6/nib/nc.h"
#include "net/gnrc/ipv6/nib.h"
#include "net/gnrc/netif.h"
#include "random.h"

#include "_nib-internal.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/* pointers for default router selection */
static _nib_dr_entry_t *_prime_def_router = NULL;
static clist_node_t _next_removable = { NULL };

static _nib_onl_entry_t _nodes[GNRC_IPV6_NIB_NUMOF];
static _nib_dr_entry_t _def_routers[GNRC_IPV6_NIB_DEFAULT_ROUTER_NUMOF];
static _nib_iface_t _nis[GNRC_NETIF_NUMOF];

#if ENABLE_DEBUG
static char addr_str[IPV6_ADDR_MAX_STR_LEN];
#endif

mutex_t _nib_mutex = MUTEX_INIT;

static void _override_node(const ipv6_addr_t *addr, unsigned iface,
                           _nib_onl_entry_t *node);
static inline bool _node_unreachable(_nib_onl_entry_t *node);

void _nib_init(void)
{
#ifdef TEST_SUITES
    _prime_def_router = NULL;
    _next_removable.next = NULL;
    memset(_nodes, 0, sizeof(_nodes));
    memset(_def_routers, 0, sizeof(_def_routers));
    memset(_nis, 0, sizeof(_nis));
#endif
    /* TODO: load ABR information from persistent memory */
}

_nib_onl_entry_t *_nib_onl_alloc(const ipv6_addr_t *addr, unsigned iface)
{
    _nib_onl_entry_t *node = NULL;

    assert(addr != NULL);
    DEBUG("nib: Allocating on-link node entry (addr = %s, iface = %u)\n",
          ipv6_addr_to_str(addr_str, addr, sizeof(addr_str)), iface);
    for (unsigned i = 0; i < GNRC_IPV6_NIB_NUMOF; i++) {
        _nib_onl_entry_t *tmp = &_nodes[i];

        if ((_nib_onl_get_if(tmp) == iface) &&
            (ipv6_addr_equal(addr, &tmp->ipv6))) {
            /* exact match */
            DEBUG("  %p is an exact match\n", (void *)tmp);
            return tmp;
        }
        if ((node == NULL) && (tmp->mode == _EMPTY)) {
            node = tmp;
        }
    }
    if (node != NULL) {
        DEBUG("  using %p\n", (void *)node);
        _override_node(addr, iface, node);
    }
#if ENABLE_DEBUG
    else {
        DEBUG("  NIB full\n");
    }
#endif
    return node;
}

static inline bool _is_gc(_nib_onl_entry_t *node)
{
    return ((node->mode & ~(_NC)) == 0) &&
           ((node->info & GNRC_IPV6_NIB_NC_INFO_AR_STATE_MASK) ==
            GNRC_IPV6_NIB_NC_INFO_AR_STATE_GC);
}

static inline _nib_onl_entry_t *_cache_out_onl_entry(const ipv6_addr_t *addr,
                                                     unsigned iface,
                                                     uint16_t cstate)
{
    /* Use clist as FIFO for caching */
    _nib_onl_entry_t *first = (_nib_onl_entry_t *)clist_lpop(&_next_removable);
    _nib_onl_entry_t *tmp = first, *res = NULL;

    DEBUG("nib: Searching for replaceable entries (addr = %s, iface = %u)\n",
          ipv6_addr_to_str(addr_str, addr, sizeof(addr_str)), iface);
    if (tmp == NULL) {
        return NULL;
    }
    do {
        if (_is_gc(tmp)) {
            DEBUG("nib: Removing neighbor cache entry (addr = %s, "
                  "iface = %u) ",
                  ipv6_addr_to_str(addr_str, &tmp->ipv6,
                                   sizeof(addr_str)),
                  _nib_onl_get_if(tmp));
            DEBUG("for (addr = %s, iface = %u)\n",
                  ipv6_addr_to_str(addr_str, addr, sizeof(addr_str)),
                  iface);
            res = tmp;
            res->mode = _EMPTY;
            _override_node(addr, iface, res);
            /* cstate masked in _nib_nc_add() already */
            res->info |= cstate;
            res->mode = _NC;
        }
        /* requeue if not garbage collectible at the moment or queueing
         * newly created NCE */
        clist_rpush(&_next_removable, (clist_node_t *)tmp);
        if (res == NULL) {
            /* no new entry created yet, get next entry in FIFO */
            tmp = (_nib_onl_entry_t *)clist_lpop(&_next_removable);
        }
    } while ((tmp != first) && (res != NULL));
    return res;
}

_nib_onl_entry_t *_nib_nc_add(const ipv6_addr_t *addr, unsigned iface,
                              uint16_t cstate)
{
    cstate &= GNRC_IPV6_NIB_NC_INFO_NUD_STATE_MASK;
    assert(cstate != GNRC_IPV6_NIB_NC_INFO_NUD_STATE_DELAY);
    assert(cstate != GNRC_IPV6_NIB_NC_INFO_NUD_STATE_PROBE);
    assert(cstate != GNRC_IPV6_NIB_NC_INFO_NUD_STATE_REACHABLE);
    _nib_onl_entry_t *node = _nib_onl_alloc(addr, iface);
    if (node == NULL) {
        return _cache_out_onl_entry(addr, iface, cstate);
    }
    DEBUG("nib: Adding to neighbor cache (addr = %s, iface = %u)\n",
          ipv6_addr_to_str(addr_str, addr, sizeof(addr_str)), iface);
    if (!(node->mode & _NC)) {
        node->info &= ~GNRC_IPV6_NIB_NC_INFO_NUD_STATE_MASK;
        /* masked above already */
        node->info |= cstate;
        node->mode |= _NC;
    }
    if (node->next == NULL) {
        DEBUG("nib: queueing (addr = %s, iface = %u) for potential removal\n",
              ipv6_addr_to_str(addr_str, addr, sizeof(addr_str)), iface);
        /* add to next removable list, if not already in it */
        clist_rpush(&_next_removable, (clist_node_t *)node);
    }
    return node;
}

_nib_onl_entry_t *_nib_onl_iter(const _nib_onl_entry_t *last)
{
    for (const _nib_onl_entry_t *node = (last) ? last + 1 : _nodes;
         node < (_nodes + GNRC_IPV6_NIB_NUMOF);
         node++) {
        if (node->mode != _EMPTY) {
            /* const modifier provided to assure internal consistency.
             * Can now be discarded. */
            return (_nib_onl_entry_t *)node;
        }
    }
    return NULL;
}

_nib_onl_entry_t *_nib_onl_get(const ipv6_addr_t *addr, unsigned iface)
{
    assert(addr != NULL);
    DEBUG("nib: Getting on-link node entry (addr = %s, iface = %u)\n",
          ipv6_addr_to_str(addr_str, addr, sizeof(addr_str)), iface);
    for (unsigned i = 0; i < GNRC_IPV6_NIB_NUMOF; i++) {
        _nib_onl_entry_t *node = &_nodes[i];

        if ((node->mode != _EMPTY) &&
            /* either requested or current interface undefined or
             * interfaces equal */
            ((_nib_onl_get_if(node) == 0) || (iface == 0) ||
             (_nib_onl_get_if(node) == iface)) &&
            ipv6_addr_equal(&node->ipv6, addr)) {
            DEBUG("  Found %p\n", (void *)node);
            return node;
        }
    }
    DEBUG("  No suitable entry found\n");
    return NULL;
}

void _nib_nc_set_reachable(_nib_onl_entry_t *node)
{
    _nib_iface_t *iface = _nib_iface_get(_nib_onl_get_if(node));

    DEBUG("nib: set %s%%%u reachable (reachable time = %u)\n",
          ipv6_addr_to_str(addr_str, &node->ipv6, sizeof(addr_str)),
          _nib_onl_get_if(node), iface->reach_time);
    node->info &= ~GNRC_IPV6_NIB_NC_INFO_NUD_STATE_MASK;
    node->info |= GNRC_IPV6_NIB_NC_INFO_NUD_STATE_REACHABLE;
    /* TODO add event for state change to STALE to event timer*/
    (void)iface;
}

void _nib_nc_remove(_nib_onl_entry_t *node)
{
    DEBUG("nib: remove from neighbor cache (addr = %s, iface = %u)\n",
          ipv6_addr_to_str(addr_str, &node->ipv6, sizeof(addr_str)),
          _nib_onl_get_if(node));
    node->mode &= ~(_NC);
    /* TODO: remove NC related timers */
    _nib_onl_clear(node);
}

_nib_dr_entry_t *_nib_drl_add(const ipv6_addr_t *router_addr, unsigned iface)
{
    _nib_dr_entry_t *def_router = NULL;

    DEBUG("nib: Allocating default router list entry "
          "(router_addr = %s, iface = %u)\n",
          ipv6_addr_to_str(addr_str, router_addr, sizeof(addr_str)), iface);
    for (unsigned i = 0; i < GNRC_IPV6_NIB_DEFAULT_ROUTER_NUMOF; i++) {
        _nib_dr_entry_t *tmp = &_def_routers[i];
        _nib_onl_entry_t *tmp_node = tmp->next_hop;

        if ((tmp_node != NULL) &&
            (_nib_onl_get_if(tmp_node) == iface) &&
            (ipv6_addr_equal(router_addr, &tmp_node->ipv6))) {
            /* exact match */
            DEBUG("  %p is an exact match\n", (void *)tmp);
            tmp_node->mode |= _DRL;
            return tmp;
        }
        if ((def_router == NULL) && (tmp_node == NULL)) {
            def_router = tmp;
        }
    }
    if (def_router != NULL) {
        DEBUG("  using %p\n", (void *)def_router);
        def_router->next_hop = _nib_onl_alloc(router_addr, iface);

        if (def_router->next_hop == NULL) {
            return NULL;
        }
        _override_node(router_addr, iface, def_router->next_hop);
        def_router->next_hop->mode |= _DRL;
    }
    return def_router;
}

void _nib_drl_remove(_nib_dr_entry_t *nib_dr)
{
    if (nib_dr->next_hop != NULL) {
        nib_dr->next_hop->mode &= ~(_DRL);
        _nib_onl_clear(nib_dr->next_hop);
        memset(nib_dr, 0, sizeof(_nib_dr_entry_t));
    }
    if (nib_dr == _prime_def_router) {
        _prime_def_router = NULL;
    }
}

_nib_dr_entry_t *_nib_drl_iter(const _nib_dr_entry_t *last)
{
    for (const _nib_dr_entry_t *def_router = (last) ? (last + 1) : _def_routers;
         def_router < (_def_routers + GNRC_IPV6_NIB_DEFAULT_ROUTER_NUMOF);
         def_router++) {
        _nib_onl_entry_t *node = def_router->next_hop;
        if ((node != NULL) && (node->mode != _EMPTY)) {
            /* const modifier provided to assure internal consistency.
             * Can now be discarded. */
            return (_nib_dr_entry_t *)def_router;
        }
    }
    return NULL;
}

_nib_dr_entry_t *_nib_drl_get(const ipv6_addr_t *router_addr, unsigned iface)
{
    for (unsigned i = 0; i < GNRC_IPV6_NIB_DEFAULT_ROUTER_NUMOF; i++) {
        _nib_dr_entry_t *def_router = &_def_routers[i];
        _nib_onl_entry_t *node = def_router->next_hop;

        if ((node != NULL) &&
            (_nib_onl_get_if(node) == iface) &&
            (ipv6_addr_equal(router_addr, &node->ipv6))) {
            /* It is linked to the default router list so it *should* be set */
            assert(node->mode & _DRL);
            return def_router;
        }
    }
    return NULL;
}

_nib_dr_entry_t *_nib_drl_get_dr(void)
{
    _nib_dr_entry_t *ptr = NULL;

    /* if there is already a default router selected or
     * its reachability is not suspect */
    if (!((_prime_def_router == NULL) ||
         (_node_unreachable(_prime_def_router->next_hop)))) {
        /* take it */
        return _prime_def_router;
    }
    /* else search next reachable router */
    do {
        ptr = _nib_drl_iter(ptr);
        /* if there is no reachable router */
        if (ptr == NULL) {
            _nib_dr_entry_t *next = _nib_drl_iter(_prime_def_router);
            /* if first time called or last selected router is last in
             * router list */
            if ((_prime_def_router == NULL) || (next == NULL)) {
                /* wrap around to first (potentially unreachable) route
                 * to trigger NUD for it */
                _prime_def_router = _nib_drl_iter(NULL);
            }
            /* there is another default router, choose it regardless of
             * reachability to potentially trigger NUD for it */
            else if (next != NULL) {
                _prime_def_router = next;
            }
            return _prime_def_router;
        }
    } while (_node_unreachable(ptr->next_hop));
    _prime_def_router = ptr;
    return _prime_def_router;
}

_nib_iface_t *_nib_iface_get(unsigned iface)
{
    _nib_iface_t *ni = NULL;

    assert(iface <= _NIB_IF_MAX);
    for (unsigned i = 0; i < GNRC_NETIF_NUMOF; i++) {
        _nib_iface_t *tmp = &_nis[i];
        if (((unsigned)tmp->pid) == iface) {
            return tmp;
        }
        if ((ni == NULL) && (tmp->pid == KERNEL_PID_UNDEF)) {
            ni = tmp;
        }
    }
    if (ni != NULL) {
        memset(ni, 0, sizeof(_nib_iface_t));
        /* TODO: set random reachable time using constants from #6220 */
        ni->pid = (kernel_pid_t)iface;
    }
    return ni;
}

static void _override_node(const ipv6_addr_t *addr, unsigned iface,
                           _nib_onl_entry_t *node)
{
    _nib_onl_clear(node);
    memcpy(&node->ipv6, addr, sizeof(node->ipv6));
    _nib_onl_set_if(node, iface);
}

static inline bool _node_unreachable(_nib_onl_entry_t *node)
{
    switch (node->info & GNRC_IPV6_NIB_NC_INFO_NUD_STATE_MASK) {
        case GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNREACHABLE:
        /* Falls through. */
        case GNRC_IPV6_NIB_NC_INFO_NUD_STATE_INCOMPLETE:
            return true;
        default:
            return false;
    }
}

/** @} */
