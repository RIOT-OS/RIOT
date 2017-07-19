/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_netif2 New network interface API
 * @ingroup     net_gnrc
 * @brief       Abstraction layer for GNRC's network interfaces
 *
 * Network interfaces in the context of GNRC are threads for protocols that are
 * below the network layer.
 *
 * @{
 *
 * @file
 * @brief   Definition for GNRC's network interfaces
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef GNRC_NETIF2_H
#define GNRC_NETIF2_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "kernel_types.h"
#include "msg.h"
#include "net/ieee802154.h"
#include "net/ipv6/addr.h"
#include "net/ethernet/hdr.h"
#include "net/gnrc/netapi.h"
#ifdef MODULE_GNRC_IPV6_NIB
#include "net/gnrc/ipv6/nib/conf.h"
#endif
#include "net/gnrc/pkt.h"
#include "net/gnrc/mac/mac.h"
#include "net/netdev.h"
#include "net/netstats.h"
#include "rmutex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Maximum number of network interfaces
 *
 * @note    Intentially not calling it `GNRC_NETIF2_NUMOF` to not require
 *          rewrites throughout the stack.
 */
#ifndef GNRC_NETIF_NUMOF
#define GNRC_NETIF_NUMOF            (1)
#endif

/**
 * @brief   PID of the (virtual) loopback interface
 */
#define GNRC_NETIF2_LOOPBACK_PID    (KERNEL_PID_LAST + 1)

/**
 * @brief   Number of multicast addressed needed for @ref net_gnrc_rpl "RPL".
 *
 * @note    Used for calculation of @ref GNRC_NETIF2_IPV6_GROUPS_NUMOF
 */
#ifdef MODULE_GNRC_RPL
#define GNRC_NETIF2_RPL_ADDR        (1)
#else
#define GNRC_NETIF2_RPL_ADDR        (0)
#endif

/**
 * @brief   Number of multicast addresses needed for a @ref net_gnrc_ipv6 "IPv6"
 *          router
 *
 * @note    Used for calculation of @ref GNRC_NETIF2_IPV6_GROUPS_NUMOF
 */
#ifdef MODULE_GNRC_IPV6_ROUTER
#define GNRC_NETIF2_IPV6_RTR_ADDR   (1)
#else
#define GNRC_NETIF2_IPV6_RTR_ADDR   (0)
#endif

/**
 * @brief   Maximum number of unicast and anycast addresses per interface
 *
 * Default: 2 (link-local + corresponding global address)
 */
#ifndef GNRC_NETIF2_IPV6_ADDRS_NUMOF
#define GNRC_NETIF2_IPV6_ADDRS_NUMOF    (2)
#endif

/**
 * @brief   Maximum number of multicast groups per interface
 *
 * Default: 2 (all-nodes + solicited-nodes of link-local and global unicast
 * address) + @ref GNRC_NETIF2_RPL_ADDR + @ref GNRC_NETIF2_IPV6_RTR_ADDR
 */
#ifndef GNRC_NETIF2_IPV6_GROUPS_NUMOF
#define GNRC_NETIF2_IPV6_GROUPS_NUMOF   (2 + GNRC_NETIF2_RPL_ADDR + GNRC_NETIF2_IPV6_RTR_ADDR)
#endif

/**
 * @brief   Maximum length of the link-layer address.
 *
 * The value for the maximum length of a link-layer address is dependent
 * on the @ref drivers_netdev_api "netdev" adapters compiled in:
 *  - Setting it via `CFLAGS` has the most precedence.
 *  - The default is 8.
 *  - 1, if only @ref drivers_cc110x devices are compiled in.
 *  - @ref ETHERNET_ADDR_LEN, if additionally (or only) ethernet devices are
 *    compiled in.
 *  - @ref IEEE802154_LONG_ADDRESS_LEN, if additionally (or only) IEEE802.15.4
 *    devices are compiled in.
 *
 * @note Implementers note: From longest to shortest extend, if new link-layer
 *       address types are included
 */
#ifndef GNRC_NETIF2_L2ADDR_MAXLEN
#if defined(MODULE_NETDEV_IEEE802154) || defined(MODULE_XBEE)
#define GNRC_NETIF2_L2ADDR_MAXLEN   (IEEE802154_LONG_ADDRESS_LEN)
#elif   MODULE_NETDEV_ETH
#define GNRC_NETIF2_L2ADDR_MAXLEN   (ETHERNET_ADDR_LEN)
#elif   MODULE_CC110X
#define GNRC_NETIF2_L2ADDR_MAXLEN   (1U)
#else
#define GNRC_NETIF2_L2ADDR_MAXLEN   (8U)
#endif
#endif

#ifndef GNRC_NETIF2_DEFAULT_HL
#define GNRC_NETIF2_DEFAULT_HL      (64U)   /**< default hop limit */
#endif

/**
 * @name    Network interface flags
 * @anchor  net_gnrc_netif2_flags
 * @{
 */
/**
 * @brief   Network interface has link-layer address
 *
 * There are link-layers (e.g. SLIP) that do not have (nor require) link-layer
 * addresses. This flag signifies this fact to upper layers by leaving it unset.
 */
#define GNRC_NETIF2_FLAGS_HAS_L2ADDR                (0x00000001U)

/**
 * @brief   Network interface is enabled for IPv6 forwarding
 */
#define GNRC_NETIF2_FLAGS_IPV6_FORWARDING           (0x00000002U)

/**
 * @brief   Network interface advertises itself as an IPv6 router
 *          (implies @ref GNRC_NETIF2_FLAGS_IPV6_FORWARDING to be set)
 */
#define GNRC_NETIF2_FLAGS_IPV6_RTR_ADV              (0x00000004U)

/**
 * @brief   This interface advertises its gnrc_netif2_t::mtu to other nodes
 *          (implies @ref GNRC_NETIF2_FLAGS_IPV6_RTR_ADV to be set)
 */
#define GNRC_NETIF2_FLAGS_IPV6_ADV_MTU              (0x00000008U)

/**
 * @brief   This interface advertises its gnrc_netif2_t::cur_hl to other nodes
 *          (implies @ref GNRC_NETIF2_FLAGS_IPV6_RTR_ADV to be set)
 */
#define GNRC_NETIF2_FLAGS_IPV6_ADV_CUR_HL           (0x00000010U)

/**
 * @brief   This interface advertises its reachable time to other nodes
 *          (implies @ref GNRC_NETIF2_FLAGS_IPV6_RTR_ADV to be set)
 */
#define GNRC_NETIF2_FLAGS_IPV6_ADV_REACH_TIME       (0x00000020U)

/**
 * @brief   This interface advertises its retransmission timer to other nodes
 *          (implies @ref GNRC_NETIF2_FLAGS_IPV6_RTR_ADV to be set)
 */
#define GNRC_NETIF2_FLAGS_IPV6_ADV_RETRANS_TIMER    (0x00000040U)

/**
 * @brief   This interface uses 6Lo header compression
 *
 * @see [RFC 6282](https://tools.ietf.org/html/rfc6282)
 */
#define GNRC_NETIF2_FLAGS_6LO_HC                    (0x00000080U)

/**
 * @brief   This interface acts as a 6Lo border router to the LLN
 */
#define GNRC_NETIF2_FLAGS_6LO_ABR                   (0x00000100U)

/**
 * @brief   This interface acts as a mesh-under node (route-over topology when
 *          unset)
 */
#define GNRC_NETIF2_FLAGS_6LO_MESH                  (0x00000200U)

/**
 * @brief   Interface supports 6LoWPAN general header compression
 *
 * @attention   GHC not implemented yet
 * @see [RFC 7400, section 3.3](https://tools.ietf.org/html/rfc7400#section-3.3)
 */
#define GNRC_NETIF2_FLAGS_6LO_BACKBONE              (0x00000400U)

/**
 * @brief   Mask for @ref gnrc_mac_tx_feedback_t
 */
#define GNRC_NETIF2_FLAGS_MAC_TX_FEEDBACK_MASK      (0x00006000U)

/**
 * @brief   Flag to track if a transmission might have corrupted a received
 *          packet
 */
#define GNRC_NETIF2_FLAGS_MAC_RX_STARTED            (0x00008000U)
/** @} */

/**
 * @name    IPv6 unicast and anycast address flags
 * @anchor  net_gnrc_netif2_ipv6_addrs_flags
 * @{
 */
/**
 * @brief   Mask for the address' state
 */
#define GNRC_NETIF2_IPV6_ADDRS_FLAGS_STATE_MASK         (0x03U)

/**
 * @brief   Tentative address state
 */
#define GNRC_NETIF2_IPV6_ADDRS_FLAGS_STATE_TENTATIVE    (0x01U)

/**
 * @brief   Deprecated address state (still valid, but not preferred)
 */
#define GNRC_NETIF2_IPV6_ADDRS_FLAGS_STATE_DEPRECATED   (0x02U)

/**
 * @brief   Valid address state
 */
#define GNRC_NETIF2_IPV6_ADDRS_FLAGS_STATE_VALID        (0x03U)

/**
 * @brief   Address is an anycast address
 */
#define GNRC_NETIF2_IPV6_ADDRS_FLAGS_ANYCAST            (0x04U)
/** @} */

/**
 * @brief   Auto-address configuration modes
 */
enum {
    GNRC_NETIF2_AAC_NONE = 0,   /**< no configuration */
    GNRC_NETIF2_AAC_AUTO,       /**< Use some automatic bootstrapping (e.g. SLAAC with IPv6) */
    GNRC_NETIF2_AAC_DHCP,       /**< Use DHCP(v6) */
    /* extend if needed */
};

/**
 * @brief   Operations to an interface
 */
typedef struct gnrc_netif2_ops gnrc_netif2_ops_t;

/**
 * @brief   Representation of a network interface
 */
typedef struct {
    const gnrc_netif2_ops_t *ops;           /**< Operations of the network interface */
    netdev_t *dev;                          /**< Network device of the network interface */
#if defined(MODULE_GNRC_MAC) || DOXYGEN
#if ((GNRC_MAC_RX_QUEUE_SIZE != 0) || (GNRC_MAC_DISPATCH_BUFFER_SIZE != 0)) || DOXYGEN
    /**
     * @brief MAC internal object which stores reception parameters, queues, and
     *        state machines.
     *
     * @note    Only available if @ref GNRC_MAC_RX_QUEUE_SIZE or
     *          @ref GNRC_MAC_DISPATCH_BUFFER_SIZE is greater than 0.
     */
    gnrc_mac_rx_t rx;
#endif /* ((GNRC_MAC_RX_QUEUE_SIZE != 0) || (GNRC_MAC_DISPATCH_BUFFER_SIZE != 0)) || DOXYGEN */

#if ((GNRC_MAC_TX_QUEUE_SIZE != 0) || (GNRC_MAC_NEIGHBOR_COUNT != 0)) || DOXYGEN
    /**
     * @brief MAC internal object which stores transmission parameters, queues, and
     *        state machines.
     *
     * @note    Only available if @ref GNRC_MAC_TX_QUEUE_SIZE or
     *          @ref GNRC_MAC_NEIGHBOR_COUNT is greater than 0.
     */
    gnrc_mac_tx_t tx;
#endif /* ((GNRC_MAC_TX_QUEUE_SIZE != 0) || (GNRC_MAC_NEIGHBOR_COUNT == 0)) || DOXYGEN */
#endif  /* MODULE_GNRC_MAC */
#ifdef MODULE_NETSTATS_IPV6
    /**
     * @brief IPv6 packet statistics
     *
     * @note    Only available with module `netstats_ipv6`.
     */
    netstats_t ipv6_stats;
#endif
    rmutex_t mutex;                         /**< Mutex of the interface */
#if (GNRC_NETIF2_L2ADDR_MAXLEN > 0)
    /**
     * @brief   The link-layer address currently used as the source address
     *          on this interface.
     *
     * @note    Only available if @ref GNRC_NETIF2_L2ADDR_MAXLEN > 0
     */
    uint8_t l2addr[GNRC_NETIF2_L2ADDR_MAXLEN];
#endif
#if defined(MODULE_GNRC_IPV6) || DOXYGEN
    /**
     * @brief   Flags for gnrc_netif2_t::ipv6_addrs
     *
     * @see net_gnrc_netif2_ipv6_addrs_flags
     *
     * @note    Only available with module @ref net_gnrc_ipv6 "gnrc_ipv6".
     */
    uint8_t ipv6_addrs_flags[GNRC_NETIF2_IPV6_ADDRS_NUMOF];

    /**
     * @brief   IPv6 unicast and anycast addresses of the interface
     *
     * @note    Only available with module @ref net_gnrc_ipv6 "gnrc_ipv6".
     */
    ipv6_addr_t ipv6_addrs[GNRC_NETIF2_IPV6_ADDRS_NUMOF];

    /**
     * @brief   IPv6 multicast groups of the interface
     *
     * @note    Only available with module @ref net_gnrc_ipv6 "gnrc_ipv6".
     */
    ipv6_addr_t ipv6_groups[GNRC_NETIF2_IPV6_GROUPS_NUMOF];

#if defined(MODULE_GNRC_IPV6_NIB) || DOXYGEN
#if GNRC_IPV6_NIB_CONF_ROUTER || DOXYGEN
    /**
     * @brief   Route info callback
     *
     * This callback is called by the @ref net_gnrc_ipv6_nib "NIB" to inform
     * the routing protocol about state changes, route usages, missing routes
     * etc.
     *
     * @todo    Define types (RRQ, RRN, NSC) in NIB
     *
     * @param[in] type      Type of the route info.
     * @param[in] ctx_addr  Context address of the route info.
     * @param[in] ctx       Further context of the route info.
     */
    int (*route_info_cb)(unsigned type, const ipv6_addr_t *ctx_addr,
                         const void *ctx);
    /**
     * @brief   Event for @ref GNRC_IPV6_NIB_SND_MC_RA
     *
     * @note    Only available with module @ref net_gnrc_ipv6 "gnrc_ipv6".
     *          and @ref net_gnrc_ipv6_nib "NIB" and if
     *          @ref GNRC_IPV6_NIB_CONF_ROUTER != 0
     */
    evtimer_msg_event_t snd_mc_ra;
#endif  /* GNRC_IPV6_NIB_CONF_ROUTER */
#if GNRC_IPV6_NIB_CONF_ARSM || DOXYGEN
    /**
     * @brief   Event for @ref GNRC_IPV6_NIB_RECALC_REACH_TIME
     *
     * @note    Only available with module @ref net_gnrc_ipv6 "gnrc_ipv6".
     *          and @ref net_gnrc_ipv6_nib "NIB" and if
     *          @ref GNRC_IPV6_NIB_CONF_ARSM != 0
     */
    evtimer_msg_event_t recalc_reach_time;
#endif /* GNRC_IPV6_NIB_CONF_ARSM */
    /**
     * @brief   Event for @ref GNRC_IPV6_NIB_SEARCH_RTR
     *
     * @note    Only available with module @ref net_gnrc_ipv6 "gnrc_ipv6".
     *          and @ref net_gnrc_ipv6_nib "NIB"
     */
    evtimer_msg_event_t search_rtr;

#if GNRC_IPV6_NIB_CONF_ROUTER || DOXYGEN
    /**
     * @brief   Timestamp in milliseconds of last unsolicited router
     *          advertisement
     *
     * @note    Only available with module @ref net_gnrc_ipv6 "gnrc_ipv6".
     *          and @ref net_gnrc_ipv6_nib "NIB" and if
     *          @ref GNRC_IPV6_NIB_CONF_ROUTER != 0
     */
    uint32_t last_ra;
#endif  /* GNRC_IPV6_NIB_CONF_ROUTER */
#if GNRC_IPV6_NIB_CONF_ARSM || defined(DOXYGEN)
    /**
     * @brief   Base for random reachable time calculation and advertised
     *          reachable time in ms (if @ref GNRC_NETIF2_FLAGS_IPV6_RTR_ADV is
     *          set)
     *
     * @note    Only available with module @ref net_gnrc_ipv6 "gnrc_ipv6".
     *          and @ref net_gnrc_ipv6_nib "NIB" and if
     *          @ref GNRC_IPV6_NIB_CONF_ARSM != 0
     */
    uint32_t reach_time_base;

    /**
     * @brief   Reachable time (in ms)
     *
     * @note    Only available with module @ref net_gnrc_ipv6 "gnrc_ipv6".
     *          and @ref net_gnrc_ipv6_nib "NIB" and if
     *          @ref GNRC_IPV6_NIB_CONF_ARSM != 0
     */
    uint32_t reach_time;
#endif /* GNRC_IPV6_NIB_CONF_ARSM */
    /**
     * @brief   Retransmission time and advertised retransmission time (in ms)
     *
     * @note    Only available with module @ref net_gnrc_ipv6 "gnrc_ipv6" and
     *          @ref net_gnrc_ipv6_nib "NIB"
     */
    uint32_t retrans_time;
#endif  /* MODULE_GNRC_IPV6_NIB */

    /**
     * @brief   Maximum transition unit (MTU) for IPv6 packets
     *
     * @note    Only available with module @ref net_gnrc_ipv6 "gnrc_ipv6".
     */
    uint16_t ipv6_mtu;
#if defined(MODULE_GNRC_IPV6_NIB) || DOXYGEN
#if GNRC_IPV6_NIB_CONF_ROUTER || DOXYGEN
    /**
     * @brief   number of unsolicited router advertisements sent
     *
     * This only counts up to the first @ref NDP_MAX_INIT_RA_NUMOF on interface
     * initialization. The last @ref NDP_MAX_FIN_RA_NUMOF of an advertising
     * interface are counted from UINT8_MAX - @ref NDP_MAX_FIN_RA_NUMOF + 1.
     *
     * @note    Only available with module @ref net_gnrc_ipv6 "gnrc_ipv6".
     *          and @ref net_gnrc_ipv6_nib "NIB" and if
     *          @ref GNRC_IPV6_NIB_CONF_ROUTER != 0
     */
    uint8_t ra_sent;
#endif
    /**
     * @brief   number of unsolicited router solicitations scheduled
     *
     * @note    Only available with module @ref net_gnrc_ipv6 "gnrc_ipv6" and
     *          @ref net_gnrc_ipv6_nib "NIB"
     */
    uint8_t rs_sent;
    /**
     * @brief   number of unsolicited neighbor advertisements scheduled
     *
     * @note    Only available with module @ref net_gnrc_ipv6 "gnrc_ipv6" and
     *          @ref net_gnrc_ipv6_nib "NIB"
     */
    uint8_t na_sent;
#endif

    /**
     * @brief   IPv6 auto-address configuration mode
     *
     * @note    Only available with module @ref net_gnrc_ipv6 "gnrc_ipv6"
     */
    uint8_t ipv6_aac_mode;
#endif  /* gnrc_ipv6 */
    /**
     * @brief   Flags for the interface
     *
     * @see net_gnrc_netif2_flags
     */
    uint16_t flags;
    kernel_pid_t pid;                       /**< PID of the network interface's thread */
#if (GNRC_NETIF2_L2ADDR_MAXLEN > 0)
    /**
     * @brief   Length in bytes of gnrc_netif2_t::l2addr
     *
     * @note    Only available if @ref GNRC_NETIF2_L2ADDR_MAXLEN > 0
     */
    uint8_t l2addr_len;
#endif
#if defined(MODULE_GNRC_SIXLOWPAN_FRAG) || DOXYGEN
    /**
     * @brief   Maximum fragment size for 6LoWPAN fragmentation.
     *
     * @note    Only available with module
     *          @ref net_gnrc_sixlowpan_frag "gnrc_sixlowpan_frag".
     */
    uint8_t max_frag_size;
#endif
    uint8_t cur_hl;                         /**< Current hop-limit for out-going packets */
    uint8_t device_type;                    /**< Device type */
} gnrc_netif2_t;

/**
 * @see gnrc_netif2_ops_t
 */
struct gnrc_netif2_ops {
    /**
     * @brief   Send a @ref net_gnrc_pkt "packet" over the network interface
     *
     * @pre `netif != NULL && pkt != NULL`
     *
     * @note The function re-formats the content of @p pkt to a format expected
     *       by the netdev_driver_t::send() method of gnrc_netif_t::dev and
     *       releases the it before returning (so no additional release should
     *       be required after calling this method).
     *
     * @param[in] netif The network interface.
     * @param[in] pkt   A packet to send.
     *
     * @return  The number of bytes actually sent on success
     * @return  -EBADMSG, if the @ref net_gnrc_netif_hdr in @p pkt is missing
     *          or of an unexpected format.
     * @return  -ENOTSUP, if sending @p pkt in the given format isn't supported
     *          (e.g. empty payload with Ethernet).
     * @return  Any negative error code reported by gnrc_netif2_t::dev.
     */
    int (*send)(gnrc_netif2_t *netif, gnrc_pktsnip_t *pkt);

    /**
     * @brief   Receives a @ref net_gnrc_pkt "packet" from the network interface
     *
     * @pre `netif != NULL`
     *
     * @note The function takes the bytes received from via
     *       netdev_driver_t::recv() from gnrc_netif_t::dev and re-formats it to
     *       a @ref net_gnrc_pkt "packet" containing a @ref net_gnrc_netif_hdr
     *       and a payload header in receive order.
     *
     * @param[in] netif The network interface.
     *
     * @return  The packet received. Contains the payload (with the type marked
     *          accordingly) and a @ref net_gnrc_netif_hdr in receive order.
     * @return  NULL, if @ref net_gnrc_pktbuf was full.
     */
    gnrc_pktsnip_t *(*recv)(gnrc_netif2_t *netif);

    /**
     * @brief   Get an option from the network interface
     *
     * Use gnrc_netif2_get_from_netdev() to just get options from
     * gnrc_netif2_t::dev.
     *
     * @param[in] netif     The network interface.
     * @param[in] opt       The option parameters.
     *
     * @return  Number of bytes in @p data.
     * @return  -EOVERFLOW, if @p max_len is lesser than the required space.
     * @return  -ENOTSUP, if @p opt is not supported to be set.
     * @return  Any negative error code reported by gnrc_netif2_t::dev.
     */
    int (*get)(gnrc_netif2_t *netif, gnrc_netapi_opt_t *opt);

    /**
     * @brief   Get an option from the network interface
     *
     * Use gnrc_netif2_set_from_netdev() to just get options from
     * gnrc_netif2_t::dev.
     *
     * @param[in] netif     The network interface.
     * @param[in] opt       The option parameters.
     *
     * @return  Number of bytes written to gnrc_netif2_t::dev.
     * @return  -EOVERFLOW, if @p data_len is greater than the allotted space in
     *          gnrc_netif2_t::dev or gnrc_netif_t.
     * @return  -ENOTSUP, if @p opt is not supported to be set.
     * @return  Any negative error code reported by gnrc_netif2_t::dev.
     */
    int (*set)(gnrc_netif2_t *netif, const gnrc_netapi_opt_t *opt);

    /**
     * @brief   Message handler for network interface
     *
     * This message handler is used, when the network interface needs to handle
     * message types beyond the ones defined in @ref net_gnrc_netapi "netapi".
     * Leave NULL if this is not the case.
     *
     * @param[in] netif The network interface.
     * @param[in] msg   Message to be handled.
     */
    void (*msg_handler)(gnrc_netif2_t *netif, msg_t *msg);
};

/**
 * @brief   Deprecated address state (still valid, but not preferred)
 */
#define GNRC_NETIF2_IPV6_ADDRS_FLAGS_STATE_DEPRECATED   (0x02U)

/**
 * @brief   Creates a network interface
 *
 * @param[in] stack     The stack for the network interface's thread.
 * @param[in] stacksize Size of @p stack.
 * @param[in] priority  Priority for the network interface's thread.
 * @param[in] name      Name for the network interface. May be NULL.
 * @param[in] dev       Device for the interface.
 * @param[in] ops       Operations for the network interface.
 *
 * @note If @ref DEVELHELP is defined netif_params_t::name is used as the
 *       name of the network interface's thread.
 *
 * @return  The network interface on success.
 * @return  NULL, on error.
 */
gnrc_netif2_t *gnrc_netif2_create(char *stack, int stacksize, char priority,
                                  const char *name, netdev_t *dev,
                                  const gnrc_netif2_ops_t *ops);

/**
int gnrc_netif2_get_from_netdev(gnrc_netif2_t *netif, netopt_t opt, void *data,
                                size_t max_len);
 * @brief   Get number of network interfaces actually allocated
 *
 * @return  Number of network interfaces actually allocated
 */
unsigned gnrc_netif2_numof(void);

/**
 * @brief   Iterate over all network interfaces.
 *
 * @param[in] prev  previous interface in iteration. NULL to start iteration.
 *
 * @return  The next network interface after @p prev.
 * @return  NULL, if @p prev was the last network interface.
 */
gnrc_netif2_t *gnrc_netif2_iter(const gnrc_netif2_t *prev);

/**
 * @brief   Default operation for gnrc_netif2_ops_t::get()
 *
 * @note    Can also be used to be called *after* a custom operation.
 *
 * @param[in] netif     The network interface.
 * @param[out] opt      The option parameters.
 *
 * @return  Return value of netdev_driver_t::get() of gnrc_netif2_t::dev of
 *          @p netif.
 */
int gnrc_netif2_get_from_netdev(gnrc_netif2_t *netif, gnrc_netapi_opt_t *opt);

/**
 * @brief   Default operation for gnrc_netif2_ops_t::set()
 *
 * @note    Can also be used to be called *after* a custom operation.
 *
 * @param[in] netif     The network interface.
 * @param[in] opt       The option parameters.
 *
 * @return  Return value of netdev_driver_t::set() of gnrc_netif2_t::dev of
 *          @p netif.
 */
int gnrc_netif2_set_from_netdev(gnrc_netif2_t *netif,
                                const gnrc_netapi_opt_t *opt);

#ifdef __cplusplus
}
#endif

#endif /* GNRC_NETIF2_H */
/** @} */
