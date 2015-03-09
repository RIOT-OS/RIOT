/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    net_ng_ndp  IPv6 Neighbor discovery
 * @ingroup     net_ng_icmpv6
 * @brief       IPv6 Neighbor Discovery Implementation
 * @{
 *
 * @file
 * @brief       Neighbor Discovery definitions
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <inttypes.h>

#include "byteorder.h"
#include "net/ng_pkt.h"
#include "net/ng_icmpv6.h"
#include "net/ng_ipv6/addr.h"
#include "net/ng_ipv6/netif.h"

#ifndef NG_NDP_H_
#define NG_NDP_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NG_NDP_DST_CACHE_SIZE
#define NG_NDP_DST_CACHE_SIZE       (3U)        /**< Size of the destination cache */
#endif

#define NG_NDP_MSG_RTR_ADV          (0x0210)    /**< Message type for periodic
                                                 *   Router advertisements */
#define NG_NDP_MSG_RTR_TIMEOUT      (0x0211)    /**< Message type for router timeouts */
#define NG_NDP_MSG_ADDR_TIMEOUT     (0x0212)    /**< Message type for address timeouts */

/**
 * @{
 * @name    Flags for router advertisement messages
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-4.2">
 *          RFC 4861, section 4.2
 *      </a>
 */
#define NG_NDP_RTR_ADV_FLAGS_MASK   (0xc0)
#define NG_NDP_RTR_ADV_FLAGS_M      (0x80)  /**< managed address configuration */
#define NG_NDP_RTR_ADV_FLAGS_O      (0x40)  /**< other configuration */
/**
 * @}
 */

/**
 * @{
 * @name    Flags for neighbor advertisement messages
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-4.4">
 *          RFC 4861, section 4.2
 *      </a>
 */
#define NG_NDP_NBR_ADV_FLAGS_MASK   (0xe0)
#define NG_NDP_NBR_ADV_FLAGS_R      (0x80)  /**< router */
#define NG_NDP_NBR_ADV_FLAGS_S      (0x40)  /**< solicited */
#define NG_NDP_NBR_ADV_FLAGS_O      (0x20)  /**< override */
/**
 * @}
 */

/**
 * @{
 * @name    NDP option types
 * @see <a href="http://www.iana.org/assignments/icmpv6-parameters/icmpv6-parameters.xhtml#icmpv6-parameters-5">
 *          IANA, IPv6 Neighbor Discovery Option Formats
 *      </a>
 */
#define NG_NDP_OPT_SL2A             (1)     /**< source link-layer address option */
#define NG_NDP_OPT_TL2A             (2)     /**< target link-layer address option */
#define NG_NDP_OPT_PI               (3)     /**< prefix information option */
#define NG_NDP_OPT_RH               (4)     /**< redirected option */
#define NG_NDP_OPT_MTU              (5)     /**< MTU option */
/**
 * @}
 */

/**
 * @{
 * @name    Flags for prefix information option
 */
#define NG_NDP_OPT_PI_FLAGS_MASK    (0xc0)
#define NG_NDP_OPT_PI_FLAGS_L       (0x80)  /**< on-link */
#define NG_NDP_OPT_PI_FLAGS_A       (0x40)  /**< autonomous address configuration */
/**
 * @}
 */

/**
 * @{
 * @name    Lengths for fixed length options
 * @brief   Options don't use bytes as their length unit, but 8 bytes.
 */
#define NG_NDP_OPT_PI_LEN           (4U)
#define NG_NDP_OPT_MTU_LEN          (1U)
/*
 * @}
 */

/**
 * @{
 * @name    Router constants
 * @see     <a href="https://tools.ietf.org/html/rfc4861#section-10">
 *              RFC 4861, section 10
 *          </a>
 */
/**
 * @brief   Initial router advertisement interval in microseconds
 */
#define NG_NDP_MAX_INIT_RTR_ADV_INT (16000000U)

/**
 * @brief   Maximum number of initial router advertisement transmissions
 */
#define NG_NDP_INIT_RTR_ADV_NUMOF   (3U)

/**
 * @brief   Maximum number of final router advertisement transmissions
 */
#define NG_NDP_FINAL_RTR_ADV_NUMOF  (3U)

/**
 * @brief   Minimum delay in microseconds between router advertisement
 *          transmissions
 */
#define NG_NDP_MIN_RTR_ADV_DELAY    (3000000U)

/**
 * @brief   Upper bound for randomised delay in microseconds between router
 *          solicitation reception and responding router advertisement
 *          transmission.
 */
#define NG_NDP_MAX_RTR_ADV_DELAY    (500000U)
/*
 * @}
 */

/**
 * @{
 * @name    Host constants
 * @see     <a href="https://tools.ietf.org/html/rfc4861#section-10">
 *              RFC 4861, section 10
 *          </a>
 */
/**
 * @brief   Upper bound for randomised delay in microseconds for initial
 *          router solicitation transmissions
 */
#define NG_NDP_MAX_RTR_SOL_DELAY    (1000000U)

/**
 * @brief   Interval in microseconds between initial router solicitation
 *          transmissions
 */
#define NG_NDP_MAX_RTR_SOL_INT      (4000000U)

/**
 * @brief   Maximum number of  initial router solicitation transmissions
 */
#define NG_NDP_MAX_RTR_SOL_NUMOF    (3U)
/*
 * @}
 */

/**
 * @{
 * @name    Node constants
 * @see     <a href="https://tools.ietf.org/html/rfc4861#section-10">
 *              RFC 4861, section 10
 *          </a>
 */
/**
 * @brief   Maximum number of unanswered multicast neighbor solicitations
 *          before address resolution is considered failed.
 */
#define NG_NDP_MAX_MC_NBR_SOL_NUMOF (3U)

/**
 * @brief   Maximum number of unanswered unicast neighbor solicitations before
 *          an address is considered unreachable.
 */
#define NG_NDP_MAX_UC_NBR_SOL_NUMOF (3U)

/**
 * @brief   Upper bound of randomized delay in microseconds for a solicited
 *          neighbor advertisement transmission for an anycast target.
 */
#define NG_NDP_MAX_AC_TGT_DELAY     (3000000U)

/**
 * @brief   Maximum number of unsolicited neighbor advertisements before on
 *          link-layer address change.
 */
#define NG_NDP_MAX_NBR_ADV_NUMOF    (3U)

/**
 * @brief   Base value in mircoseconds for computing randomised
 *          reachable time.
 */
#define NG_NDP_REACH_TIME           (30000000U)

/**
 * @brief   Time in mircoseconds between retransmissions of neighbor
 *          solicitations to a neighbor.
 */
#define NG_NDP_RETRANS_TIMER        (1000000U)

/**
 * @brief   Delay in mircoseconds for neighbor cache entry between entering
 *          DELAY state and entering PROBE state if no reachability
 *          confirmation has been received.
 */
#define NG_NDP_FIRST_PROBE_DELAY    (5000000U)

/**
 * @brief   Lower bound for randomised reachable time calculation.
 */
#define NG_NDP_MIN_RAND             (5U)

/**
 * @brief   Upper bound for randomised reachable time calculatio.
 */
#define NG_NDP_MAX_RAND             (15U)
/**
 * @}
 */

/**
 * @{
 * @name    Default values for configuration variables
 *
 * @brief   The variables are defined in @ref ng_ipv6_netif_t
 *
 * @see     <a href="https://tools.ietf.org/html/rfc4861#section-6.2.1">
 *              RFC 4861, section 6.2.1
 *          </a>
 * @see     <a href="https://tools.ietf.org/html/rfc4861#section-6.3.2">
 *              RFC 4861, section 6.3.2
 *          </a>
 */
/**
 * @brief   Flag to indicate that routing is enabled on the interface
 * @details Since (un-)setting this flag option triggers behavior for the
 *          interface it **must** be set using ng_ndp_netif_add_router() and
 *          unset using ng_ndp_netif_remove_router().
 */
#define NG_NDP_NETIF_FLAGS_ROUTER               (0x0001)

/**
 * @brief   Flag to indicate that the interface sends periodic router
 *          advertisements and in response to router solicitations.
 * @details Since (un-)setting this flag option triggers behavior for the
 *          interface it **must** be (un-)set using ng_ndp_netif_rtr_adv().
 */
#define NG_NDP_NETIF_FLAGS_RTR_ADV              (0x0002)

/**
 * @brief   Flag to indicate that ng_ipv6_netif_t::mtu shall be propagated
 *          with the MTU options in router advertisements.
 */
#define NG_NDP_NETIF_FLAGS_ADV_MTU              (0x0004)

/**
 * @brief   Flag to indicate that ng_ipv6_netif_t::cur_hl shall be propagated
 *          in router advertisements.
 */
#define NG_NDP_NETIF_FLAGS_ADV_CUR_HL           (0x0008)

/**
 * @brief   Flag to indicate that ng_ipv6_netif_t::reach_time shall be propagated
 *          in router advertisements.
 */
#define NG_NDP_NETIF_FLAGS_ADV_REACH_TIME       (0x0010)

/**
 * @brief   Flag to indicate that ng_ipv6_netif_t::retrans_timer shall be
 *          propagated in router advertisements.
 */
#define NG_NDP_NETIF_FLAGS_ADV_RETRANS_TIMER    (0x20)

/**
 * @brief   Flag to indicate that the interface has other address
 *          configuration.
 * @see     @ref NG_NDP_RTR_ADV_FLAGS_O
 */
#define NG_NDP_NETIF_FLAGS_OTHER_CONF           (NG_NDP_RTR_ADV_FLAGS_O)

/**
 * @brief   Flag to indicate that the interface has managed address
 *          configuration.
 * @see     @ref NG_NDP_RTR_ADV_FLAGS_M
 */
#define NG_NDP_NETIF_FLAGS_MANAGED              (NG_NDP_RTR_ADV_FLAGS_M)

/**
 * @brief   Maximum time in seconds between sending unsolicited multicast router
 *          advertisements.
 */
#define NG_NDP_NETIF_MAX_ADV_INT_DEFAULT    (600U)

/**
 * @brief   Minimum time in seconds between sending unsolicited multicast router
 *          advertisements.
 *
 * @note    If you change @ref NG_NDP_NETIF_MAX_ADV_INT_DEFAULT to a value < 9
 *          this value must be equal to @ref NG_NDP_NETIF_MAX_ADV_INT_DEFAULT.
 */
#define NG_NDP_NETIF_MIN_ADV_INT_DEFAULT    (NG_NDP_NETIF_MAX_ADV_INT_DEFAULT / 3)

/**
 * @brief   The router lifetime to propagate in router advertisements.
 */
#define NG_NDP_NETIF_ADV_LTIME              (NG_NDP_NETIF_MAX_ADV_INT_DEFAULT * 3)

/**
 * @}
 */

/**
 * @brief   Router solicitation message format.
 * @extends ng_icmpv6_hdr_t
 *
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-4.1">
 *          RFC 4861, section 4.1
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;           /**< message type */
    uint8_t code;           /**< message code */
    network_uint16_t csum;  /**< checksum */
    uint32_t resv;          /**< reserved field */
} ng_ndp_rtr_sol_t;

/**
 * @brief   Router advertisement message format.
 * @extends ng_icmpv6_hdr_t
 *
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-4.2">
 *          RFC 4861, section 4.2
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;                       /**< message type */
    uint8_t code;                       /**< message code */
    network_uint16_t csum;              /**< checksum */
    uint8_t cur_hl;                     /**< current hop limit */
    uint8_t flags;                      /**< flags */
    network_uint16_t ltime;             /**< router lifetime */
    network_uint32_t reach_time;        /**< reachable time */
    network_uint32_t retrans_timer;     /**< retransmission timer */
} ng_ndp_rtr_adv_t;

/**
 * @brief   Neighbor solicitation message format.
 * @extends ng_icmpv6_hdr_t
 *
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-4.3">
 *          RFC 4861, section 4.3
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;           /**< message type */
    uint8_t code;           /**< message code */
    network_uint16_t csum;  /**< checksum */
    uint32_t resv;          /**< reserved field */
    ng_ipv6_addr_t tgt;     /**< target address */
} ng_ndp_nbr_sol_t;

/**
 * @brief   Neighbor advertisement message format.
 * @extends ng_icmpv6_hdr_t
 *
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-4.4">
 *          RFC 4861, section 4.4
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;           /**< message type */
    uint8_t code;           /**< message code */
    network_uint16_t csum;  /**< checksum */
    uint8_t flags;          /**< flags */
    uint8_t resv[3];        /**< reserved fields */
    ng_ipv6_addr_t tgt;     /**< target address */
} ng_ndp_nbr_adv_t;

/**
 * @brief   Neighbor advertisement message format.
 * @extends ng_icmpv6_hdr_t
 *
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-4.5">
 *          RFC 4861, section 4.5
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;           /**< message type */
    uint8_t code;           /**< message code */
    network_uint16_t csum;  /**< checksum */
    uint32_t resv;          /**< reserved field */
    ng_ipv6_addr_t tgt;     /**< target address */
    ng_ipv6_addr_t dst;     /**< destination address */
} ng_ndp_redirect_t;

/**
 * @brief   General NDP option format
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-4.6">
 *          RFC 4861, section 4.6
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;   /**< option type */
    uint8_t len;    /**< length in units of 8 octets */
} ng_ndp_opt_t;

/* XXX: slla and tlla are just ng_ndp_opt_t with variable link layer address
 * appended */

/**
 * @brief   Prefix information option format
 * @extends ng_ndp_opt_t
 *
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-4.6.2">
 *          RFC 4861, section 4.6.2
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;                   /**< option type */
    uint8_t len;                    /**< length in units of 8 octets */
    uint8_t prefix_len;             /**< prefix length */
    uint8_t flags;                  /**< flags */
    network_uint32_t valid_ltime;   /**< valid lifetime */
    network_uint32_t pref_ltime;    /**< preferred lifetime */
    uint32_t resv;                  /**< reserved field */
    ng_ipv6_addr_t prefix;          /**< prefix */
} ng_ndp_opt_pi_t;

/**
 * @brief   Redirected header option format
 * @extends ng_ndp_opt_t
 *
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-4.6.3">
 *          RFC 4861, section 4.6.3
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;           /**< option type */
    uint8_t len;            /**< length in units of 8 octets */
    uint8_t resv[6];        /**< reserved field */
} ng_ndp_opt_rh_t;

/**
 * @brief   MTU option format
 * @extends ng_ndp_opt_t
 *
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-4.6.4">
 *          RFC 4861, section 4.6.4
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;           /**< option type */
    uint8_t len;            /**< length in units of 8 octets */
    uint16_t resv;          /**< reserved field */
    network_uint32_t mtu;   /**< MTU */
} ng_ndp_opt_mtu_t;

/**
 * @brief   Demultiplexes a received neighbor discovery message.
 *
 * @param[in] iface         The receiving interface
 * @param[in] pkt           The packet containing @p ipv6 and @p icmpv6.
 * @param[in] ipv6          The IPv6 header in @p pkt.
 * @param[in] icmpv6        The ICMPv6 datagram in @p pkt.
 * @param[in] icmpv6_size   Size of @p icmpv6.
 *
 */
void ng_ndp_demux(kernel_pid_t iface, ng_pktsnip_t *pkt, ng_ipv6_hdr_t *ipv6,
                  ng_icmpv6_hdr_t *icmpv6, size_t icmpv6_size);

/**
 * @brief   NDP interface initialization.
 *
 * @param[in] iface     An IPv6 interface descriptor. Must not be NULL.
 */
void ng_ndp_netif_add(ng_ipv6_netif_t *iface);

/**
 * @brief   NDP interface removal.
 *
 * @param[in] iface     An IPv6 interface descriptor. Must not be NULL.
 */
void ng_ndp_netif_remove(ng_ipv6_netif_t *iface);

/**
 * @brief   Get link-layer address and interface for next hop to destination
 *          IPv6 address.
 *
 * @param[out] l2addr           The link-layer for the next hop to @p dst.
 * @param[out] l2addr_len       Length of @p l2addr.
 * @param[in] dst               An IPv6 address to search the next hop for.
 * @param[in] pkt               Packet to send to @p dst. Leave NULL if you
 *                              just want to get the addresses.
 *
 * @return  The PID of the interface, on success.
 * @return  -EHOSTUNREACH, if @p dst is not reachable.
 * @return  -ENOBUFS, if @p l2addr_len was smaller than the resulting @p l2addr
 *          would be long.
 */
kernel_pid_t ng_ndp_next_hop_l2addr(uint8_t *l2addr, uint8_t *l2addr_len,
                                    const ng_ipv6_addr_t *dst,
                                    ng_pktsnip_t *pkt);

/**
 * @brief   Builds a router solicitation message for sending.
 *
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-4.1">
 *          RFC 4861, section 4.1
 *      </a>
 *
 * @param[in] options   Options to append to the router solicitation.
 *
 * @return  The resulting ICMPv6 packet on success.
 * @return  NULL, on failure.
 */
ng_pktsnip_t *ng_ndp_rtr_sol_build(ng_pktsnip_t *options);

/**
 * @brief   Builds a neighbor solicitation message for sending.
 *
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-4.3">
 *          RFC 4861, section 4.3
 *      </a>
 *
 * @param[in] tgt       The target address.
 * @param[in] options   Options to append to the router solicitation.
 *
 * @return  The resulting ICMPv6 packet on success.
 * @return  NULL, on failure.
 */
ng_pktsnip_t *ng_ndp_nbr_sol_build(ng_ipv6_addr_t *tgt, ng_pktsnip_t *options);

/**
 * @brief   Builds a neighbor advertisement message for sending.
 *
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-4.4">
 *          RFC 4861, section 4.4
 *      </a>
 *
 * @param[in] flags     Flags as defined above.
 *                      @ref NG_NDP_NBR_ADV_FLAGS_R == 1 indicates, that the
 *                      sender is a router,
 *                      @ref NG_NDP_NBR_ADV_FLAGS_S == 1 indicates that the
 *                      advertisement was sent in response to a neighbor
 *                      solicitation,
 *                      @ref NG_NDP_NBR_ADV_FLAGS_O == 1 indicates that the
 *                      advertisement should override an existing cache entry
 *                      and update the cached link-layer address.
 * @param[in] tgt       For solicited advertisements, the Target Address field
 *                      in the neighbor solicitaton.
 *                      For and unsolicited advertisement, the address whose
 *                      link-layer addres has changed.
 *                      MUST NOT be multicast.
 * @param[in] options   Options to append to the neighbor advertisement.
 *
 * @return  The resulting ICMPv6 packet on success.
 * @return  NULL, on failure.
 */
ng_pktsnip_t *ng_ndp_nbr_adv_build(uint8_t flags, ng_ipv6_addr_t *tgt,
                                   ng_pktsnip_t *options);

/**
 * @brief   Builds a redirect message for sending.
 *
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-4.5">
 *          RFC 4861, section 4.5
 *      </a>
 *
 * @param[in] tgt       Address that is a better first hop for @p dst.
 * @param[in] dst       Address of the destination that is redirected to the
 *                      target.
 * @param[in] options   Options to append to the neighbor advertisement.
 *
 * @return  The resulting ICMPv6 packet on success.
 * @return  NULL, on failure.
 */
ng_pktsnip_t *ng_ndp_redirect_build(ng_ipv6_addr_t *tgt, ng_ipv6_addr_t *dst,
                                    ng_pktsnip_t *options);

/**
 * @brief   Builds the source link-layer address option.
 *
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-4.6.1">
 *          RFC 4861, section 4.6.1
 *      </a>
 *
 * @note    Must only be used with neighbor solicitations, router solicitations,
 *          and router advertisements. This is not checked however, since
 *          hosts should silently ignore it in other NDP messages.
 *
 * @param[in] l2addr        A link-layer address of variable length.
 * @param[in] l2addr_len    Length of @p l2addr.
 * @param[in] more          More options in the packet. NULL, if there are none.
 *
 * @return  The packet snip list of options, on success
 * @return  NULL, if packet buffer is full
 */
ng_pktsnip_t *ng_ndp_opt_sl2a_build(const uint8_t *l2addr, uint8_t l2addr_len,
                                    ng_pktsnip_t *more);

/**
 * @brief   Builds the target link-layer address option.
 *
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-4.6.1">
 *          RFC 4861, section 4.6.1
 *      </a>
 *
 * @note    Must only be used with neighbor advertisemnents and redirect packets.
 *          This is not checked however, since hosts should silently ignore it
 *          in other NDP messages.
 *
 * @param[in] l2addr        A link-layer address of variable length.
 * @param[in] l2addr_len    Length of @p l2addr.
 * @param[in] more          More options in the packet. NULL, if there are none.
 *
 * @return  The pkt snip list of options, on success
 * @return  NULL, if packet buffer is full
 */
ng_pktsnip_t *ng_ndp_opt_tl2a_build(const uint8_t *l2addr, uint8_t l2addr_len,
                                    ng_pktsnip_t *more);

/**
 * @brief   Builds the redirected header option.
 *
 * @param[in] data  The original packet. Must be in sending context and contain
 *                  only an unparsed NG_NETTYPE_IPV6 packet snip and the
 *                  NG_NETTYPE_NETIF header.
 * @param[in] more  More options in the packet. NULL, if there are none.
 *
 * @return  The packet snip list of options, on success
 * @return  NULL, if packet buffer is full
 * @return  NULL, if @p data was malformed
 */
ng_pktsnip_t *ng_ndp_opt_rh_build(ng_pktsnip_t *data, ng_pktsnip_t *more);

#ifdef MODULE_NG_IPV6_ROUTER
/**
 * @brief   Activates periodic sending of neighbor advertisements on the
 *          interface if @ref NG_NDP_NETIF_FLAGS_ROUTER and
 *          @ref NG_NDP_NETIF_FLAGS_RTR_ADV are set on the interface.
 *
 * @param[in] iface An interface descriptor. Must not be NULL.
 */
void ng_ndp_netif_advertise_router(ng_ipv6_netif_t *iface);

/**
 * @brief   NDP interface initialization for routers.
 *
 * @details Always sets both the @ref NG_NDP_NETIF_FLAGS_ROUTER **and** the
 *          @ref NG_NDP_NETIF_FLAGS_RTR_ADV flags for @p iface.
 *
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-6.2.4">
 *          RFC 4861, section 6.2.4
 *      </a>
 *
 * @param[in] iface     An IPv6 interface. Must not be NULL.
 */
void ng_ndp_netif_add_router(ng_ipv6_netif_t *iface);

/**
 * @brief   NDP interface removal for routers.
 *
 * @details Unsets only the @ref NG_NDP_NETIF_FLAGS_ROUTER flag
 *
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-6.2.5">
 *          RFC 4861, section 6.2.5
 *      </a>
 *
 * @param[in] iface     An IPv6 interface. Must not be NULL.
 */
void ng_ndp_netif_remove_router(ng_ipv6_netif_t *iface);

/**
 * @brief   Activates and deactivates periodic sending of router advertisements
 *          on this interface.
 *
 * @details (Un-)sets the @ref NG_NDP_NETIF_FLAGS_RTR_ADV flag
 *
 * @param[in] iface     An IPv6 interface. Must not be NULL.
 * @param[in] active    true to activate periodic router advertisements, false
 *                      to deactivate it.
 */
void ng_ndp_netif_rtr_adv(ng_ipv6_netif_t *iface, bool active);

/**
 * @brief   Builds a router advertisement message for sending.
 *
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-4.2">
 *          RFC 4861, section 4.2
 *      </a>
 *
 * @note    The source address for the packet MUST be the link-local address
 *          of the interface.
 *
 * @param[in] cur_hl        Default hop limit for outgoing IP packets, 0 if
 *                          unspecified by this router.
 * @param[in] flags         Flags as defined above.
 *                          @ref NG_NDP_RTR_ADV_FLAGS_M == 1 indicates, that the
 *                          addresses are managed by DHCPv6,
 *                          @ref NG_NDP_RTR_ADV_FLAGS_O == 1 indicates that other
 *                          configuration information is available via DHCPv6.
 * @param[in] ltime         Lifetime of the default router in seconds.
 * @param[in] reach_time    Time in milliseconds a node should assume a neighbor
 *                          reachable. 0 means unspecified by the router.
 * @param[in] retrans_timer Time in milliseconds between retransmitted
 *                          neighbor solicitations. 0 means unspecified by
 *                          the router.
 * @param[in] options       Options to append to the router advertisement.
 *
 * @return  The resulting ICMPv6 packet on success.
 * @return  NULL, on failure.
 */
ng_pktsnip_t *ng_ndp_rtr_adv_build(uint8_t cur_hl, uint8_t flags,
                                   uint16_t ltime, uint32_t reach_time,
                                   uint32_t retrans_timer, ng_pktsnip_t *options);

/**
 * @brief   Builds the prefix information option.
 *
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-4.6.2">
 *          RFC 4861, section 4.6.2
 *      </a>
 *
 * @note    Must only be used with router advertisemnents. This is not checked
 *          however, since hosts should silently ignore it in other NDP messages.
 *
 * @param[in] prefix_len    The length of @p prefix in bits. Must be between
 *                          0 and 128.
 * @param[in] flags         Flags as defined above.
 *                          @ref NG_NDP_OPT_PI_FLAGS_L == 1 indicates, that
 *                          @p prefix can be used for on-link determination,
 *                          @ref NG_NDP_OPT_PI_FLAGS_A == 1 indicates, that
 *                          @p prefix can be used for stateless address
 *                          configuration.
 * @param[in] valid_ltime   Length of time in seconds that @p prefix is valid.
 *                          UINT32_MAX represents infinity.
 * @param[in] pref_ltime    Length of time in seconds that addresses using
 *                          @p prefix remain prefered. UINT32_MAX represents
 *                          infinity.
 * @param[in] prefix        An IPv6 address or a prefix of an IPv6 address.
 * @param[in] more          More options in the packet. NULL, if there are none.
 *
 * @return  The packet snip list of options, on success
 * @return  NULL, if packet buffer is full
 */
ng_pktsnip_t *ng_ndp_opt_pi_build(uint8_t prefix_len, uint8_t flags,
                                  uint32_t valid_ltime, uint32_t pref_ltime,
                                  ng_ipv6_addr_t *prefix, ng_pktsnip_t *more);

/**
 * @brief   Builds the MTU option.
 *
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-4.6.4">
 *          RFC 4861, section 4.6.4
 *      </a>
 *
 * @note    Must only be used with router advertisemnents. This is not checked
 *          however, since hosts should silently ignore it in other NDP messages.
 *
 * @param[in] mtu           The recommended MTU for the link.
 * @param[in] more          More options in the packet. NULL, if there are none.
 *
 * @return  The packet snip list of options, on success
 * @return  NULL, if packet buffer is full
 */
ng_pktsnip_t *ng_ndp_opt_mtu_build(uint32_t mtu, ng_pktsnip_t *more);
#else
/**
 * @brief   Stub for non-routing hosts
 *
 * @param[in] iface An interface descriptor.
 */
#define ng_ndp_netif_advertise_router(iface);

/**
 * @brief   Stub for non-routing hosts
 *
 * @param[in] iface An IPv6 interface
 */
#define ng_ndp_netif_add_router(iface)

/**
 * @brief   Stub for non-routing hosts
 *
 * @param[in] iface An IPv6 interface
 */
#define ng_ndp_netif_remove_router(iface)

/**
 * @brief   Stub for non-routing hosts
 *
 * @param[in] iface     An IPv6 interface.
 * @param[in] active    true to activate periodic router advertisements, false
 *                      to deactivate it.
 */
#define ng_ndp_netif_rtr_adv(iface, active)

/**
 * @brief   Stub for non-routing hosts
 *
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-4.2">
 *          RFC 4861, section 4.2
 *      </a>
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-6.2.4">
 *          RFC 4861, section 6.2.4
 *      </a>
 *
 * @note    Non-routing nodes should not send router advertisements, this
 *          is why this implementation always returns NULL.
 *
 * @param[in] cur_hl        Default hop limit for outgoing IP packets, 0 if
 *                          unspecified by this router.
 * @param[in] flags         Flags as defined above.
 *                          @ref NG_NDP_RTR_ADV_FLAGS_M == 1 indicates, that the
 *                          addresses are managed by DHCPv6,
 *                          @ref NG_NDP_RTR_ADV_FLAGS_O == 1 indicates that other
 *                          configuration information is available via DHCPv6.
 * @param[in] ltime         Lifetime of the default router in seconds.
 * @param[in] reach_time    Time in milliseconds a node should assume a neighbor
 *                          reachable. 0 means unspecified by the router.
 * @param[in] retrans_timer Time in milliseconds between retransmitted
 *                          neighbor solicitations. 0 means unspecified by
 *                          the router.
 * @param[in] options       Options to append to the router advertisement.
 *
 * @return NULL
 */
#define ng_ndp_rtr_adv_build(cur_hl, flags, ltime, reach_time, retrans_timer, \
                             options)   (NULL)

/**
 * @brief   Stub for non-routing hosts
 *
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-4.6.2">
 *          RFC 4861, section 4.6.2
 *      </a>
 *
 * @note    Since this option MUST only be used with router advertisements and
 *          sending of router advertisements is not allowed for non-routing
 *          hosts, this implementation always returns NULL.
 *
 * @param[in] prefix_len    The length of @p prefix in bits. Must be between
 *                          0 and 128.
 * @param[in] flags         Flags as defined above.
 *                          @ref NG_NDP_OPT_PI_FLAGS_L == 1 indicates, that
 *                          @p prefix can be used for on-link determination,
 *                          @ref NG_NDP_OPT_PI_FLAGS_A == 1 indicates, that
 *                          @p prefix can be used for stateless address
 *                          configuration.
 * @param[in] valid_ltime   Length of time in seconds that @p prefix is valid.
 *                          UINT32_MAX represents infinity.
 * @param[in] pref_ltime    Length of time in seconds that addresses using
 *                          @p prefix remain prefered. UINT32_MAX represents
 *                          infinity.
 * @param[in] prefix        An IPv6 address or a prefix of an IPv6 address.
 * @param[in] more          More options in the packet. NULL, if there are none.
 *
 * @return  NULL
 */
#define ng_ndp_opt_pi_build(prefix_len, flags, valid_ltime, pref_ltime, \
                            prefix, more)   (NULL)

/**
 * @brief   Builds the MTU option.
 *
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-4.6.4">
 *          RFC 4861, section 4.6.4
 *      </a>
 *
 * @note    Since this option MUST only be used with router advertisements and
 *          sending of router advertisements is not allowed for non-routing
 *          hosts, this implementation always returns NULL.
 *
 * @param[in] mtu           The recommended MTU for the link.
 * @param[in] more          More options in the packet. NULL, if there are none.
 *
 * @return  The packet snip list of options, on success
 * @return  NULL, if packet buffer is full
 */
#define ng_ndp_opt_mtu_build(mtu, more) (NULL)
#endif

#ifdef __cplusplus
}
#endif

#endif /* NG_NDP_H_ */
/**
 * @}
 */
