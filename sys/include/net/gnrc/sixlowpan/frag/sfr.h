/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_sixlowpan_frag_sfr  6LoWPAN selective fragment recovery
 * @ingroup     net_gnrc_sixlowpan
 * @brief       6LoWPAN selective fragment recovery implementation for GNRC
 *
 * 6LoWPAN selective fragment recovery is an alternative fragmentation
 * specification to [classic 6LoWPAN fragmentation](@ref
 * net_gnrc_sixlowpan_frag). It can be run in parallel to classic fragmentation,
 * but is incompatible with its message formats.
 *
 * How nodes can exchange that they are able to communicate using selective
 * fragment recovery is currently not specified, so this feature should only be
 * used if the operator of a network can ensure that all 6LoWPAN nodes within
 * that network can communicate using selective fragment recovery.
 *
 * @see         [RFC 8931](https://tools.ietf.org/html/rfc8931)
 * @{
 *
 * @file
 * @brief   6LoWPAN selective fragment recovery definitions for GNRC
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef NET_GNRC_SIXLOWPAN_FRAG_SFR_H
#define NET_GNRC_SIXLOWPAN_FRAG_SFR_H

#include "assert.h"
#include "bitfield.h"
#include "net/gnrc/pkt.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/sixlowpan/config.h"
#include "net/gnrc/sixlowpan/frag/fb.h"
#include "net/gnrc/sixlowpan/frag/vrb.h"
#include "net/gnrc/sixlowpan/frag/sfr_types.h"
#include "net/sixlowpan/sfr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Message type to signal an acknowledgement request timeout.
 */
#define GNRC_SIXLOWPAN_FRAG_SFR_ARQ_TIMEOUT_MSG     (0x0227)

/**
 * @brief   Message type to signal the sending of the next frame.
 */
#define GNRC_SIXLOWPAN_FRAG_SFR_INTER_FRAG_GAP_MSG  (0x0228)

/**
 * @brief   Stats on selective fragment recovery
 */
typedef struct {
    uint32_t datagram_resends;  /**< datagrams resent */
    struct {
        uint32_t usual;         /**< non-abort fragments sent */
        uint32_t aborts;        /**< abort pseudo-fragments sent */
        uint32_t forwarded;     /**< forwarded fragments */
    } fragments_sent;           /**< RFRAG packets sent */
    struct {
        uint32_t by_nack;       /**< fragments resent due to a 0 in ACK's bitmap */
        uint32_t by_timeout;    /**< fragments resent due to an ARQ timeout */
    } fragment_resends;         /**< fragments resent */
    struct {
        uint32_t full;          /**< full RFRAGs ACKs sent */
        uint32_t partly;        /**< partly ACKing RFRAGs sent */
        uint32_t aborts;        /**< abort RFRAG ACKs sent */
        uint32_t forwarded;     /**< forwarded ACKs */
    } acks;                     /**< ACKs stats */
} gnrc_sixlowpan_frag_sfr_stats_t;

/**
 * @brief   Initialize selective fragment recovery
 */
void gnrc_sixlowpan_frag_sfr_init(void);

/**
 * @brief   Initialize a network interface for selective fragment recovery
 *
 * @note    This is a NOP without module `gnrc_sixlowpan_frag_sfr`
 *
 * @param[in] netif A network interface
 */
static inline void gnrc_sixlowpan_frag_sfr_init_iface(gnrc_netif_t *netif)
{
    if (IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR) &&
        gnrc_netif_is_6lo(netif)) {
#if IS_USED(MODULE_GNRC_NETIF_6LO)
        netif->sixlo.local_flags |= GNRC_NETIF_6LO_LOCAL_FLAGS_SFR;
        netif->sixlo.max_frag_size =
              (netif->sixlo.max_frag_size > CONFIG_GNRC_SIXLOWPAN_SFR_OPT_FRAG_SIZE)
            ? CONFIG_GNRC_SIXLOWPAN_SFR_OPT_FRAG_SIZE
            : netif->sixlo.max_frag_size;
        assert(netif->sixlo.max_frag_size >= CONFIG_GNRC_SIXLOWPAN_SFR_MIN_FRAG_SIZE);
#endif
    }
}

/**
 * @brief   Checks if a network interface is configured for selective fragment
 *          recovery
 *
 * @param[in] netif A network interface.
 *
 * @return  true, if @p netif supports selective fragment recovery and has it
 *          enabled.
 * @return  false, if @p netif does not support selective fragment recovery or
 *          does not have it enabled.
 */
static inline bool gnrc_sixlowpan_frag_sfr_netif(gnrc_netif_t *netif)
{
#if IS_USED(MODULE_GNRC_NETIF_6LO)
    return IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR) &&
        gnrc_netif_is_6lo(netif) &&
        (netif->sixlo.local_flags & GNRC_NETIF_6LO_LOCAL_FLAGS_SFR);
#else
    (void)netif;
    return false;
#endif
}

/**
 * @brief   Sends a packet via selective fragment recovery
 *
 * @pre `ctx != NULL`
 * @pre gnrc_sixlowpan_frag_fb_t::pkt of @p ctx is equal to @p pkt or
 *      `pkt == NULL`.
 *
 * @param[in] pkt   A packet. May be NULL.
 * @param[in] ctx   Fragmentation buffer entry of. Expected to be of type
 *                  @ref gnrc_sixlowpan_frag_fb_t, with gnrc_sixlowpan_frag_fb_t
 *                  set to @p pkt. Must not be NULL.
 * @param[in] page  Current 6Lo dispatch parsing page.
 */
void gnrc_sixlowpan_frag_sfr_send(gnrc_pktsnip_t *pkt, void *ctx,
                                  unsigned page);

/**
 * @brief   Handles a packet containing a selective fragment recovery header
 *
 * @param[in] pkt       The packet to handle.
 * @param[in] ctx       Context for the packet. May be NULL.
 * @param[in] page      Current 6Lo dispatch parsing page.
 */
void gnrc_sixlowpan_frag_sfr_recv(gnrc_pktsnip_t *pkt, void *ctx, unsigned page);

/**
 * @brief   Forward a fragment via selective fragment recovery
 *
 * @param[in] pkt       The fragment to forward (without RFRAG header).
 *                      Is consumed by this function.
 * @param[in] rfrag     The originally received RFRAG header.
 * @param[in] vrbe      Virtual reassembly buffer containing the forwarding
 *                      information.
 * @param[in] page      Current 6Lo dispatch parsing page.
 *
 * @return  0, on success.
 * @return  -ENOMEM, when packet buffer is too full to prepare packet for
 *          forwarding. @p pkt is released in that case.
 */
int gnrc_sixlowpan_frag_sfr_forward(gnrc_pktsnip_t *pkt,
                                    sixlowpan_sfr_rfrag_t *rfrag,
                                    gnrc_sixlowpan_frag_vrb_t *vrbe,
                                    unsigned page);

/**
 * @brief   Handles an Acknowledgment request timeout
 *
 * @param[in] fbuf  The fragmentation buffer representing the datagram for which
 *                  fragments the Acknowledgment request timed out.
 */
void gnrc_sixlowpan_frag_sfr_arq_timeout(gnrc_sixlowpan_frag_fb_t *fbuf);

/**
 * @brief   Handles inter frame gap
 */
void gnrc_sixlowpan_frag_sfr_inter_frame_gap(void);

#if IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR_STATS)
/**
 * @brief   Fetch current stats for selective fragment recovery
 *
 * @param[out] stats    The current stats. Must not be NULL.
 */
void gnrc_sixlowpan_frag_sfr_stats_get(gnrc_sixlowpan_frag_sfr_stats_t *stats);
#endif /* IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR_STATS) */

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_SIXLOWPAN_FRAG_SFR_H */
/** @} */
