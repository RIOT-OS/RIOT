/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  net_gnrc_ipv6_nib
 * @internal
 * @{
 *
 * @file
 * @brief       Internal definitions
 *
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef PRIV_NIB_INTERNAL_H
#define PRIV_NIB_INTERNAL_H

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "bitfield.h"
#include "evtimer_msg.h"
#include "sched.h"
#include "mutex.h"
#include "net/eui64.h"
#include "kernel_defines.h"
#include "net/ipv6/addr.h"
#ifdef MODULE_GNRC_IPV6
#include "net/gnrc/ipv6.h"
#endif
#include "net/gnrc/ipv6/nib/ft.h"
#include "net/gnrc/ipv6/nib/nc.h"
#include "net/gnrc/ipv6/nib/conf.h"
#include "net/gnrc/pktqueue.h"
#include "net/gnrc/sixlowpan/ctx.h"
#include "net/ndp.h"
#include "random.h"
#include "timex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Mode flags for entries
 * @anchor  net_gnrc_ipv6_nib_mode
 * @{
 */
#define _EMPTY  (0x00)      /**< empty */
#define _NC     (0x01)      /**< neighbor cache */
#define _DC     (0x02)      /**< destination cache */
#define _PL     (0x04)      /**< prefix list */
#define _DRL    (0x08)      /**< default router list */
#define _FT     (0x10)      /**< forwarding table */
#define _DAD    (0x20)      /**< 6LoWPAN duplicate address detection table */
#define _DST    (0x40)      /**< there is @ref _nib_offl_entry_t pointing
                                 to this @ref _nib_onl_entry_t */
/** @} */

/**
 * @name    Off-link entry flags
 * @anchor  net_gnrc_ipv6_nib_offl_flags
 * @{
 */
#define _PFX_ON_LINK    (0x0001)
#define _PFX_SLAAC      (0x0002)
/** @} */

/**
 * @brief   Shorter name for convenience ;-)
 */
#define _NIB_IF_MASK        (GNRC_IPV6_NIB_NC_INFO_IFACE_MASK)

/**
 * @brief   Shorter name for convenience ;-)
 */
#define _NIB_IF_POS         (GNRC_IPV6_NIB_NC_INFO_IFACE_POS)

/**
 * @brief   Maximum identifier for the interface
 */
#define _NIB_IF_MAX         (_NIB_IF_MASK >> _NIB_IF_POS)

/**
 * @brief   On-link NIB entry
 * @anchor  _nib_onl_entry_t
 */
typedef struct _nib_onl_entry {
    struct _nib_onl_entry *next;        /**< next removable entry */
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_QUEUE_PKT) || defined(DOXYGEN)
    /**
     * @brief   queue for packets currently in address resolution
     *
     * @note    Only available if @ref CONFIG_GNRC_IPV6_NIB_QUEUE_PKT != 0.
     */
    gnrc_pktqueue_t *pktqueue;
#endif
    /**
     * @brief Neighbors IPv6 address
     */
    ipv6_addr_t ipv6;
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_6LR) || defined(DOXYGEN)
    /**
     * @brief   The neighbors EUI-64 (used for DAD)
     *
     * @note    Only available if @ref CONFIG_GNRC_IPV6_NIB_6LR != 0.
     */
    eui64_t eui64;
#endif
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_ARSM) || defined(DOXYGEN)
    /**
     * @brief   Link-layer address of _nib_onl_entry_t::next_hop
     *
     * @note    Only available if @ref CONFIG_GNRC_IPV6_NIB_ARSM != 0.
     */
    uint8_t l2addr[CONFIG_GNRC_IPV6_NIB_L2ADDR_MAX_LEN];
#endif
    /**
     * @brief Event for @ref GNRC_IPV6_NIB_SND_UC_NS,
     *        @ref GNRC_IPV6_NIB_SND_MC_NS, @ref GNRC_IPV6_NIB_REACH_TIMEOUT and
     *        @ref GNRC_IPV6_NIB_DELAY_TIMEOUT
     *
     * @note    Four event types
     *          1. To easier distinguish multicast probes in _evtimer_lookup for
     *             rate-limiting from unicast probes.
     *          2. Since the types can't be in the event queue at the same time
     *             (since they only have one NUD state at a time and probing is
     *             one of these states). Because of this we can use one event
     *             for all of them (but need the different types, since the
     *             events are handled differently).
     * @note    This is also available with @ref CONFIG_GNRC_IPV6_NIB_ARSM == 0,
     *          since 6Lo address registration uses it to time the sending of
     *          neighbor solicitations.
     */
    evtimer_msg_event_t nud_timeout;
    /**
     * @brief Event for @ref GNRC_IPV6_NIB_SND_NA
     */
    evtimer_msg_event_t snd_na;
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_ROUTER) || defined(DOXYGEN)
    evtimer_msg_event_t reply_rs;           /**< Event for @ref GNRC_IPV6_NIB_REPLY_RS */
#endif
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_6LR) || defined(DOXYGEN)
    evtimer_msg_event_t addr_reg_timeout;   /**< Event for @ref GNRC_IPV6_NIB_ADDR_REG_TIMEOUT */
#endif

    /**
     * @brief   Information flags
     *
     * @see [NC info values](@ref net_gnrc_ipv6_nib_nc_info)
     */
    uint16_t info;

    /**
     * @brief   NIB entry mode
     *
     * This field identifies which "views" of the NIB the entry belongs to.
     *
     * @see [Mode flags for entries](@ref net_gnrc_ipv6_nib_mode).
     */
    uint8_t mode;
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_ARSM) || defined(DOXYGEN)
    /**
     * @brief   Neighbor solicitations sent for probing
     *
     * @note    Only available if @ref CONFIG_GNRC_IPV6_NIB_ARSM != 0.
     */
    uint8_t ns_sent;

    /**
     * @brief   length in bytes of _nib_onl_entry_t::l2addr
     *
     * @note    Only available if @ref CONFIG_GNRC_IPV6_NIB_ARSM != 0.
     */
    uint8_t l2addr_len;
#endif
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_QUEUE_PKT) || defined(DOXYGEN)
    uint8_t pktqueue_len; /**< Number of queued packets (in pktqueue) */
#endif
} _nib_onl_entry_t;

/**
 * @brief   Default route NIB entry
 */
typedef struct {
    _nib_onl_entry_t *next_hop; /**< next hop to destination */
    /**
     * @brief   Event for @ref GNRC_IPV6_NIB_RTR_TIMEOUT
     */
    evtimer_msg_event_t rtr_timeout;
} _nib_dr_entry_t;

/**
 * @brief   Off-link NIB entry
 */
typedef struct {
    _nib_onl_entry_t *next_hop; /**< next hop to destination */
    ipv6_addr_t pfx;            /**< prefix to the destination */
    /**
     * @brief   Event for @ref GNRC_IPV6_NIB_PFX_TIMEOUT
     */
    evtimer_msg_event_t pfx_timeout;
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_ROUTER)
    /**
     * @brief   Event for @ref GNRC_IPV6_NIB_ROUTE_TIMEOUT
     */
    evtimer_msg_event_t route_timeout;
#endif
    uint8_t mode;               /**< [mode](@ref net_gnrc_ipv6_nib_mode) of the
                                 *   off-link entry */
    uint8_t pfx_len;            /**< prefix-length in bits of
                                 *   _nib_onl_entry_t::pfx */
    uint16_t flags;             /**< [flags](@ref net_gnrc_ipv6_nib_offl_flags */
    uint32_t valid_until;       /**< timestamp (in ms) until which the prefix
                                     valid (UINT32_MAX means forever) */
    uint32_t pref_until;        /**< timestamp (in ms) until which the prefix
                                     preferred (UINT32_MAX means forever) */
} _nib_offl_entry_t;

/**
 * @brief   Internal NIB-representation of the authoritative border router
 *          for multihop prefix and 6LoWPAN context dissemination
 */
typedef struct {
    ipv6_addr_t addr;               /**< The address of the border router */
    uint32_t version;               /**< last received version of the info of
                                     *   the _nib_abr_entry_t::addr */
    uint32_t valid_until_ms;        /**< timestamp (in ms) until which information is valid
                                     *   (needs resolution in minutes an 16 bits of them)*/
    evtimer_msg_event_t timeout;    /**< timeout of the information */
    /**
     * @brief   Bitfield marking the prefixes in the NIB's off-link entries
     *          disseminated by _nib_abr_entry_t::addr
     */
    BITFIELD(pfxs, CONFIG_GNRC_IPV6_NIB_OFFL_NUMOF);
    /**
     * @brief   Bitfield marking the contexts disseminated by
     *          _nib_abr_entry_t::addr
     */
    BITFIELD(ctxs, GNRC_SIXLOWPAN_CTX_SIZE);
} _nib_abr_entry_t;

/**
 * @brief   Event timer for the NIB.
 */
extern evtimer_msg_t _nib_evtimer;

/**
 * @brief   Primary default router.
 *
 * This value is returned by @ref @_nib_drl_get_dr() when it is not NULL and it
 * is reachable. Otherwise it is selected with the [default router selection
 * algorithm](https://tools.ietf.org/html/rfc4861#section-6.3.6) by that function.
 * Exposed to be settable by @ref net_gnrc_ipv6_nib_ft.
 */
extern _nib_dr_entry_t *_prime_def_router;

/**
 * @brief   Looks up if an event is queued in the event timer
 *
 * @param[in] ctx   Context of the event. May be NULL for any event context.
 * @param[in] type  [Type of the event](@ref net_gnrc_ipv6_nib_msg).
 *
 * @return  Milliseconds to the event, if event in queue.
 * @return  UINT32_MAX, event is not in queue.
 */
uint32_t _evtimer_lookup(const void *ctx, uint16_t type);

/**
 * @brief   Removes an event from the event timer
 *
 * @param[in] event Representation of the event.
 */
static inline void _evtimer_del(evtimer_msg_event_t *event)
{
    evtimer_del(&_nib_evtimer, &event->event);
}

/**
 * @brief   Adds an event to the event timer
 *
 * @param[in] ctx       The context of the event
 * @param[in] type      [Type of the event](@ref net_gnrc_ipv6_nib_msg).
 * @param[in,out] event Representation of the event.
 * @param[in] offset    Offset in milliseconds to the event.
 */
static inline void _evtimer_add(void *ctx, int16_t type,
                                evtimer_msg_event_t *event, uint32_t offset)
{
#ifdef MODULE_GNRC_IPV6
    kernel_pid_t target_pid = gnrc_ipv6_pid;
#else
    kernel_pid_t target_pid = KERNEL_PID_LAST;  /* just for testing */
#endif
    _evtimer_del(event);
    event->event.next = NULL;
    event->event.offset = offset;
    event->msg.type = type;
    event->msg.content.ptr = ctx;
    evtimer_add_msg(&_nib_evtimer, event, target_pid);
}

/**
 * @brief   Initializes NIB internally
 */
void _nib_init(void);

/**
 * @brief   Acquire exclusive access to the NIB
 */
void _nib_acquire(void);

/**
 * @brief   Release exclusive access to the NIB
 */
void _nib_release(void);

/**
 * @brief   Gets interface identifier from a NIB entry
 *
 * @param[in] node  A NIB entry.
 *
 * @return  The NIB entry's interface identifier.
 */
static inline unsigned _nib_onl_get_if(const _nib_onl_entry_t *node)
{
    return (node->info & _NIB_IF_MASK) >> _NIB_IF_POS;
}

/**
 * @brief   Sets interface for a NIB entry
 *
 * @param[in,out] node  A NIB entry.
 * @param[in] iface     An interface identifier.
 */
static inline void _nib_onl_set_if(_nib_onl_entry_t *node, unsigned iface)
{
    assert(iface <= _NIB_IF_MAX);
    node->info &= ~(_NIB_IF_MASK);
    node->info |= ((iface << _NIB_IF_POS) & _NIB_IF_MASK);
}

/**
 * @brief   Creates or gets an existing on-link entry by address
 *
 * @param[in] addr  An IPv6 address. May be NULL (to be pointed to by a prefix
 *                  list entry). *May also be a global address!*
 * @param[in] iface The interface to the node.
 *
 * @return  A new or existing on-link entry with _nib_onl_entry_t::ipv6 set to
 *          @p addr.
 * @return  NULL, if no space is left.
 */
_nib_onl_entry_t *_nib_onl_alloc(const ipv6_addr_t *addr, unsigned iface);

/**
 * @brief   Clears out a NIB entry (on-link version)
 *
 * @param[in,out] node  An entry.
 *
 * @return  true, if entry was cleared.
 * @return  false, if entry was not cleared.
 */
static inline bool _nib_onl_clear(_nib_onl_entry_t *node)
{
    if (node->mode == _EMPTY) {
        memset(node, 0, sizeof(_nib_onl_entry_t));
        return true;
    }
    return false;
}

/**
 * @brief   Iterates over on-link entries
 *
 * @param[in] last  Last entry (NULL to start).
 *
 * @return  entry after @p last.
 */
_nib_onl_entry_t *_nib_onl_iter(const _nib_onl_entry_t *last);

/**
 * @brief   Gets a node by IPv6 address and interface
 *
 * @pre     `(addr != NULL)`
 *
 * @param[in] addr  The address of a node. Must not be NULL.
 * @param[in] iface The interface to the node. May be 0 for any interface.
 *
 * @return  The NIB entry for node with @p addr and @p iface on success.
 * @return  NULL, if there is no such entry.
 */
_nib_onl_entry_t *_nib_onl_get(const ipv6_addr_t *addr, unsigned iface);

/**
 * @brief   Gets a node by IPv6 address and interface from the neighbor cache
 *
 * @pre     `(addr != NULL)`
 *
 * @param[in] addr  The address of a node. Must not be NULL.
 * @param[in] iface The interface to the node. May be 0 for any interface.
 *
 * @return  The Neighbor Cache entry for node with @p addr and @p iface on success.
 * @return  NULL, if there is no such entry.
 */
static inline _nib_onl_entry_t *_nib_onl_nc_get(const ipv6_addr_t *addr, unsigned iface)
{
    _nib_onl_entry_t *nce = _nib_onl_get(addr, iface);
    if (nce && (nce->mode & _NC)) {
        return nce;
    }
    return NULL;
}

/**
 * @brief   Creates or gets an existing node from the neighbor cache by address
 *
 * @pre     `((cstate & GNRC_IPV6_NIB_NC_INFO_NUD_STATE_MASK) !=
 *             GNRC_IPV6_NIB_NC_INFO_NUD_STATE_DELAY)`
 * @pre     `((cstate & GNRC_IPV6_NIB_NC_INFO_NUD_STATE_MASK) !=
 *             GNRC_IPV6_NIB_NC_INFO_NUD_STATE_PROBE)`
 * @pre     `((cstate & GNRC_IPV6_NIB_NC_INFO_NUD_STATE_MASK) !=
 *             GNRC_IPV6_NIB_NC_INFO_NUD_STATE_REACHABLE)`
 *
 * @param[in] addr      The address of a node. May be NULL for prefix list
 *                      entries.
 * @param[in] iface     The interface to the node.
 * @param[in] cstate    Creation state. State of the entry *if* the entry is
 *                      newly created.
 *
 * @return  The NIB entry for the new neighbor cache entry on success.
 * @return  NULL, if there is no space left.
 */
_nib_onl_entry_t *_nib_nc_add(const ipv6_addr_t *addr, unsigned iface,
                              uint16_t cstate);

/**
 * @brief   Removes a node from the neighbor cache
 *
 * @param[in,out] node  A node.
 */
void _nib_nc_remove(_nib_onl_entry_t *node);

/**
 * @brief   Gets external neighbor cache entry representation from on-link entry
 *
 * @pre `(node != NULL) && (nce != NULL)`
 *
 * @param[in] node  On-link entry.
 * @param[out] nce  External representation of the neighbor cache entry.
 */
void _nib_nc_get(const _nib_onl_entry_t *node, gnrc_ipv6_nib_nc_t *nce);

/**
 * @brief   Sets a NUD-managed neighbor cache entry to reachable and sets the
 *          respective event in @ref _nib_evtimer "event timer"
 *          (@ref GNRC_IPV6_NIB_MSG_NUD_SET_STALE)
 *
 * @param[in,out] node  A node.
 */
void _nib_nc_set_reachable(_nib_onl_entry_t *node);

/**
 * @brief   Creates or gets an existing node from the DAD table by address
 *
 * @pre `addr != NULL`
 *
 * @param[in] addr      The address of a node. Must not be NULL.
 *
 * @return  The NIB entry for the new DAD table entry on success.
 * @return  NULL, if there is no space left.
 */
static inline _nib_onl_entry_t *_nib_dad_add(const ipv6_addr_t *addr)
{
    assert(addr != NULL);
    _nib_onl_entry_t *node = _nib_onl_alloc(addr, 0);

    if (node != NULL) {
        node->mode |= (_DAD);
    }
    return node;
}

/**
 * @brief   Removes a node from the DAD table
 *
 * @param[in] node  A node.
 */
static inline void _nib_dad_remove(_nib_onl_entry_t *node)
{
    node->mode &= ~(_DAD);
    _nib_onl_clear(node);
}

/**
 * @brief   Creates or gets an existing default router list entry by address
 *
 * @pre     `(addr != NULL)`
 *
 * @param[in] addr  An IPv6 address. Must not be NULL.
 *                  *May also be a global address!*
 * @param[in] iface The interface to the router.
 *
 * @return  A new or existing default router entry with _nib_onl_entry_t::ipv6
 *          of _nib_dr_entry_t::next_hop set to @p router_addr.
 * @return  NULL, if no space is left.
 */
_nib_dr_entry_t *_nib_drl_add(const ipv6_addr_t *addr, unsigned iface);

/**
 * @brief   Removes a default router list entry
 *
 * @param[in,out] nib_dr    An entry.
 *
 * Corresponding on-link entry is removed, too.
 */
void _nib_drl_remove(_nib_dr_entry_t *nib_dr);

/**
 * @brief   Iterates over default router list
 *
 * @param[in] last  Last entry (NULL to start).
 *
 * @return  entry after @p last.
 */
_nib_dr_entry_t *_nib_drl_iter(const _nib_dr_entry_t *last);

/**
 * @brief   Gets a default router by IPv6 address and interface
 *
 * @pre     `(router_addr != NULL) || (iface != 0)`
 *
 * @param[in] router_addr   The address of a default router. May be NULL for
 *                          any address, but then @p iface must not be NULL.
 * @param[in] iface         The interface to the node. May be 0 for any
 *                          interface, but then @p router_addr must not be NULL.
 *
 * @return  The NIB entry for node with @p router_addr and @p iface onsuccess.
 * @return  NULL, if there is no such entry.
 */
_nib_dr_entry_t *_nib_drl_get(const ipv6_addr_t *router_addr, unsigned iface);

/**
 * @brief   Gets external forwarding table entry representation from default
 *          router entry
 *
 * @pre `(drl != NULL) && (drl->next_hop != NULL) && (fte != NULL)`
 *
 * @param[in] drl   Default router entry.
 * @param[out] fte  External representation of the forwarding table entry.
 */
void _nib_drl_ft_get(const _nib_dr_entry_t *drl, gnrc_ipv6_nib_ft_t *fte);

/**
 * @brief   Gets *the* default router
 *
 * @see [RFC 4861, section 6.3.6](https://tools.ietf.org/html/rfc4861#section-6.3.6)
 *
 * @return  The current default router, on success.
 * @return  NULL, if there is no default router in the list.
 */
_nib_dr_entry_t *_nib_drl_get_dr(void);

/**
 * @brief   Creates or gets an existing off-link entry by next hop and prefix
 *
 * @pre `(pfx != NULL) && (pfx != "::") && (pfx_len != 0) && (pfx_len <= 128)`
 *
 * @param[in] next_hop  An IPv6 address to next hop. May be NULL (for prefix
 *                      list). *May also be a global address!*
 * @param[in] iface     The interface to @p next_hop.
 * @param[in] pfx       The IPv6 prefix or address of the destination.
 *                      Must not be NULL or unspecified address. Use
 *                      @ref _nib_drl_add() for default route destinations.
 * @param[in] pfx_len   The length in bits of @p pfx in bits.
 *
 * @return  A new or existing off-link entry with _nib_dr_entry_t::pfx set to
 *          @p pfx.
 * @return  NULL, if no space is left.
 */
_nib_offl_entry_t *_nib_offl_alloc(const ipv6_addr_t *next_hop, unsigned iface,
                                   const ipv6_addr_t *pfx, unsigned pfx_len);

/**
 * @brief   Clears out a NIB entry (off-link version)
 *
 * @param[in,out] dst  An entry.
 */
void _nib_offl_clear(_nib_offl_entry_t *dst);

/**
 * @brief   Iterates over off-link entries
 *
 * @param[in] last  Last entry (NULL to start).
 *
 * @return  entry after @p last.
 */
_nib_offl_entry_t *_nib_offl_iter(const _nib_offl_entry_t *last);

/**
 * @brief   Checks if @p entry was allocated using _nib_offl_alloc()
 *
 * @param[in] entry An entry.
 *
 * @return  true, if @p entry was allocated using @ref _nib_offl_alloc()
 * @return  false, if @p entry was not allocated using @ref _nib_offl_alloc()
 */
bool _nib_offl_is_entry(const _nib_offl_entry_t *entry);

/**
 * @brief   Helper function for view-level add-functions below
 *
 * @pre     `(pfx != NULL) && (pfx != "::") && (pfx_len != 0) && (pfx_len <= 128)`
 *
 * @param[in] next_hop  An IPv6 address to next hop. May be NULL (for prefix
 *                      list). *May also be a global address!*
 * @param[in] iface     The interface to the destination.
 * @param[in] pfx       The IPv6 prefix or address of the destination.
 *                      Must not be NULL or unspecified address. Use
 *                      @ref _nib_drl_add() for default route destinations.
 * @param[in] pfx_len   The length in bits of @p pfx in bits.
 * @param[in] mode      [NIB-mode](_nib_onl_entry_t::mode).
 *
 * @return  A new or existing off-link entry with _nib_offl_entry_t::pfx set to
 *          @p pfx.
 * @return  NULL, if no space is left.
 */
static inline _nib_offl_entry_t *_nib_offl_add(const ipv6_addr_t *next_hop,
                                               unsigned iface,
                                               const ipv6_addr_t *pfx,
                                               unsigned pfx_len, uint8_t mode)
{
    _nib_offl_entry_t *nib_offl = _nib_offl_alloc(next_hop, iface, pfx, pfx_len);

    if (nib_offl != NULL) {
        nib_offl->mode |= mode;
    }
    return nib_offl;
}

/**
 * @brief   Helper function for the view-level remove-functions below
 *
 * @param[in,out] nib_offl  An entry.
 */
static inline void _nib_offl_remove(_nib_offl_entry_t *nib_offl, uint8_t mode)
{
    nib_offl->mode &= ~mode;
    _nib_offl_clear(nib_offl);
}

#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_DC) || DOXYGEN
/**
 * @brief   Creates or gets an existing destination cache entry by its addresses
 *
 * @pre     `(next_hop != NULL)`
 * @pre     `(dst != NULL)`
 *
 * @param[in] next_hop  Next hop to the destination. Must not be NULL.
 *                      *May also be a global address!*
 * @param[in] iface     The interface to the destination.
 *
 * @note    Only available if @ref CONFIG_GNRC_IPV6_NIB_DC.
 *
 * @return  A new or existing off-link entry with _nib_offl_entry_t::pfx set to
 *          @p pfx.
 * @return  NULL, if no space is left.
 */
static inline _nib_offl_entry_t *_nib_dc_add(const ipv6_addr_t *next_hop,
                                             unsigned iface,
                                             const ipv6_addr_t *dst)
{
    assert((next_hop != NULL) && (dst != NULL));
    return _nib_offl_add(next_hop, iface, dst, IPV6_ADDR_BIT_LEN, _DC);
}

/**
 * @brief   Removes a destination cache entry
 *
 * @param[in,out] nib_offl  Destination entry to remove.
 *
 * Corresponding on-link entry is removed, too.
 *
 * @note    Only available if @ref CONFIG_GNRC_IPV6_NIB_DC.
 */
static inline void _nib_dc_remove(_nib_offl_entry_t *nib_offl)
{
    _nib_offl_remove(nib_offl, _DC);
}
#endif /* CONFIG_GNRC_IPV6_NIB_DC */

/**
 * @brief   Creates or gets an existing prefix list entry by its prefix
 *
 * @pre     `(next_hop != NULL)`
 * @pre     `(pfx != NULL) && (pfx != "::") && (pfx_len != 0) && (pfx_len <= 128)`
 * @pre     `(pref_ltime <= valid_ltime)`
 *
 * @param[in] iface         The interface to the prefix is added to.
 * @param[in] pfx           The IPv6 prefix or address of the destination.
 *                          Must not be NULL or unspecified address. Use
 *                          @ref _nib_drl_add() for default route destinations.
 * @param[in] pfx_len       The length in bits of @p pfx in bits.
 * @param[in] valid_ltime   Valid lifetime in microseconds. `UINT32_MAX` for
 *                          infinite.
 * @param[in] pref_ltime    Preferred lifetime in microseconds. `UINT32_MAX` for
 *                          infinite.
 *
 * @return  A new or existing off-link entry with _nib_offl_entry_t::pfx set to
 *          @p pfx.
 * @return  NULL, if no space is left.
 */
_nib_offl_entry_t *_nib_pl_add(unsigned iface,
                               const ipv6_addr_t *pfx,
                               unsigned pfx_len,
                               uint32_t valid_ltime,
                               uint32_t pref_ltime);

/**
 * @brief   Removes a prefix list entry
 *
 * @param[in,out] nib_offl    An entry.
 *
 * Corresponding on-link entry is removed, too.
 */
void _nib_pl_remove(_nib_offl_entry_t *nib_offl);

/**
 * @brief   Removes a prefix from the prefix list as well as the addresses
 *          associated with the prefix.
 *
 * @param[in,out] pfx       The prefix to remove.
 *
 * Corresponding on-link entry is removed, too.
 */
void _nib_offl_remove_prefix(_nib_offl_entry_t *pfx);

#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_ROUTER) || DOXYGEN
/**
 * @brief   Creates or gets an existing forwarding table entry by its prefix
 *
 * @pre     `(next_hop != NULL)`
 * @pre     `(pfx != NULL) && (pfx != "::") && (pfx_len != 0) && (pfx_len <= 128)`
 *
 * @param[in] next_hop  Next hop to the destination. Must not be NULL.
 *                      *May also be a global address!*
 * @param[in] iface     The interface to the destination.
 * @param[in] pfx       The IPv6 prefix or address of the destination.
 *                      Must not be NULL or unspecified address. Use
 *                      @ref _nib_drl_add() for default route destinations.
 * @param[in] pfx_len   The length in bits of @p pfx in bits.
 *
 * @note    Only available if @ref CONFIG_GNRC_IPV6_NIB_ROUTER.
 *
 * @return  A new or existing off-link entry with _nib_offl_entry_t::pfx set to
 *          @p pfx.
 * @return  NULL, if no space is left.
 */
static inline _nib_offl_entry_t *_nib_ft_add(const ipv6_addr_t *next_hop,
                                             unsigned iface,
                                             const ipv6_addr_t *pfx,
                                             unsigned pfx_len)
{
    return _nib_offl_add(next_hop, iface, pfx, pfx_len, _FT);
}

/**
 * @brief   Removes a forwarding table entry
 *
 * @param[in,out] nib_offl    An entry.
 *
 * Corresponding on-link entry is removed, too.
 *
 * @note    Only available if @ref CONFIG_GNRC_IPV6_NIB_ROUTER.
 */
static inline void _nib_ft_remove(_nib_offl_entry_t *nib_offl)
{
    _evtimer_del(&nib_offl->route_timeout);
    _nib_offl_remove(nib_offl, _FT);
}
#endif  /* CONFIG_GNRC_IPV6_NIB_ROUTER */

#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_MULTIHOP_P6C) || defined(DOXYGEN)
/**
 * @brief   Creates or gets an existing authoritative border router.
 *
 * @pre `addr != NULL`
 *
 * @param[in] addr  Address of the authoritative border router.
 *
 * @return  An authoritative border router entry, on success.
 * @return  NULL, if no space is left.
 */
_nib_abr_entry_t *_nib_abr_add(const ipv6_addr_t *addr);

/**
 * @brief   Removes an authoritative border router
 *
 * @pre `addr != NULL`
 *
 * @param[in] addr  Address of the authoritative border router.
 */
void _nib_abr_remove(const ipv6_addr_t *addr);

/**
 * @brief   Adds a prefix to the managed prefix of the authoritative border
 *          router
 *
 * @pre `(abr != NULL) && (offl != NULL) && (offl->mode & _PL)`
 *
 * @param[in] abr   The border router.
 * @param[in] offl  The prefix to add.
 */
void _nib_abr_add_pfx(_nib_abr_entry_t *abr, const _nib_offl_entry_t *offl);

/**
 * @brief   Iterates over an authoritative border router's prefixes
 *
 * @pre `(abr != NULL)`
 *
 * @param[in] abr   The border router
 * @param[in] last  Last prefix (NULL to start)
 *
 * @return  entry after @p last.
 * @return  NULL, if @p last is the last prefix of @p abr or if @p last
 *          wasn't in NIB (and != NULL).
 */
_nib_offl_entry_t *_nib_abr_iter_pfx(const _nib_abr_entry_t *abr,
                                     const _nib_offl_entry_t *last);

/**
 * @brief   Iterates over authoritative border router entries
 *
 * @param[in] last  Last entry (NULL to start).
 *
 * @return  entry after @p last.
 * @return  NULL, if @p last is the last ABR in the NIB.
 */
_nib_abr_entry_t *_nib_abr_iter(const _nib_abr_entry_t *last);
#else
#define _nib_abr_iter(abr) NULL
#endif

/**
 * @brief   Gets external forwarding table entry representation from off-link
 *          entry
 *
 * @pre `(dst != NULL) && (dst->next_hop != NULL) && (fte != NULL)`
 *
 * @param[in] dst   Off-link entry.
 * @param[out] fte  External representation of the forwarding table entry.
 */
void _nib_ft_get(const _nib_offl_entry_t *dst, gnrc_ipv6_nib_ft_t *fte);

/**
 * @brief   Gets best match to @p dst from all off-link entries and default
 *          route.
 *
 * @pre `(dst != NULL) && (fte != NULL)`
 *
 * @param[in] dst   Destination address to get the off-link entry for.
 * @param[in] pkt   Packet causing the route look-up (provided to allow reactive
 *                  routing protocols to queue it if needed). May be NULL.
 * @param[out] fte  Resulting forwarding table entry.
 *
 * @return  0, on success.
 * @return  -ENETUNREACH, when no route was found.
 */
int _nib_get_route(const ipv6_addr_t *dst, gnrc_pktsnip_t *pkt,
                   gnrc_ipv6_nib_ft_t *fte);

#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_QUEUE_PKT) || DOXYGEN
/**
 * @brief Flush the packet queue of a on-link neighbor.
 *
 * @param node neighbor entry to be flushed
 */
void _nbr_flush_pktqueue(_nib_onl_entry_t *node);

/**
 * @brief Remove oldest packet from a on-link neighbor's packet queue.
 *
 * @param node neighbor entry
 *
 * @retval pointer to the packet entry or NULL if the queue is empty
 */
gnrc_pktqueue_t *_nbr_pop_pkt(_nib_onl_entry_t *node);

/**
 * @brief Push packet to a on-link neighbor's packet queue.
 *
 * If there are already @ref CONFIG_GNRC_IPV6_NIB_NBR_QUEUE_CAP packets queued,
 * the oldest will be dropped silently.
 *
 * @pre Neighbor is INCOMPLETE.
 *
 * @param node neighbor entry
 * @param pkt packet to be pushed
 */
void _nbr_push_pkt(_nib_onl_entry_t *node, gnrc_pktqueue_t *pkt);
#else
#define _nbr_flush_pktqueue(node) ((void)node)
#define _nbr_pop_pkt(node) ((void)node, NULL)
#define _nbr_push_pkt(node, pkt) ((void)node, (void)pkt)
#endif

#ifdef __cplusplus
}
#endif

#endif /* PRIV_NIB_INTERNAL_H */
/** @internal
 * @} */
