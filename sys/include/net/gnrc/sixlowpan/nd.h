/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_sixlowpan_nd 6LoWPAN neighbor discovery
 * @ingroup     net_gnrc_sixlowpan
 * @brief       Neighbor Discovery Optimization for 6LoWPAN
 * @see         <a href="https://tools.ietf.org/html/rfc6775">
 *                  RFC 6775
 *              </a>
 * @{
 *
 * @file
 * @brief   General 6LoWPAN ND definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef GNRC_SIXLOWPAN_ND_H_
#define GNRC_SIXLOWPAN_ND_H_

#include <stdint.h>

#include "kernel_types.h"
#include "net/gnrc/ipv6/nc.h"
#include "net/gnrc/ipv6/netif.h"
#include "net/ipv6/addr.h"
#include "net/ndp.h"
#include "net/sixlowpan/nd.h"
#include "timex.h"

#include "net/gnrc/sixlowpan/nd/border_router.h"
#include "net/gnrc/sixlowpan/nd/router.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Message type for next multicast router solicitation.
 */
#define GNRC_SIXLOWPAN_ND_MSG_MC_RTR_SOL    (0x0220)

/**
 * @brief   Message type for next unicast router solicitation.
 */
#define GNRC_SIXLOWPAN_ND_MSG_UC_RTR_SOL    (0x0221)

/**
 * @brief   Message type for removing 6LoWPAN contexts.
 */
#define GNRC_SIXLOWPAN_ND_MSG_DELETE_CTX    (0x0222)

/**
 * @brief   Message type for authoritative border router timeout
 */
#define GNRC_SIXLOWPAN_ND_MSG_ABR_TIMEOUT   (0x0223)

/**
 * @brief   Message type for address registration timeout
 */
#define GNRC_SIXLOWPAN_ND_MSG_AR_TIMEOUT    (0x0224)

#ifndef GNRC_SIXLOWPAN_ND_AR_LTIME
/**
 * @brief   Registration lifetime in minutes for the address registration option
 *
 * This value should be adapted to the devices power-lifecycle so that it is greater than the
 * time the device spends sleeping.
 *
 * @see <a href="https://tools.ietf.org/html/rfc6775#section-5.8.1">
 *          RFC 6775, section 5.8.1
 *      </a>
 */
#define GNRC_SIXLOWPAN_ND_AR_LTIME          (15U)
#endif

/**
 * @name    Border router constants
 * @{
 * @see     <a href="https://tools.ietf.org/html/rfc6775#section-9">
 *              RFC 6775, section 9
 *          </a>
 */
#define GNRC_SIXLOWPAN_ND_RTR_MIN_CTX_DELAY (300U)  /**< minimum delay between context change and
                                                     *   stop of C=0 dissimination in seconds */
/** @} */
/**
 * @name    Host constants
 * @{
 * @see     <a href="https://tools.ietf.org/html/rfc6775#section-9">
 *              RFC 6775, section 9
 *          </a>
 */
#define GNRC_SIXLOWPAN_ND_RTR_SOL_INT       (10U)   /**< replacement value (in seconds) for
                                                     *   @ref GNRC_NDP_MAX_RTR_SOL_INT */
#define GNRC_SIXLOWPAN_ND_MAX_RTR_SOL_INT   (60U)   /**< retransmission increment for exponential
                                                     *   backoff of subsequent RS */
/** @} */
/**
 * @name    Router constants
 * @{
 * @see     <a href="https://tools.ietf.org/html/rfc6775#section-9">
 *              RFC 6775, section 9
 *          </a>
 */
#define GNRC_SIXLOWPAN_ND_MIN_RTR_ADV_DELAY (10U)   /**< replacement value (in seconds) for
                                                     *   @ref GNRC_NDP_MIN_RTR_ADV_DELAY */
/**
 * @brief   replacement value (in microseconds) for @ref GNRC_NDP_MAX_RTR_ADV_DELAY
 */
#define GNRC_SIXLOWPAN_ND_MAX_RTR_ADV_DELAY (2U * SEC_IN_USEC)
/**
 * @brief   Lifetime of a tentative address entry in seconds
 */
#define GNRC_SIXLOWPAN_ND_TENTATIVE_NCE_LIFETIME    (20U)
/**
 * @brief   6LoWPAN Multihop Hoplimit
 */
#define GNRC_SIXLOWPAN_ND_MULTIHOP_HOPLIMIT (64U)
/** @} */

/**
 * @brief   Initializes 6LoWPAN neighbor discovery for the interface.
 * @pre     @p iface->flags & GNRC_IPV6_NETIF_FLAGS_SIXLOWPAN
 * @param[in] iface An IPv6 interface.
 */
void gnrc_sixlowpan_nd_init(gnrc_ipv6_netif_t *iface);

/**
 * @brief   Multicasts a router solicitation over @p iface
 * @pre     @p iface->flags & GNRC_IPV6_NETIF_FLAGS_SIXLOWPAN
 * @param[in] iface An IPv6 interface.
 */
void gnrc_sixlowpan_nd_mc_rtr_sol(gnrc_ipv6_netif_t *iface);

/**
 * @brief   Unicasts a router solicitation to the neighbor represented by @p nce
 * @pre     @p nce->iface is an IPv6 interface and @ref GNRC_IPV6_NETIF_FLAGS_SIXLOWPAN is set
 *          in its flags.
 * @param[in] nce   The neighbor to send the router solicitation to.
 */
void gnrc_sixlowpan_nd_uc_rtr_sol(gnrc_ipv6_nc_t *nce);

/**
 * @brief   Get link-layer address and interface for next hop to destination
 *          IPv6 address.
 *
 * @param[out] l2addr           The link-layer for the next hop to @p dst.
 * @param[out] l2addr_len       Length of @p l2addr.
 * @param[in] iface             The interface to search the next hop on.
 *                              May be @ref KERNEL_PID_UNDEF if not specified.
 * @param[in] dst               An IPv6 address to search the next hop for.
 *
 * @return  The PID of the interface, on success.
 * @return  -EHOSTUNREACH, if @p dst is not reachable.
 * @return  -ENOBUFS, if @p l2addr_len was smaller than the resulting @p l2addr
 *          would be long.
 */
kernel_pid_t gnrc_sixlowpan_nd_next_hop_l2addr(uint8_t *l2addr, uint8_t *l2addr_len,
                                               kernel_pid_t iface, ipv6_addr_t *dst);

/**
 * @brief   Reschedules the next router advertisement for a neighboring router.
 *
 * @pre nce != NULL && sec_delay != 0U
 *
 * @param[in] nce       Neighbor cache entry representing the neighboring router.
 * @param[in] sec_delay The delay for the next router solicitation in seconds.
 */
void gnrc_sixlowpan_nd_rtr_sol_reschedule(gnrc_ipv6_nc_t *nce, uint32_t sec_delay);

/**
 * @brief   Builds the address registration option.
 *
 * @param[in] status    Status for the ARO.
 * @param[in] ltime     Registration lifetime for the ARO.
 * @param[in] eui64     The EUI-64 for the ARO
 * @param[in] next          More options in the packet. NULL, if there are none.
 *
 * @return  The pkt snip list of options, on success
 * @return  NULL, if packet buffer is full
 */
gnrc_pktsnip_t *gnrc_sixlowpan_nd_opt_ar_build(uint8_t status, uint16_t ltime, eui64_t *eui64,
                                               gnrc_pktsnip_t *next);

/**
 * @brief   Handles address registration option.
 *
 * @param[in] iface         The interface the ARO was received on.
 * @param[in] ipv6          The IPv6 header the ARO was received in.
 * @param[in] icmpv6_type   Message type of the ICMPv6 message that contained.
 *                          this message.
 * @param[in] ar_opt        The address registration option.
 * @param[in] sl2a          The link-layer source address contained in SL2A accompanying this
 *                          option. May be NULL for icmpv6_type == ICMPV6_NBR_ADV.
 * @param[in] sl2a_len      Length of @p sl2a. May be 0 if sl2a == NULL.
 *
 * @return  Status for the ARO in the replying NA (always 0 if icmpv6_type == ICMPV6_NBR_ADV).
 */
uint8_t gnrc_sixlowpan_nd_opt_ar_handle(kernel_pid_t iface, ipv6_hdr_t *ipv6,
                                        uint8_t icmpv6_type, sixlowpan_nd_opt_ar_t *ar_opt,
                                        uint8_t *sl2a, size_t sl2a_len);

/**
 * @brief   Handles 6LoWPAN context option.
 *
 * @param[in] icmpv6_type   Message type of the ICMPv6 message that contained.
 *                          this message.
 * @param[in] ctx_opt       The 6LoWPAN context option.
 *
 * @return  true, when 6LoWPAN context option was correct.
 * @return  false, when it was incorrect.
 */
bool gnrc_sixlowpan_nd_opt_6ctx_handle(uint8_t icmpv6_type, sixlowpan_nd_opt_6ctx_t *ctx_opt);

/**
 * @brief   Handles registration calls after node-wakeup.
 *
 * @see     <a href="https://tools.ietf.org/html/rfc6775#section-5.8.2">
 *              RFC 6776, section 5.8.2
 *          </a>
 */
void gnrc_sixlowpan_nd_wakeup(void);

#ifdef MODULE_GNRC_SIXLOWPAN_ND_ROUTER
/**
 * @brief   Handles authoritative border router option.
 *
 * @param[in] iface         Interface the source link-layer option was received
 *                          on.
 * @param[in] rtr_adv       The router advertisement containing the ABRO.
 * @param[in] icmpv6_size   The size of the @p rtr_adv.
 * @param[in] abr_opt       The ABRO.
 *
 * @note    Erroneous ABROs are always ignored silently.
 */
void gnrc_sixlowpan_nd_opt_abr_handle(kernel_pid_t iface, ndp_rtr_adv_t *rtr_adv, int icmpv6_size,
                                      sixlowpan_nd_opt_abr_t *abr_opt);

/**
 * @brief   Builds the 6LoWPAN context option.
 *
 * @param[in] prefix_len    The length of the context's prefix.
 * @param[in] flags         Flags + CID for the context.
 * @param[in] ltime         Lifetime of the context.
 * @param[in] prefix        The context's prefix
 * @param[in] next      More options in the packet. NULL, if there are none.
 *
 * @return  The pkt snip list of options, on success
 * @return  NULL, if packet buffer is full or on error
 */
gnrc_pktsnip_t *gnrc_sixlowpan_nd_opt_6ctx_build(uint8_t prefix_len, uint8_t flags, uint16_t ltime,
                                                 ipv6_addr_t *prefix, gnrc_pktsnip_t *next);

/**
 * @brief   Builds the authoritative border router option.
 *
 * @param[in] version   Version of the border router information.
 * @param[in] ltime     Registration lifetime for the border router.
 * @param[in] braddr    The IPv6 address of the border router.
 * @param[in] next      More options in the packet. NULL, if there are none.
 *
 * @return  The pkt snip list of options, on success
 * @return  NULL, if packet buffer is full or on error
 */
gnrc_pktsnip_t *gnrc_sixlowpan_nd_opt_abr_build(uint32_t version, uint16_t ltime,
                                                ipv6_addr_t *braddr, gnrc_pktsnip_t *next);
#else
#define gnrc_sixlowpan_nd_opt_abr_handle(iface, rtr_adv, icmpv6_size, abr_opt)
#define gnrc_sixlowpan_nd_opt_6ctx_build(prefix_len, flags, ltime, prefix, next)        (NULL)
#define gnrc_sixlowpan_nd_opt_abr_build(version, ltime, braddr, next)                   (NULL)
#endif

#ifdef __cplusplus
}
#endif

#endif /* GNRC_SIXLOWPAN_ND_H_ */
/** @} */
