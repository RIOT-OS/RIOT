/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 * Copyright (C) 2015 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_sixlowpan_frag   6LoWPAN Fragmentation
 * @ingroup     net_gnrc_sixlowpan
 * @brief       6LoWPAN Fragmentation headers and functionality
 * @see <a href="https://tools.ietf.org/html/rfc4944#section-5.3">
 *          RFC 4944, section 5.3
 *      </a>
 * @{
 *
 * @file
 * @brief   6LoWPAN Fragmentation definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author  Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 */
#ifndef NET_GNRC_SIXLOWPAN_FRAG_H
#define NET_GNRC_SIXLOWPAN_FRAG_H

#include <inttypes.h>
#include <stdbool.h>

#include "byteorder.h"
#include "kernel_types.h"
#include "net/gnrc/pkt.h"
#include "net/gnrc/netif/hdr.h"
#include "net/ieee802154.h"
#include "net/sixlowpan.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Message types
 * @{
 */
/**
 * @brief   Message type for passing one 6LoWPAN fragment down the network stack
 */
#define GNRC_SIXLOWPAN_MSG_FRAG_SND         (0x0225)

/**
 * @brief   Message type for triggering garbage collection reassembly buffer
 */
#define GNRC_SIXLOWPAN_MSG_FRAG_GC_RBUF     (0x0226)
/** @} */

/**
 * @brief   An entry in the 6LoWPAN reassembly buffer.
 *
 * A recipient of a fragment SHALL use
 *
 * 1. the source address,
 * 2. the destination address,
 * 3. the datagram size (gnrc_pktsnip_t::size of rbuf_t::pkt), and
 * 4. the datagram tag
 *
 * to identify all fragments that belong to the given datagram.
 *
 * @see [RFC 4944, section 5.3](https://tools.ietf.org/html/rfc4944#section-5.3)
 */
typedef struct {
    /**
     * @brief   The reassembled packet in the packet buffer
     */
    gnrc_pktsnip_t *pkt;
    uint8_t src[IEEE802154_LONG_ADDRESS_LEN];   /**< source address */
    uint8_t dst[IEEE802154_LONG_ADDRESS_LEN];   /**< destination address */
    uint8_t src_len;                            /**< length of gnrc_sixlowpan_rbuf_t::src */
    uint8_t dst_len;                            /**< length of gnrc_sixlowpan_rbuf_t::dst */
    uint16_t tag;                               /**< the datagram's tag */
    /**
     * @brief   The number of bytes currently received of the complete datagram
     */
    uint16_t current_size;
} gnrc_sixlowpan_rbuf_t;

/**
 * @brief   Definition of 6LoWPAN fragmentation type.
 */
typedef struct {
    gnrc_pktsnip_t *pkt;    /**< Pointer to the IPv6 packet to be fragmented */
    size_t datagram_size;   /**< Length of just the (uncompressed) IPv6 packet to be fragmented */
    uint16_t offset;        /**< Offset of the Nth fragment from the beginning of the
                             *   payload datagram */
    kernel_pid_t pid;       /**< PID of the interface */
} gnrc_sixlowpan_msg_frag_t;

/**
 * @brief   Allocates a @ref gnrc_sixlowpan_msg_frag_t object
 *
 * @return  A @ref gnrc_sixlowpan_msg_frag_t if available
 * @return  NULL, otherwise
 */
gnrc_sixlowpan_msg_frag_t *gnrc_sixlowpan_msg_frag_get(void);

/**
 * @brief   Sends a packet fragmented
 *
 * @pre `ctx != NULL`
 * @pre gnrc_sixlowpan_msg_frag_t::pkt of @p ctx is equal to @p pkt or
 *      `pkt == NULL`.
 *
 * @param[in] pkt       A packet. May be NULL.
 * @param[in] ctx       Message containing status of the 6LoWPAN fragmentation
 *                      progress. Expected to be of type
 *                      @ref gnrc_sixlowpan_msg_frag_t, with
 *                      gnrc_sixlowpan_msg_frag_t set to @p pkt. Must not be
 *                      NULL.
 * @param[in] page      Current 6Lo dispatch parsing page.
 */
void gnrc_sixlowpan_frag_send(gnrc_pktsnip_t *pkt, void *ctx, unsigned page);

/**
 * @brief   Handles a packet containing a fragment header
 *
 * @param[in] pkt       The packet to handle
 * @param[in] ctx       Context for the packet. May be NULL.
 * @param[in] page      Current 6Lo dispatch parsing page.
 */
void gnrc_sixlowpan_frag_recv(gnrc_pktsnip_t *pkt, void *ctx, unsigned page);

/**
 * @brief   Garbage collect reassembly buffer.
 */
void gnrc_sixlowpan_frag_rbuf_gc(void);

#if defined(MODULE_GNRC_SIXLOWPAN_FRAG) || defined(DOXYGEN)
/**
 * @brief   Removes an entry from the reassembly buffer
 *
 * @pre `rbuf != NULL`
 *
 * @param[in] rbuf  A reassembly buffer entry. Must not be NULL.
 */
void gnrc_sixlowpan_frag_rbuf_remove(gnrc_sixlowpan_rbuf_t *rbuf);

/**
 * @brief   Checks if a reassembly buffer entry is complete and dispatches it
 *          to the next layer if that is the case
 *
 * @pre `rbuf != NULL`
 * @pre `netif != NULL`
 *
 * @param[in] rbuf  A reassembly buffer entry. Must not be NULL.
 * @param[in] netif Original @ref gnrc_netif_hdr_t of the last received frame.
 *                  Used to construct the @ref gnrc_netif_hdr_t of the completed
 *                  datagram. Must not be NULL.
 */
void gnrc_sixlowpan_frag_rbuf_dispatch_when_complete(gnrc_sixlowpan_rbuf_t *rbuf,
                                                     gnrc_netif_hdr_t *netif);
#else
/* NOPs to be used with gnrc_sixlowpan_iphc if gnrc_sixlowpan_frag is not
 * compiled in */
#define gnrc_sixlowpan_frag_rbuf_remove(rbuf)       (void)(rbuf)
#define gnrc_sixlowpan_frag_rbuf_dispatch_when_complete(rbuf, netif) \
    (void)(rbuf); (void)(netif)
#endif

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_SIXLOWPAN_FRAG_H */
/** @} */
