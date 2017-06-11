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

#include <stdbool.h>
#include <stdint.h>

#include "kernel_types.h"
#include "mutex.h"
#include "net/eui64.h"
#include "net/ipv6/addr.h"
#include "net/gnrc/ipv6/nib/nc.h"
#include "net/gnrc/ipv6/nib/conf.h"
#include "net/gnrc/pktqueue.h"
#include "net/ndp.h"
#include "random.h"

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
#if GNRC_IPV6_NIB_CONF_QUEUE_PKT || defined(DOXYGEN)
    /**
     * @brief   queue for packets currently in address resolution
     *
     * @note    Only available if @ref GNRC_IPV6_NIB_CONF_QUEUE_PKT != 0.
     */
    gnrc_pktqueue_t *pktqueue;
#endif
    /**
     * @brief Neighbors IPv6 address
     */
    ipv6_addr_t ipv6;
#if GNRC_IPV6_NIB_CONF_6LR || defined(DOXYGEN)
    /**
     * @brief   The neighbors EUI-64 (used for DAD)
     *
     * @note    Only available if @ref GNRC_IPV6_NIB_CONF_6LR != 0.
     */
    eui64_t eui64;
#endif
#if GNRC_IPV6_NIB_CONF_ARSM || defined(DOXYGEN)
    /**
     * @brief   Link-layer address of _nib_onl_entry_t::next_hop
     *
     * @note    Only available if @ref GNRC_IPV6_NIB_CONF_ARSM != 0.
     */
    uint8_t l2addr[GNRC_IPV6_NIB_L2ADDR_MAX_LEN];
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
#if GNRC_IPV6_NIB_CONF_ARSM || defined(DOXYGEN)
    /**
     * @brief   Neighbor solicitations sent for probing
     *
     * @note    Only available if @ref GNRC_IPV6_NIB_CONF_ARSM != 0.
     */
    uint8_t ns_sent;

    /**
     * @brief   length in bytes of _nib_onl_entry_t::l2addr
     *
     * @note    Only available if @ref GNRC_IPV6_NIB_CONF_ARSM != 0.
     */
    uint8_t l2addr_len;
#endif
} _nib_onl_entry_t;

/**
 * @brief   Default route NIB entry
 */
typedef struct {
    _nib_onl_entry_t *next_hop; /**< next hop to destination */
    uint16_t ltime;             /**< lifetime in seconds */
} _nib_dr_entry_t;

/**
 * @brief   Off-link NIB entry
 */
typedef struct {
    _nib_onl_entry_t *next_hop; /**< next hop to destination */
    ipv6_addr_t pfx;            /**< prefix to the destination */
    unsigned pfx_len;           /**< prefix-length in bits of
                                 *   _nib_onl_entry_t::pfx */
} _nib_offl_entry_t;

/**
 * @brief   Interface specific information for Neighbor Discovery
 */
typedef struct {
#if GNRC_IPV6_NIB_CONF_ARSM
    /**
     * @brief   base for random reachable time calculation
     */
    uint32_t reach_time_base;
    uint32_t reach_time;                /**< reachable time (in ms) */
    uint32_t retrans_time;              /**< retransmission time (in ms) */
#endif
#if GNRC_IPV6_NIB_CONF_ROUTER || defined(DOXYGEN)
    /**
     * @brief   timestamp in milliseconds of last unsolicited router
     *          advertisement
     *
     * @note    Only available if @ref GNRC_IPV6_NIB_CONF_ROUTER.
     */
    uint32_t last_ra;
#endif
    kernel_pid_t pid;                   /**< identifier of the interface */
#if GNRC_IPV6_NIB_CONF_ROUTER || defined(DOXYGEN)
    /**
     * @brief   number of unsolicited router advertisements sent
     *
     * This only counts up to the first @ref NDP_MAX_INIT_RA_NUMOF on interface
     * initialization. The last @ref NDP_MAX_FIN_RA_NUMOF of an advertising
     * interface are counted from UINT8_MAX - @ref NDP_MAX_FIN_RA_NUMOF + 1.
     *
     * @note    Only available if @ref GNRC_IPV6_NIB_CONF_ROUTER.
     */
    uint8_t ra_sent;
#endif
    /**
     * @brief   number of unsolicited router solicitations scheduled
     */
    uint8_t rs_sent;
    /**
     * @brief   number of unsolicited neighbor advertisements scheduled
     */
    uint8_t na_sent;
} _nib_iface_t;

/**
 * @brief   Mutex for locking the NIB
 */
extern mutex_t _nib_mutex;

/**
 * @brief   Initializes NIB internally
 */
void _nib_init(void);

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
 * @pre     `(addr != NULL)`.
 *
 * @param[in] addr  An IPv6 address. May not be NULL.
 *                  *May also be a global address!*
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
 * @param[in] addr  The address of a node. May not be NULL.
 * @param[in] iface The interface to the node. May be 0 for any interface.
 *
 * @return  The NIB entry for node with @p addr and @p iface on success.
 * @return  NULL, if there is no such entry.
 */
_nib_onl_entry_t *_nib_onl_get(const ipv6_addr_t *addr, unsigned iface);

/**
 * @brief   Creates or gets an existing node from the neighbor cache by address
 *
 * @pre     `(addr != NULL)`
 * @pre     `((cstate & GNRC_IPV6_NIB_NC_INFO_NUD_STATE_MASK) !=
 *             GNRC_IPV6_NIB_NC_INFO_NUD_STATE_DELAY)`
 * @pre     `((cstate & GNRC_IPV6_NIB_NC_INFO_NUD_STATE_MASK) !=
 *             GNRC_IPV6_NIB_NC_INFO_NUD_STATE_PROBE)`
 * @pre     `((cstate & GNRC_IPV6_NIB_NC_INFO_NUD_STATE_MASK) !=
 *             GNRC_IPV6_NIB_NC_INFO_NUD_STATE_REACHABLE)`
 *
 * @param[in] addr      The address of a node. May not be NULL.
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
 * @param[in] addr      The address of a node. May not be NULL.
 *
 * @return  The NIB entry for the new DAD table entry on success.
 * @return  NULL, if there is no space left.
 */
static inline _nib_onl_entry_t *_nib_dad_add(const ipv6_addr_t *addr)
{
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
 * @pre     `(router_addr != NULL)`
 *
 * @param[in] addr  An IPv6 address. May not be NULL.
 *                  *May also be a global address!*
 * @param[in] iface The interface to the router.
 *
 * @return  A new or existing default router entry with _nib_onl_entry_t::ipv6
 *          of _nib_dr_entry_t::next_hop set to @p router_addr.
 * @return  NULL, if no space is left.
 */
_nib_dr_entry_t *_nib_drl_add(const ipv6_addr_t *router_addr, unsigned iface);

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
 * @pre     `(router_addr != NULL)`
 *
 * @param[in] router_addr   The address of a default router. May not be NULL.
 * @param[in] iface         The interface to the node. May be 0 for any
 *                          interface.
 *
 * @return  The NIB entry for node with @p router_addr and @p iface onsuccess.
 * @return  NULL, if there is no such entry.
 */
_nib_dr_entry_t *_nib_drl_get(const ipv6_addr_t *router_addr, unsigned iface);

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
 * @pre `(next_hop != NULL)`
 * @pre `(pfx != NULL) && (pfx != "::") && (pfx_len != 0) && (pfx_len <= 128)`
 *
 * @param[in] next_hop  An IPv6 address to next hop. May not be NULL.
 *                      *May also be a global address!*
 * @param[in] iface     The interface to @p next_hop.
 * @param[in] pfx       The IPv6 prefix or address of the destination.
 *                      May not be NULL or unspecified address. Use
 *                      @ref _nib_drl_add() for default route destinations.
 * @param[in] pfx_len   The length in bits of @p pfx in bits.
 *
 * @return  A new or existing off-link entry with _nib_dr_entry_t::pfx set to
 *          @p pfx.
 * @return  NULL, if no space is left.
 */
_nib_offl_entry_t *_nib_dst_alloc(const ipv6_addr_t *next_hop, unsigned iface,
                                  const ipv6_addr_t *pfx, unsigned pfx_len);

/**
 * @brief   Clears out a NIB entry (off-link version)
 *
 * @param[in,out] dst  An entry.
 */
void _nib_dst_clear(_nib_offl_entry_t *dst);

/**
 * @brief   Iterates over off-link entries
 *
 * @param[in] last  Last entry (NULL to start).
 *
 * @return  entry after @p last.
 */
_nib_offl_entry_t *_nib_dst_iter(const _nib_offl_entry_t *last);

/**
 * @brief   Helper function for view-level add-functions below
 *
 * @pre     `(next_hop != NULL)`
 * @pre     `(pfx != NULL) && (pfx != "::") && (pfx_len != 0) && (pfx_len <= 128)`
 *
 * @param[in] next_hop  Next hop to the destination. May not be NULL.
 *                      *May also be a global address!*
 * @param[in] iface     The interface to the destination.
 * @param[in] pfx       The IPv6 prefix or address of the destination.
 *                      May not be NULL or unspecified address. Use
 *                      @ref _nib_drl_add() for default route destinations.
 * @param[in] pfx_len   The length in bits of @p pfx in bits.
 * @param[in] mode      [NIB-mode](_nib_onl_entry_t::mode).
 *
 * @return  A new or existing off-link entry with _nib_dr_entry_t::pfx set to
 *          @p pfx.
 * @return  NULL, if no space is left.
 */
static inline _nib_offl_entry_t *_nib_dst_add(const ipv6_addr_t *next_hop,
                                              unsigned iface,
                                              const ipv6_addr_t *pfx,
                                              unsigned pfx_len, uint8_t mode)
{
    _nib_offl_entry_t *nib_offl = _nib_dst_alloc(next_hop, iface, pfx, pfx_len);

    if (nib_offl != NULL) {
        nib_offl->next_hop->mode |= (mode);
    }
    return nib_offl;
}

/**
 * @brief   Helper function for the view-level remove-functions below
 *
 * @param[in,out] nib_offl  An entry.
 */
static inline void _nib_dst_remove(_nib_offl_entry_t *nib_offl, uint8_t mode)
{
    _nib_onl_entry_t *node = nib_offl->next_hop;

    if (node != NULL) {
        node->mode &= ~mode;
    }
    _nib_dst_clear(nib_offl);
}

#if defined(GNRC_IPV6_NIB_CONF_DC) || DOXYGEN
/**
 * @brief   Creates or gets an existing destination cache entry by its addresses
 *
 * @pre     `(next_hop != NULL)`
 * @pre     `(dst != NULL)`
 *
 * @param[in] next_hop  Next hop to the destination. May not be NULL.
 *                      *May also be a global address!*
 * @param[in] iface     The interface to the destination.
 *
 * @note    Only available if @ref GNRC_IPV6_NIB_CONF_DC.
 *
 * @return  A new or existing destination cache entry with
 *          _nib_onl_entry_t::ipv6 of _nib_dr_entry_t::next_hop set to
 *          @p next_hop.
 * @return  NULL, if no space is left.
 */
static inline _nib_offl_entry_t *_nib_dc_add(const ipv6_addr_t *next_hop,
                                             unsigned iface,
                                             const ipv6_addr_t *dst)
{
    return _nib_dst_add(next_hop, iface, dst, IPV6_ADDR_BIT_LEN, _DC);
}

/**
 * @brief   Removes a destination cache entry
 *
 * @param[in,out] nib_dr    An entry.
 *
 * Corresponding on-link entry is removed, too.
 *
 * @note    Only available if @ref GNRC_IPV6_NIB_CONF_DC.
 */
static inline void _nib_dc_remove(_nib_offl_entry_t *nib_offl)
{
    _nib_dst_remove(nib_offl, _DC);
}
#endif /* GNRC_IPV6_NIB_CONF_DC */

/**
 * @brief   Creates or gets an existing prefix list entry by its prefix
 *
 * @pre     `(next_hop != NULL)`
 * @pre     `(pfx != NULL) && (pfx != "::") && (pfx_len != 0) && (pfx_len <= 128)`
 *
 * @param[in] iface     The interface to the prefix is added to.
 * @param[in] pfx       The IPv6 prefix or address of the destination.
 *                      May not be NULL or unspecified address. Use
 *                      @ref _nib_drl_add() for default route destinations.
 * @param[in] pfx_len   The length in bits of @p pfx in bits.
 *
 * @return  A new or existing prefix list entry with _nib_dr_entry_t::pfx set to
 *          @p pfx.
 * @return  NULL, if no space is left.
 */
static inline _nib_offl_entry_t *_nib_pl_add(unsigned iface,
                                             const ipv6_addr_t *pfx,
                                             unsigned pfx_len)
{
    return _nib_dst_add(NULL, iface, pfx, pfx_len, _PL);
}

/**
 * @brief   Removes a prefix list entry
 *
 * @param[in,out] nib_offl    An entry.
 *
 * Corresponding on-link entry is removed, too.
 */
static inline void _nib_pl_remove(_nib_offl_entry_t *nib_offl)
{
    _nib_dst_remove(nib_offl, _PL);
}

#if defined(GNRC_IPV6_NIB_CONF_ROUTER) || DOXYGEN
/**
 * @brief   Creates or gets an existing forwarding table entry by its prefix
 *
 * @pre     `(next_hop != NULL)`
 * @pre     `(pfx != NULL) && (pfx != "::") && (pfx_len != 0) && (pfx_len <= 128)`
 *
 * @param[in] next_hop  Next hop to the destination. May not be NULL.
 *                      *May also be a global address!*
 * @param[in] iface     The interface to the destination.
 * @param[in] pfx       The IPv6 prefix or address of the destination.
 *                      May not be NULL or unspecified address. Use
 *                      @ref _nib_drl_add() for default route destinations.
 * @param[in] pfx_len   The length in bits of @p pfx in bits.
 *
 * @note    Only available if @ref GNRC_IPV6_NIB_CONF_ROUTER.
 *
 * @return  A new or existing forwarding table entry with _nib_dr_entry_t::pfx
 *          set to @p pfx.
 * @return  NULL, if no space is left.
 */
static inline _nib_offl_entry_t *_nib_ft_add(const ipv6_addr_t *next_hop,
                                             unsigned iface,
                                             const ipv6_addr_t *pfx,
                                             unsigned pfx_len)
{
    return _nib_dst_add(next_hop, iface, pfx, pfx_len, _FT);
}

/**
 * @brief   Removes a forwarding table entry
 *
 * @param[in,out] nib_offl    An entry.
 *
 * Corresponding on-link entry is removed, too.
 *
 * @note    Only available if @ref GNRC_IPV6_NIB_CONF_ROUTER.
 */
static inline void _nib_ft_remove(_nib_offl_entry_t *nib_offl)
{
    _nib_dst_remove(nib_offl, _FT);
}
#endif  /* GNRC_IPV6_NIB_CONF_ROUTER */

/**
 * @brief   Gets (or creates if it not exists) interface information for
 *          neighbor discovery
 *
 * @pre `(iface <= _NIB_IF_MAX)`
 *
 * @param[in] iface Interface identifier to get information for.
 *
 * @return  Interface information on @p iface.
 * @return  NULL, if no space left for interface.
 */
_nib_iface_t *_nib_iface_get(unsigned iface);

#ifdef __cplusplus
}
#endif

#endif /* PRIV_NIB_INTERNAL_H */
/** @} */
