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
#include "msg.h"
#include "net/gnrc/pkt.h"
#include "net/gnrc/netif/hdr.h"
#ifdef MODULE_GNRC_SIXLOWPAN_FRAG_HINT
#include "net/gnrc/sixlowpan/frag/hint.h"
#endif  /* MODULE_GNRC_SIXLOWPAN_FRAG_HINT */
#include "net/gnrc/sixlowpan/internal.h"
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
 * @brief   Definition of 6LoWPAN fragmentation type.
 */
typedef struct {
    gnrc_pktsnip_t *pkt;    /**< Pointer to the IPv6 packet to be fragmented */
    uint16_t datagram_size; /**< Length of just the (uncompressed) IPv6 packet to be fragmented */
    uint16_t tag;           /**< Tag used for the fragment */
    uint16_t offset;        /**< Offset of the Nth fragment from the beginning of the
                             *   payload datagram */
#ifdef MODULE_GNRC_SIXLOWPAN_FRAG_HINT
    /**
     * @brief   Hint for the size (smaller than link-layer PDU) for the next
     *          fragment to sent
     */
    gnrc_sixlowpan_frag_hint_t hint;
#endif /* MODULE_GNRC_SIXLOWPAN_FRAG_HINT */
} gnrc_sixlowpan_msg_frag_t;

#if defined(MODULE_GNRC_SIXLOWPAN_FRAG_STATS) || DOXYGEN
/**
 * @brief   Statistics on fragmentation and reassembly
 *
 * @note    Only available with the `gnrc_sixlowpan_frag_stats` module
 */
typedef struct {
    unsigned rbuf_full;     /**< counts the number of events where the
                             *   reassembly buffer is full */
    unsigned frag_full;     /**< counts the number of events that there where
                             *   no @ref gnrc_sixlowpan_msg_frag_t available */
#if defined(MODULE_GNRC_SIXLOWPAN_FRAG_VRB) || DOXYGEN
    unsigned vrb_full;      /**< counts the number of events where the virtual
                             *   reassembly buffer is full */
#endif
} gnrc_sixlowpan_frag_stats_t;

/**
 * @brief   Get the current statistics on fragmentation and reassembly
 *
 * @return  The current statistics on fragmentation and reassembly
 */
gnrc_sixlowpan_frag_stats_t *gnrc_sixlowpan_frag_stats_get(void);
#endif

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
 * @brief   Generate a new datagram tag for sending
 *
 * @return  A new datagram tag.
 */
uint16_t gnrc_sixlowpan_frag_next_tag(void);

/**
 * @brief   Sends a message to pass a further fragment down the network stack
 *
 * @see GNRC_SIXLOWPAN_MSG_FRAG_SND
 *
 * @param[in] fragment_msg  A @ref gnrc_sixlowpan_msg_frag_t object
 *
 * @return  true, when the message was sent
 * @return  false when sending the message failed.
 */
static inline bool gnrc_sixlowpan_frag_send_msg(gnrc_sixlowpan_msg_frag_t *fragment_msg)
{
    msg_t msg;

    msg.content.ptr = fragment_msg;
    msg.type = GNRC_SIXLOWPAN_MSG_FRAG_SND;
#ifdef TEST_SUITES
    return (msg_try_send(&msg, gnrc_sixlowpan_get_pid()) > 0);
#else
    return (msg_send_to_self(&msg) != 0);
#endif
}

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_SIXLOWPAN_FRAG_H */
/** @} */
