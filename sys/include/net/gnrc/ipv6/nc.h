/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    net_gnrc_ipv6_nc  IPv6 neighbor cache
 * @ingroup     net_gnrc_ipv6
 * @brief       Translates IPv6 addresses to link layer addresses.
 * @{
 *
 * @file
 * @brief       Neighbor cache definitions.
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#ifndef GNRC_IPV6_NC_H_
#define GNRC_IPV6_NC_H_

#include <stdbool.h>
#include <stdint.h>

#include "kernel_types.h"
#include "net/eui64.h"
#include "net/ipv6/addr.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/pktqueue.h"
#include "vtimer.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef GNRC_IPV6_NC_SIZE
/**
 * @brief   The size of the neighbor cache
 */
#define GNRC_IPV6_NC_SIZE           (GNRC_NETIF_NUMOF * 8)
#endif

#ifndef GNRC_IPV6_NC_L2_ADDR_MAX
/**
 * @brief   The maximum size of a link layer address
 */
#define GNRC_IPV6_NC_L2_ADDR_MAX    (8)
#endif

/**
 * @{
 * @name Flag definitions for gnrc_ipv6_nc_t
 */
/**
 * @{
 * @brief   States of a neighbor cache entry.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4861#section-7.3.2">
 *          RFC 4861, section 7.3.2
 *      </a>
 */
#define GNRC_IPV6_NC_STATE_MASK         (0x07)  /**< Mask for neighbor cache state */
#define GNRC_IPV6_NC_STATE_POS          (0)     /**< Shift of neighbor cache state */

#define GNRC_IPV6_NC_STATE_UNMANAGED    (0x00)  /**< The entry is not manage by NDP */

/**
 * @brief The entry is unreachable
 *
 * @see <a href="http://tools.ietf.org/html/rfc7048#section-3">
 *          RFC 7048, section 3
 *      </a>
 */
#define GNRC_IPV6_NC_STATE_UNREACHABLE  (0x01)
#define GNRC_IPV6_NC_STATE_INCOMPLETE   (0x02)  /**< Address resolution is performed */
#define GNRC_IPV6_NC_STATE_STALE        (0x03)  /**< The entry is stale */
#define GNRC_IPV6_NC_STATE_DELAY        (0x04)  /**< The entry was stale but packet was sent out */
#define GNRC_IPV6_NC_STATE_PROBE        (0x05)  /**< Periodic reachabality confirmation */
#define GNRC_IPV6_NC_STATE_REACHABLE    (0x07)  /**< The entry is reachable */
/**
 * @}
 */

#define GNRC_IPV6_NC_IS_ROUTER          (0x08)  /**< The neighbor is a router */

#define GNRC_IPV6_NC_TYPE_MASK          (0x30)  /**< Mask for neighbor cache state */
#define GNRC_IPV6_NC_TYPE_POS           (4)     /**< Shift of neighbor cache state */

/**
 * @{
 * @brief   States of a neighbor cache entry.
 *
 * @see <a href="http://tools.ietf.org/html/rfc6775#section-3.5">
 *          RFC 6775, section 3.5
 *      </a>
 */
/**
 * @brief The entry has no type
 *
 * @details The node sents multicast Neighbor Solicitations for hosts.
 */
#define GNRC_IPV6_NC_TYPE_NONE          (0x00)
#define GNRC_IPV6_NC_TYPE_GC            (0x10)  /**< The entry is marked for removal */
#define GNRC_IPV6_NC_TYPE_TENTATIVE     (0x20)  /**< The entry is temporary */
#define GNRC_IPV6_NC_TYPE_REGISTERED    (0x30)  /**< The entry is registered */
/**
 * @}
 */
/**
 * @}
 */

/**
 * @brief   Neighbor cache entry as defined in
 *          <a href="http://tools.ietf.org/html/rfc4861#section-5.1">
 *              RFC 4861, section 5.1
 *          </a>.
 */
typedef struct {
#ifdef MODULE_GNRC_NDP_NODE
    gnrc_pktqueue_t *pkts;                      /**< Packets waiting for address resolution */
#endif
    ipv6_addr_t ipv6_addr;                      /**< IPv6 address of the neighbor */
    uint8_t l2_addr[GNRC_IPV6_NC_L2_ADDR_MAX];  /**< Link layer address of the neighbor */
    uint8_t l2_addr_len;                        /**< Length of gnrc_ipv6_nc_t::l2_addr */
    uint8_t flags;                              /**< Flags as defined above */
    kernel_pid_t iface;                         /**< PID to the interface where the neighbor is */
    vtimer_t rtr_timeout;                       /**< timeout timer for router flag */

    /**
     * @brief (Re)Transmission timer for neighbor solicitations of this entry and
     *        timeout for states.
     */
    vtimer_t nbr_sol_timer;

    /**
     * @brief Delay timer for neighbor advertisements of this entry.
     *
     * @note Only needed for delayed anycast neighbor advertisements
     *
     * @see <a href="https://tools.ietf.org/html/rfc4861#section-7.2.7">
     *          RFC 4861, section 7.2.7
     *      </a>
     */
    vtimer_t nbr_adv_timer;

#ifdef MODULE_GNRC_SIXLOWPAN_ND_ROUTER
    vtimer_t type_timeout;                  /**< Timer for type transissions */
    eui64_t eui64;                          /**< the unique EUI-64 of the neighbor (might be
                                             *   different from L2 address, if l2_addr_len == 2) */
#endif

    uint8_t probes_remaining;               /**< remaining number of unanswered probes */
    /**
     * @}
     */
} gnrc_ipv6_nc_t;

/**
 * @brief   Initializes neighbor cache
 */
void gnrc_ipv6_nc_init(void);

/**
 * @brief   Adds a neighbor to the neighbor cache
 *
 * @param[in] iface         PID to the interface where the neighbor is.
 * @param[in] ipv6_addr     IPv6 address of the neighbor. Must not be NULL.
 * @param[in] l2_addr       Link layer address of the neighbor. NULL if unknown.
 * @param[in] l2_addr_len   Length of @p l2_addr, must be lesser than or equal
 *                          to GNRC_IPV6_L2_ADDR_MAX. 0 if unknown.
 * @param[in] flags         Flags for the entry
 *
 * @return  Pointer to new neighbor cache entry on success
 * @return  NULL, on failure
 */
gnrc_ipv6_nc_t *gnrc_ipv6_nc_add(kernel_pid_t iface, const ipv6_addr_t *ipv6_addr,
                                 const void *l2_addr, size_t l2_addr_len, uint8_t flags);

/**
 * @brief   Removes a neighbor from the neighbor cache
 *
 * @param[in] iface         PID to the interface where the neighbor is. If it
 *                          is KERNEL_PID_UNDEF it will be removed for all
 *                          interfaces.
 * @param[in] ipv6_addr     IPv6 address of the neighbor
 */
void gnrc_ipv6_nc_remove(kernel_pid_t iface, const ipv6_addr_t *ipv6_addr);

/**
 * @brief   Searches for any neighbor cache entry fitting the @p ipv6_addr.
 *
 * @param[in] iface         PID to the interface where the neighbor is. If it
 *                          is KERNEL_PID_UNDEF it will be searched on all
 *                          interfaces.
 * @param[in] ipv6_addr     An IPv6 address
 *
 * @return  The neighbor cache entry, if one is found.
 * @return  NULL, if none is found.
 */
gnrc_ipv6_nc_t *gnrc_ipv6_nc_get(kernel_pid_t iface, const ipv6_addr_t *ipv6_addr);

/**
 * @brief   Gets next entry in neighbor cache after @p prev.
 *
 * @param[in] prev  Previous entry. NULL to start iteration.
 *
 * @return  The next entry in neighbor cache.
 */
gnrc_ipv6_nc_t *gnrc_ipv6_nc_get_next(gnrc_ipv6_nc_t *prev);

/**
 * @brief   Gets next reachable router entry in neighbor cache after @p prev.
 *
 * @param[in] prev  Previous router entry. NULL to start iteration.
 *
 * @return  The next reachable router entry in neighbor cache.
 */
gnrc_ipv6_nc_t *gnrc_ipv6_nc_get_next_router(gnrc_ipv6_nc_t *prev);

/**
 * @brief   Returns the state of a neigbor cache entry.
 *
 * @param[in] entry A neighbor cache entry
 *
 * @return  The state of the neighbor cache entry as defined by its flags.
 */
static inline uint8_t gnrc_ipv6_nc_get_state(const gnrc_ipv6_nc_t *entry)
{
    return (entry->flags & GNRC_IPV6_NC_STATE_MASK);
}

/**
 * @brief   Returns the type of a neigbor cache entry.
 *
 * @param[in] entry A neighbor cache entry
 *
 * @return  The type of the neighbor cache entry as defined by its flags.
 */
static inline uint8_t gnrc_ipv6_nc_get_type(const gnrc_ipv6_nc_t *entry)
{
    return (entry->flags & GNRC_IPV6_NC_TYPE_MASK);
}

/**
 * @brief   Checks if an entry is reachable (do not confuse with
 *          @ref GNRC_IPV6_NC_STATE_REACHABLE).
 *
 * @param[in] entry A neighbor cache entry
 *
 * @return  true, if you can send packets to @p entry
 * @return  false, if you can't send packets to @p entry
 */
static inline bool gnrc_ipv6_nc_is_reachable(const gnrc_ipv6_nc_t *entry)
{
    switch (gnrc_ipv6_nc_get_state(entry)) {
        case GNRC_IPV6_NC_STATE_UNREACHABLE:
        case GNRC_IPV6_NC_STATE_INCOMPLETE:
            return false;

        default:
            return true;
    }
}

/**
 * @brief   Marks an entry as still reachable, if one with a fitting @p ipv6_addr
 *          can be found.
 *
 * @details This function can be used by upper layer protocols for neighbor
 *          discovery optimization to confirm that there was a reachability
 *          confirmation (e. g. an ACK in TCP) from the neighbor.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4861#section-7.3.1">
 *          RFC 4861, section 7.3.1
 *      </a>
 *
 * @param[in] ipv6_addr     An IPv6 address
 *
 * @return  The neighbor cache entry, if one is found.
 * @return  NULL, if none is found.
 */
gnrc_ipv6_nc_t *gnrc_ipv6_nc_still_reachable(const ipv6_addr_t *ipv6_addr);

#ifdef __cplusplus
}
#endif

#endif /* GNRC_IPV6_NC_H_ */
/**
 * @}
 */
