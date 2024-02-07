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

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <kernel_defines.h>

#include "net/gnrc/icmpv6/error.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/ipv6/nib/conf.h"
#include "net/gnrc/ipv6/nib/nc.h"
#include "net/gnrc/ipv6/nib.h"
#include "net/gnrc/netif/internal.h"
#include "net/ipv6/addr.h"
#include "random.h"

#include "_nib-internal.h"
#include "_nib-router.h"
#include "_nib-slaac.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* pointers for default router selection */
_nib_dr_entry_t *_prime_def_router = NULL;
static clist_node_t _next_removable = { NULL };

static _nib_onl_entry_t _nodes[CONFIG_GNRC_IPV6_NIB_NUMOF];
static _nib_offl_entry_t _dsts[CONFIG_GNRC_IPV6_NIB_OFFL_NUMOF];
static _nib_dr_entry_t _def_routers[CONFIG_GNRC_IPV6_NIB_DEFAULT_ROUTER_NUMOF];

#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_MULTIHOP_P6C)
static _nib_abr_entry_t _abrs[CONFIG_GNRC_IPV6_NIB_ABR_NUMOF];
#endif  /* CONFIG_GNRC_IPV6_NIB_MULTIHOP_P6C */
static rmutex_t _nib_mutex = RMUTEX_INIT;

static char addr_str[IPV6_ADDR_MAX_STR_LEN];

evtimer_msg_t _nib_evtimer;

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
    memset(_dsts, 0, sizeof(_dsts));
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_MULTIHOP_P6C)
    memset(_abrs, 0, sizeof(_abrs));
#endif  /* CONFIG_GNRC_IPV6_NIB_MULTIHOP_P6C */
#endif  /* TEST_SUITES */
    evtimer_init_msg(&_nib_evtimer);
    /* TODO: load ABR information from persistent memory */
}

void _nib_acquire(void)
{
    rmutex_lock(&_nib_mutex);
}

void _nib_release(void)
{
    rmutex_unlock(&_nib_mutex);
}

static inline bool _addr_equals(const ipv6_addr_t *addr,
                                const _nib_onl_entry_t *node)
{
    if (addr == NULL) {
        return ipv6_addr_is_unspecified(&node->ipv6);
    } else {
        return ipv6_addr_equal(addr, &node->ipv6);
    }
}

_nib_onl_entry_t *_nib_onl_alloc(const ipv6_addr_t *addr, unsigned iface)
{
    _nib_onl_entry_t *node = NULL;

    DEBUG("nib: Allocating on-link node entry (addr = %s, iface = %u)\n",
          (addr == NULL) ? "NULL" : ipv6_addr_to_str(addr_str, addr,
                                                     sizeof(addr_str)), iface);
    for (unsigned i = 0; i < CONFIG_GNRC_IPV6_NIB_NUMOF; i++) {
        _nib_onl_entry_t *tmp = &_nodes[i];

        if ((_nib_onl_get_if(tmp) == iface) && _addr_equals(addr, tmp)) {
            /* exact match */
            DEBUG("  %p is an exact match\n", (void *)tmp);
            node = tmp;
            break;
        }
        if ((node == NULL) && (tmp->mode == _EMPTY)) {
            DEBUG("  using %p\n", (void *)node);
            node = tmp;
        }
    }
    if (node != NULL) {
        _override_node(addr, iface, node);
    }
    else {
        DEBUG("  NIB full\n");
    }
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
            /* call _nib_nc_remove to remove timers from _evtimer */
            _nib_nc_remove(tmp);
            res = tmp;
            _override_node(addr, iface, res);
            /* cstate masked in _nib_nc_add() already */
            res->info |= cstate;
            res->mode = _NC;
        }
        /* requeue if not garbage collectible at the moment or queueing
         * newly created NCE or in case entry becomes garbage collectible
         * again */
        clist_rpush(&_next_removable, (clist_node_t *)tmp);
        if (res == NULL) {
            /* no new entry created yet, get next entry in FIFO */
            tmp = (_nib_onl_entry_t *)clist_lpop(&_next_removable);
        }
    } while ((tmp != first) && (res == NULL));
    if (res == NULL) {
        /* we did not find any removable entry => requeue current one */
        clist_rpush(&_next_removable, (clist_node_t *)tmp);
    }
    return res;
}

_nib_onl_entry_t *_nib_nc_add(const ipv6_addr_t *addr, unsigned iface,
                              uint16_t cstate)
{
    assert(addr != NULL);
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
         node < (_nodes + CONFIG_GNRC_IPV6_NIB_NUMOF);
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
    for (unsigned i = 0; i < CONFIG_GNRC_IPV6_NIB_NUMOF; i++) {
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
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_ARSM)
    gnrc_netif_t *netif = gnrc_netif_get_by_pid(_nib_onl_get_if(node));

    node->info &= ~GNRC_IPV6_NIB_NC_INFO_NUD_STATE_MASK;
    node->info |= GNRC_IPV6_NIB_NC_INFO_NUD_STATE_REACHABLE;
#ifdef TEST_SUITES
    /* exit early for unittests */
    if (netif == NULL) {
        return;
    }
#endif  /* TEST_SUITES */
    DEBUG("nib: set %s%%%u reachable (reachable time = %u)\n",
          ipv6_addr_to_str(addr_str, &node->ipv6, sizeof(addr_str)),
          _nib_onl_get_if(node), (unsigned)netif->ipv6.reach_time);
    _evtimer_add(node, GNRC_IPV6_NIB_REACH_TIMEOUT, &node->nud_timeout,
                 netif->ipv6.reach_time);
#else   /* CONFIG_GNRC_IPV6_NIB_ARSM */
    (void)node;
#endif  /* CONFIG_GNRC_IPV6_NIB_ARSM */
}

void _nib_nc_remove(_nib_onl_entry_t *node)
{
    DEBUG("nib: remove from neighbor cache (addr = %s, iface = %u)\n",
          ipv6_addr_to_str(addr_str, &node->ipv6, sizeof(addr_str)),
          _nib_onl_get_if(node));
    node->mode &= ~(_NC);
    evtimer_del((evtimer_t *)&_nib_evtimer, &node->snd_na.event);
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_ARSM)
    evtimer_del((evtimer_t *)&_nib_evtimer, &node->nud_timeout.event);
#endif  /* CONFIG_GNRC_IPV6_NIB_ARSM */
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_ROUTER)
    evtimer_del((evtimer_t *)&_nib_evtimer, &node->reply_rs.event);
#endif  /* CONFIG_GNRC_IPV6_NIB_ROUTER */
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_6LR)
    evtimer_del((evtimer_t *)&_nib_evtimer, &node->addr_reg_timeout.event);
#endif  /* CONFIG_GNRC_IPV6_NIB_6LR */
    _nbr_flush_pktqueue(node);
    /* remove from cache-out procedure */
    clist_remove(&_next_removable, (clist_node_t *)node);
    _nib_onl_clear(node);
}

#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_6LN) || !IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_ARSM)
static inline int _get_l2addr_from_ipv6(const gnrc_netif_t *netif,
                                        const _nib_onl_entry_t *node,
                                        gnrc_ipv6_nib_nc_t *nce)
{
    int res = gnrc_netif_ipv6_iid_to_addr(netif,
                                          (eui64_t *)&node->ipv6.u64[1],
                                          nce->l2addr);
    if (res >= 0) {
        DEBUG("nib: resolve address %s%%%u by reverse translating to ",
              ipv6_addr_to_str(addr_str, &node->ipv6, sizeof(addr_str)),
              (unsigned)netif->pid);
        nce->l2addr_len = res;
    }
    return res;
}
#endif /* CONFIG_GNRC_IPV6_NIB_6LN || !CONFIG_GNRC_IPV6_NIB_ARSM */

void _nib_nc_get(const _nib_onl_entry_t *node, gnrc_ipv6_nib_nc_t *nce)
{
    assert((node != NULL) && (nce != NULL));
    memcpy(&nce->ipv6, &node->ipv6, sizeof(nce->ipv6));
    nce->info = node->info;
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_ARSM)
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_6LN)
    if (ipv6_addr_is_link_local(&nce->ipv6)) {
        gnrc_netif_t *netif = gnrc_netif_get_by_pid(_nib_onl_get_if(node));
        assert(netif != NULL);
        (void)netif;    /* flag-checkers might evaluate just to constants */
        if (gnrc_netif_is_6ln(netif) && !gnrc_netif_is_rtr(netif) &&
            (_get_l2addr_from_ipv6(netif, node, nce) >= 0)) {
            return;
        }
    }
#endif  /* CONFIG_GNRC_IPV6_NIB_6LN */
    nce->l2addr_len = node->l2addr_len;
    memcpy(&nce->l2addr, &node->l2addr, node->l2addr_len);
#else   /* CONFIG_GNRC_IPV6_NIB_ARSM */
    gnrc_netif_t *netif = gnrc_netif_get_by_pid(_nib_onl_get_if(node));
    assert(ipv6_addr_is_link_local(&nce->ipv6));
    assert(netif != NULL);
    _get_l2addr_from_ipv6(netif, node, nce);
#endif  /* CONFIG_GNRC_IPV6_NIB_ARSM */
}

_nib_dr_entry_t *_nib_drl_add(const ipv6_addr_t *router_addr, unsigned iface)
{
    _nib_dr_entry_t *def_router = NULL;

    DEBUG("nib: Allocating default router list entry "
          "(router_addr = %s, iface = %u)\n",
          ipv6_addr_to_str(addr_str, router_addr, sizeof(addr_str)), iface);
    for (unsigned i = 0; i < CONFIG_GNRC_IPV6_NIB_DEFAULT_ROUTER_NUMOF; i++) {
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
        _evtimer_del(&nib_dr->rtr_timeout);
        nib_dr->next_hop->mode &= ~(_DRL);
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_DC)
        /*  When removing a router from the Default
            Router list, the node MUST update the Destination Cache in such a way
            that all entries using the router perform next-hop determination
            again rather than continue sending traffic to the (deleted) router.
            (https://datatracker.ietf.org/doc/html/rfc4861#section-6.3.5)
        */
        _nib_offl_entry_t *dc = NULL;
        while ((dc = _nib_offl_iter(dc))) {
            if ((dc->mode & _DC) && dc->next_hop == nib_dr->next_hop) {
                _nib_dc_remove(dc);
            }
        }
#endif
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
         def_router < (_def_routers + CONFIG_GNRC_IPV6_NIB_DEFAULT_ROUTER_NUMOF);
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
    assert((router_addr != NULL) || (iface != 0));
    for (unsigned i = 0; i < CONFIG_GNRC_IPV6_NIB_DEFAULT_ROUTER_NUMOF; i++) {
        _nib_dr_entry_t *def_router = &_def_routers[i];
        _nib_onl_entry_t *node = def_router->next_hop;

        if ((node != NULL) &&
            ((iface == 0) || (_nib_onl_get_if(node) == iface)) &&
            ((router_addr == NULL) ||
             ipv6_addr_equal(router_addr, &node->ipv6))) {
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

void _nib_drl_ft_get(const _nib_dr_entry_t *drl, gnrc_ipv6_nib_ft_t *fte)
{
    assert((drl != NULL) && (drl->next_hop != NULL) && (fte != NULL));
    ipv6_addr_set_unspecified(&fte->dst);
    fte->dst_len = 0;
    fte->primary = ((drl == _prime_def_router) &&
                    !((_prime_def_router == NULL) ||
                      (_node_unreachable(_prime_def_router->next_hop))));
    memcpy(&fte->next_hop, &drl->next_hop->ipv6, sizeof(fte->next_hop));
    fte->iface = _nib_onl_get_if(drl->next_hop);
}

_nib_offl_entry_t *_nib_offl_alloc(const ipv6_addr_t *next_hop, unsigned iface,
                                   const ipv6_addr_t *pfx, unsigned pfx_len)
{
    _nib_offl_entry_t *dst = NULL;

    assert((pfx != NULL) && (!ipv6_addr_is_unspecified(pfx)) &&
           (pfx_len > 0) && (pfx_len <= 128));
    DEBUG("nib: Allocating off-link-entry entry "
          "(next_hop = %s, iface = %u, ",
          (next_hop == NULL) ? "NULL" : ipv6_addr_to_str(addr_str, next_hop,
                                                         sizeof(addr_str)),
          iface);
    DEBUG("pfx = %s/%u)\n", ipv6_addr_to_str(addr_str, pfx,
                                             sizeof(addr_str)), pfx_len);
    for (unsigned i = 0; i < CONFIG_GNRC_IPV6_NIB_OFFL_NUMOF; i++) {
        _nib_offl_entry_t *tmp = &_dsts[i];
        _nib_onl_entry_t *tmp_node = tmp->next_hop;

        if (tmp->mode == _EMPTY) {
            if (dst == NULL) {
                dst = tmp;
            }
            continue;
        }

        /* else: offlink entry not empty, potential match */
        if (tmp->pfx_len == pfx_len && ipv6_addr_match_prefix(&tmp->pfx, pfx) >= pfx_len) {
            /* prefix matches */
            assert(tmp_node);
            if (_nib_onl_get_if(tmp_node) == iface && (ipv6_addr_is_unspecified(&tmp_node->ipv6)
                                                       || _addr_equals(next_hop, tmp_node))) {
                /* next hop matches or is unspecified */
                DEBUG("  %p is an exact match\n", (void *)tmp);
                if (next_hop != NULL) {
                    /* sets next_hop if it was previously unspecified */
                    memcpy(&tmp_node->ipv6, next_hop, sizeof(tmp_node->ipv6));
                }
                /*mark that this NCE is used by an offl_entry*/
                tmp->next_hop->mode |= _DST;
                return tmp;
            }
        }
    }
    if (dst != NULL) {
        DEBUG("  using %p\n", (void *)dst);
        if (!dst->next_hop && !(dst->next_hop = _nib_onl_alloc(next_hop, iface))) {
            memset(dst, 0, sizeof(_nib_offl_entry_t));
            return NULL;
        }
        _override_node(next_hop, iface, dst->next_hop);
        dst->next_hop->mode |= _DST;
        ipv6_addr_init_prefix(&dst->pfx, pfx, pfx_len);
        dst->pfx_len = pfx_len;
    }
    return dst;
}

static inline bool _in_dsts(const _nib_offl_entry_t *dst)
{
    return (dst < (_dsts + CONFIG_GNRC_IPV6_NIB_OFFL_NUMOF));
}

#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_MULTIHOP_P6C)
static inline unsigned _idx_dsts(const _nib_offl_entry_t *dst)
{
    return (dst - _dsts);
}

static inline bool _in_abrs(const _nib_abr_entry_t *abr)
{
    return (abr < (_abrs + CONFIG_GNRC_IPV6_NIB_ABR_NUMOF));
}
#endif  /* CONFIG_GNRC_IPV6_NIB_MULTIHOP_P6C */

void _nib_offl_clear(_nib_offl_entry_t *dst)
{
    if (dst->mode == _EMPTY) {
        if (dst->next_hop != NULL) {
            _nib_offl_entry_t *ptr;
            for (ptr = _dsts; _in_dsts(ptr); ptr++) {
                /* there is another dst pointing to next-hop => only remove dst */
                if ((dst != ptr) && (dst->next_hop == ptr->next_hop)) {
                    break;
                }
            }
            /* we iterated and found no further dst pointing to next-hop */
            if (!_in_dsts(ptr)) {
                dst->next_hop->mode &= ~(_DST);
                _nib_onl_clear(dst->next_hop);
            }
        }
        memset(dst, 0, sizeof(_nib_offl_entry_t));
    }
    else {
        DEBUG("nib: offlink entry %s/%u with mode %u not cleared\n",
              ipv6_addr_to_str(addr_str, &dst->pfx, sizeof(addr_str)),
              dst->pfx_len, dst->mode);
    }
}

_nib_offl_entry_t *_nib_offl_iter(const _nib_offl_entry_t *last)
{
    for (const _nib_offl_entry_t *dst = (last) ? (last + 1) : _dsts;
         _in_dsts(dst);
         dst++) {
        if (dst->mode != _EMPTY) {
            /* const modifier provided to assure internal consistency.
             * Can now be discarded. */
            return (_nib_offl_entry_t *)dst;
        }
    }
    return NULL;
}

bool _nib_offl_is_entry(const _nib_offl_entry_t *entry)
{
    return (entry >= _dsts) && _in_dsts(entry);
}

static _nib_offl_entry_t *_nib_offl_get_match(const ipv6_addr_t *dst)
{
    _nib_offl_entry_t *res = NULL;
    uint8_t best_match = 0;

    DEBUG("nib: get match for destination %s from NIB\n",
          ipv6_addr_to_str(addr_str, dst, sizeof(addr_str)));
    for (_nib_offl_entry_t *entry = _dsts; _in_dsts(entry); entry++) {
        if (entry->mode != _EMPTY) {
            uint8_t match = ipv6_addr_match_prefix(&entry->pfx, dst);

            DEBUG("nib: %s/%u => ",
                  ipv6_addr_to_str(addr_str, &entry->pfx, sizeof(addr_str)),
                  entry->pfx_len);
            DEBUG("%s%%%u matches with %u bits\n",
                  (entry->mode == _PL) ? "(nil)" :
                  ipv6_addr_to_str(addr_str, &entry->next_hop->ipv6,
                                   sizeof(addr_str)),
                  _nib_onl_get_if(entry->next_hop), match);
            if ((match > best_match) && (match >= entry->pfx_len)) {
                DEBUG("nib: best match (%u bits)\n", match);
                res = entry;
                best_match = match;
            }
        }
    }
    return res;
}

void _nib_ft_get(const _nib_offl_entry_t *dst, gnrc_ipv6_nib_ft_t *fte)
{
    assert((dst != NULL) && (dst->next_hop != NULL) && (fte != NULL));
    memcpy(&fte->dst, &dst->pfx, sizeof(dst->pfx));
    fte->dst_len = dst->pfx_len;
    fte->primary = 0;
    fte->iface = _nib_onl_get_if(dst->next_hop);
    if (dst->mode == _PL) { /* entry is only in prefix list */
        ipv6_addr_set_unspecified(&fte->next_hop);
    }
    else {
        memcpy(&fte->next_hop, &dst->next_hop->ipv6, sizeof(dst->next_hop->ipv6));
    }
}

int _nib_get_route(const ipv6_addr_t *dst, gnrc_pktsnip_t *pkt,
                   gnrc_ipv6_nib_ft_t *fte)
{
    assert((dst != NULL) && (fte != NULL));
    DEBUG("nib: get route %s for packet %p\n",
          ipv6_addr_to_str(addr_str, dst, sizeof(addr_str)),
          (void *)pkt);
    _nib_offl_entry_t *offl = _nib_offl_get_match(dst);

    if ((offl == NULL) ||
        /* give default route precedence over off-link PLEs */
        ((offl->mode == _PL) && !(offl->flags & _PFX_ON_LINK))) {
        _nib_dr_entry_t *router = _nib_drl_get_dr();

        if ((router == NULL) && (offl == NULL)) {
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_ROUTER)
            gnrc_netif_t *ptr = NULL;

            while ((ptr = gnrc_netif_iter(ptr))) {
                _call_route_info_cb(ptr,
                                    GNRC_IPV6_NIB_ROUTE_INFO_TYPE_RRQ,
                                    dst, pkt);
            }
#else   /* CONFIG_GNRC_IPV6_NIB_ROUTER */
            (void)pkt;
#endif  /* CONFIG_GNRC_IPV6_NIB_ROUTER */
            return -ENETUNREACH;
        }
        else if (router != NULL) {
            DEBUG("nib: prefer default router %s%%%u over prefix list entry\n",
                  ipv6_addr_to_str(addr_str, &router->next_hop->ipv6,
                                   sizeof(addr_str)),
                  _nib_onl_get_if(router->next_hop));
            _nib_drl_ft_get(router, fte);
            return 0;
        }
    }
    _nib_ft_get(offl, fte);
    return 0;
}

void _nib_pl_remove(_nib_offl_entry_t *nib_offl)
{
    _evtimer_del(&nib_offl->pfx_timeout);
    _nib_offl_remove(nib_offl, _PL);
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_MULTIHOP_P6C)
    unsigned idx = _idx_dsts(nib_offl);
    if (idx < CONFIG_GNRC_IPV6_NIB_OFFL_NUMOF) {
        for (_nib_abr_entry_t *abr = _abrs; _in_abrs(abr); abr++) {
            if (bf_isset(abr->pfxs, idx)) {
                DEBUG("nib: Removing prefix %s/%u ",
                      ipv6_addr_to_str(addr_str, &nib_offl->pfx,
                                       sizeof(addr_str)),
                      nib_offl->pfx_len);
                DEBUG("from border router %s\n",
                      ipv6_addr_to_str(addr_str, &abr->addr, sizeof(addr_str)));
                bf_unset(abr->pfxs, idx);
            }
        }
    }
#endif  /* CONFIG_GNRC_IPV6_NIB_MULTIHOP_P6C */
}

void _nib_offl_remove_prefix(_nib_offl_entry_t *pfx)
{
    gnrc_netif_t *netif;

    /* remove prefix timer */
    evtimer_del(&_nib_evtimer, &pfx->pfx_timeout.event);

    /* get interface associated with prefix */
    netif = gnrc_netif_get_by_pid(_nib_onl_get_if(pfx->next_hop));

    if (netif != NULL) {
        uint8_t best_match_len = pfx->pfx_len;
        ipv6_addr_t *best_match = NULL;

        /* remove address associated with prefix */
        for (int i = 0; i < CONFIG_GNRC_NETIF_IPV6_ADDRS_NUMOF; i++) {
            if (ipv6_addr_match_prefix(&netif->ipv6.addrs[i],
                                       &pfx->pfx) >= best_match_len) {
                best_match_len = pfx->pfx_len;
                best_match = &netif->ipv6.addrs[i];
            }
        }
        if (best_match != NULL) {
            gnrc_netif_ipv6_addr_remove_internal(netif,
                                                 best_match);
        }
    }

    /* remove prefix */
    _nib_pl_remove(pfx);
}

#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_MULTIHOP_P6C)
_nib_abr_entry_t *_nib_abr_add(const ipv6_addr_t *addr)
{
    _nib_abr_entry_t *abr = NULL;

    assert(addr != NULL);
    DEBUG("nib: Allocating authoritative border router entry (addr = %s)\n",
          ipv6_addr_to_str(addr_str, addr, sizeof(addr_str)));
    for (unsigned i = 0; i < CONFIG_GNRC_IPV6_NIB_ABR_NUMOF; i++) {
        _nib_abr_entry_t *tmp = &_abrs[i];

        if (ipv6_addr_equal(addr, &tmp->addr)) {
            /* exact match */
            DEBUG("  %p is an exact match\n", (void *)tmp);
            return tmp;
        }
        if ((abr == NULL) && (ipv6_addr_is_unspecified(&tmp->addr))) {
            abr = tmp;
        }
    }
    if (abr != NULL) {
        DEBUG("  using %p\n", (void *)abr);
        memcpy(&abr->addr, addr, sizeof(abr->addr));
    }
    else {
        DEBUG("  NIB full\n");
    }
    return abr;
}

void _nib_abr_remove(const ipv6_addr_t *addr)
{
    assert(addr != NULL);
    DEBUG("nib: Removing border router %s\n", ipv6_addr_to_str(addr_str, addr,
                                                               sizeof(addr_str)));
    for (_nib_abr_entry_t *abr = _abrs; _in_abrs(abr); abr++) {
        if (ipv6_addr_equal(addr, &abr->addr)) {
            for (int i = 0; i < CONFIG_GNRC_IPV6_NIB_OFFL_NUMOF; i++) {
                if (bf_isset(abr->pfxs, i)) {
                    _nib_pl_remove(&_dsts[i]);
                }
            }
#if MODULE_GNRC_SIXLOWPAN_CTX
            for (int i = 0; i < GNRC_SIXLOWPAN_CTX_SIZE; i++) {
                if (bf_isset(abr->ctxs, i)) {
                    gnrc_sixlowpan_ctx_remove(i);
                }
            }
#endif  /* MODULE_GNRC_SIXLOWPAN_CTX */
            memset(abr, 0, sizeof(_nib_abr_entry_t));
        }
    }
}

void _nib_abr_add_pfx(_nib_abr_entry_t *abr, const _nib_offl_entry_t *offl)
{
    assert((abr != NULL) && (offl != NULL) && (offl->mode & _PL));
    unsigned idx = _idx_dsts(offl);

    DEBUG("nib: Prefix %s/%u ",
          ipv6_addr_to_str(addr_str, &offl->pfx, sizeof(addr_str)),
          offl->pfx_len);
    DEBUG("came from border router %s\n", ipv6_addr_to_str(addr_str, &abr->addr,
                                                           sizeof(addr_str)));
    if (idx < CONFIG_GNRC_IPV6_NIB_OFFL_NUMOF) {
        bf_set(abr->pfxs, idx);
    }
}

_nib_offl_entry_t *_nib_abr_iter_pfx(const _nib_abr_entry_t *abr,
                                     const _nib_offl_entry_t *last)
{
    if ((last == NULL) ||
        (_idx_dsts(last) < CONFIG_GNRC_IPV6_NIB_OFFL_NUMOF)) {
        /* we don't change `ptr`, so dropping const qualifier for now is okay */
        _nib_offl_entry_t *ptr = (_nib_offl_entry_t *)last;

        while ((ptr = _nib_offl_iter(ptr))) {
            /* bf_isset() discards const, but doesn't change the array, so
             * discarding it on purpose */
            if ((ptr->mode & _PL) && (bf_isset((uint8_t *)abr->pfxs, _idx_dsts(ptr)))) {
                return ptr;
            }
        }
    }
    return NULL;
}

_nib_abr_entry_t *_nib_abr_iter(const _nib_abr_entry_t *last)
{
    for (const _nib_abr_entry_t *abr = (last) ? (last + 1) : _abrs;
         _in_abrs(abr); abr++) {
        if (!ipv6_addr_is_unspecified(&abr->addr)) {
            /* const modifier provided to assure internal consistency.
             * Can now be discarded. */
            return (_nib_abr_entry_t *)abr;
        }
    }
    return NULL;
}
#endif  /* CONFIG_GNRC_IPV6_NIB_MULTIHOP_P6C */

_nib_offl_entry_t *_nib_pl_add(unsigned iface,
                               const ipv6_addr_t *pfx,
                               unsigned pfx_len,
                               uint32_t valid_ltime,
                               uint32_t pref_ltime)
{
    _nib_offl_entry_t *dst = _nib_offl_add(NULL, iface, pfx, pfx_len, _PL);

    if (dst == NULL) {
        return NULL;
    }
    assert(valid_ltime >= pref_ltime);
    if ((valid_ltime != UINT32_MAX) || (pref_ltime != UINT32_MAX)) {
        uint32_t now = evtimer_now_msec();
        if (pref_ltime != UINT32_MAX) {
            _evtimer_add(dst, GNRC_IPV6_NIB_PFX_TIMEOUT, &dst->pfx_timeout,
                         pref_ltime);
            /* ignore capped of preferred lifetimes from sec to ms conversion */
            if (pref_ltime < (UINT32_MAX - 1)) {
                /* prevent pref_ltime from becoming UINT32_MAX */
                if (((pref_ltime + now) == UINT32_MAX) && (now != 0)) {
                    pref_ltime++;
                }
                pref_ltime += now;
            }
        }
        /* ignore capped of valid lifetimes from sec to ms conversion */
        if (valid_ltime < (UINT32_MAX - 1)) {
            /* prevent valid_ltime from becoming UINT32_MAX */
            if ((valid_ltime + now) == UINT32_MAX) {
                valid_ltime++;
            }
            valid_ltime += now;
        }
    }
    dst->valid_until = valid_ltime;
    dst->pref_until = pref_ltime;
    return dst;
}

static void _override_node(const ipv6_addr_t *addr, unsigned iface,
                           _nib_onl_entry_t *node)
{
    _nib_onl_clear(node);
    if (addr != NULL) {
        memcpy(&node->ipv6, addr, sizeof(node->ipv6));
    }
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

uint32_t _evtimer_lookup(const void *ctx, uint16_t type)
{
    evtimer_msg_event_t *event = (evtimer_msg_event_t *)_nib_evtimer.events;
    uint32_t offset = 0;

    DEBUG("nib: lookup ctx = %p, type = %04x\n", (void *)ctx, type);
    while (event != NULL) {
        offset += event->event.offset;
        if ((event->msg.type == type) &&
            ((ctx == NULL) || (event->msg.content.ptr == ctx))) {
            return offset;
        }
        event = (evtimer_msg_event_t *)event->event.next;
    }
    return UINT32_MAX;
}

/** @} */
