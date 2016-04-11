/*
 * Copyright (C) 2016 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_nib    Network information base
 * @ingroup     net_gnrc
 * @brief       Information base for network-based information
 *
 * Merges neighbor cache, destination cache, prefix list, and default router
 * list into a single data structure
 * @{
 *
 * @file
 * @brief   @ref net_gnrc_nib definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef GNRC_NIB_H_
#define GNRC_NIB_H_

#include <stddef.h>
#include <stdint.h>

#include "extimer.h"
#include "kernel_types.h"
#include "net/ipv4/addr.h"
#include "net/ipv6/addr.h"
#include "net/gnrc/ipv6/netif.h"
#include "net/gnrc/netif/hdr.h"
#include "net/gnrc/pkt.h"
#include "net/gnrc/pktqueue.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Option to queue packets when next hop information isn't available
 *          yet.
 */
#ifndef GNRC_NIB_QUEUE_PKTS
#define GNRC_NIB_QUEUE_PKTS (1)
#endif

/**
 * @name    NIB entry flags
 * @brief   Values for the gnrc_nib_entry_t::flags field
 * @{
 */
/**
 * @brief Entry contains neighbor cache information
 */
#define GNRC_NIB_FLAGS_NC                       (0x00000001U)

/**
 * @brief Entry contains destination cache information
 */
#define GNRC_NIB_FLAGS_DC                       (0x00000002U)

/**
 * @brief Entry contains prefix cache information
 */
#define GNRC_NIB_FLAGS_PREFIX_LIST              (0x00000004U)

/**
 * @brief Entry contains default router list information
 */
#define GNRC_NIB_FLAGS_DEFAULT_ROUTER_LIST      (0x00000008U)
/**
 * @brief   Neighbor cache states
 * @{
 */
#define GNRC_NIB_FLAGS_NC_STATE_MASK            (0x00000070U)
#define GNRC_NIB_FLAGS_NC_STATE_UNMANAGED       (0x00000000U)   /**< not managed by ND */

/**
 * @brief The entry is unreachable
 *
 * @see <a href="http://tools.ietf.org/html/rfc7048#section-3">
 *          RFC 7048, section 3
 *      </a>
 */
#define GNRC_NIB_FLAGS_NC_STATE_UNREACHABLE     (0x00000010U)
#define GNRC_NIB_FLAGS_NC_STATE_INCOMPLETE      (0x00000020U)   /**< Address resolution is performed
                                                                 */
#define GNRC_NIB_FLAGS_NC_STATE_STALE           (0x00000030U)   /**< The entry's L2 info is stale */
#define GNRC_NIB_FLAGS_NC_STATE_DELAY           (0x00000040U)   /**< entry is stale, but packet was
                                                                 *   send out */
#define GNRC_NIB_FLAGS_NC_STATE_PROBE           (0x00000050U)   /**< NUD is performed */
#define GNRC_NIB_FLAGS_NC_STATE_REACHABLE       (0x00000070U)   /**< Entry is reachable */
/** @} */

/**
 * @brief   This entry is a router
 *
 * This flag is different from @ref GNRC_NIB_FLAGS_DEFAULT_ROUTER_LIST in the
 * sense that @ref GNRC_NIB_FLAGS_IS_ROUTER is used by NUD to determine if an
 * entry is *still* a router to manipulate forwarding information, in case this
 * isn't the case anymore. @ref GNRC_NIB_FLAGS_DEFAULT_ROUTER_LIST on the other
 * hand is used for marking the packet as part of the default router list i.e.
 * that it is used to make a forwarding decision.
 */
#define GNRC_NIB_FLAGS_IS_ROUTER                (0x00000080U)

/**
 * @brief   6LoWPAN neighbor cache types
 * @see <a href="http://tools.ietf.org/html/rfc6775#section-3.5">
 *          RFC 6775, section 3.5
 *      </a>
 * @{
 */
#define GNRC_NIB_FLAGS_NC_TYPE_MASK             (0x00000300U)
#define GNRC_NIB_FLAGS_NC_TYPE_NONE             (0x00000000U)
#define GNRC_NIB_FLAGS_NC_TYPE_GC               (0x00000100U)   /**< The entry is marked for removal
                                                                 */
#define GNRC_NIB_FLAGS_NC_TYPE_TENTATIVE        (0x00000200U)   /**< The entry is temporary */
#define GNRC_NIB_FLAGS_NC_TYPE_REGISTERED       (0x00000300U)   /**< The entry is registered */
/** @} */

#define GNRC_NIB_FLAGS_PREFIX_VALID             (0x00000400U)   /**< The prefix is valid */

/** @} */

/**
 * @brief   Link-layer information for a NIB entry
 */
typedef struct {
    uint8_t addr[GNRC_NETIF_HDR_L2ADDR_MAX_LEN];    /**< link-layer address */
#ifdef MODULE_GNRC_SIXLOWPAN_ND_ROUTER
    eui64_t eui64;                                  /**< EUI-64 of the node to uniquely identify
                                                     *   the node */
#endif
    size_t addr_len;                                /**< length of gnrc_nib_l2_info_t::addr */
} gnrc_nib_l2_info_t;

/**
 * @brief   Event for sending a packet
 */
typedef struct {
    extimer_event_t event;      /**< extimer event */
    gnrc_pkt_t *pkt;            /**< packet to send */
} gnrc_nib_send_event_t;

typedef struct {
    gnrc_ipv6_netif_t *iface;                       /**< the interface associated with this
                                                     *   information */
    ipv6_addr_t *prefix;                            /**< prefix or destination */
    ipv6_addr_t *next_hop;                          /**< next hop to the destination */
    gnrc_nib_l2_info_t *l2;                         /**< link-layer information for next hop */
#if GNRC_NIB_QUEUE_PKTS
    gnrc_pktqueue_t *queued_pkts;                   /**< packets queued for destination */
#endif
    uint32_t flags;                                 /**< flags for the entry */
    extimer_event_t prefix_invalidation;            /**< prefix invalidation event */
    extimer_event_t router_invalidation;            /**< router invalidation event */
    extimer_event_t state_event;                    /**< state transition event */
#ifdef MODULE_GNRC_SIXLOWPAN_ND_ROUTER
    extimer_event_t type_event;                     /**< type transition event */
#endif
#ifdef MODULE_GNRC_SIXLOWPAN_ND
    gnrc_nib_send_event_t rereg;                    /**< Address re-registration timer */
#endif
#if defined(MODULE_GNRC_NDP_ROUTER) || defined(MODULE_GNRC_SIXLOWPAN_ND_ROUTER)
    gnrc_nib_send_event_t delayed_ra;               /**< event for delayed RAs */
#endif
    uint8_t prefix_len;                             /**< length of gnrc_nib_entry_t::prefix */
    uint8_t probes_remaining;                       /**< remaining probes for NUD */
    uint16_t pmtu;                                  /**< Path MTU to gnrc_nib_entry_t::prefix */
} gnrc_nib_entry_t;

#ifdef __cplusplus
}
#endif

#endif /* GNRC_NIB_H_ */
/** @} */
