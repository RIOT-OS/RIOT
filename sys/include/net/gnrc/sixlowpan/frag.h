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
#ifndef GNRC_SIXLOWPAN_FRAG_H_
#define GNRC_SIXLOWPAN_FRAG_H_

#include <inttypes.h>
#include <stdbool.h>

#include "byteorder.h"
#include "kernel_types.h"
#include "net/gnrc/pkt.h"
#include "net/sixlowpan.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Message type for passing one 6LoWPAN fragment down the network stack
 */
#define GNRC_SIXLOWPAN_MSG_FRAG_SND    (0x0225)

/**
 * @brief   Definition of 6LoWPAN fragmentation type.
 */
typedef struct {
    kernel_pid_t pid;       /**< PID of the interface */
    gnrc_pktsnip_t *pkt;    /**< Pointer to the IPv6 packet to be fragmented */
    size_t datagram_size;   /**< Length of just the IPv6 packet to be fragmented */
    uint16_t offset;        /**< Offset of the Nth fragment from the beginning of the
                             *   payload datagram */
} gnrc_sixlowpan_msg_frag_t;

/**
 * @brief   Sends a packet fragmented.
 *
 * @param[in] fragment_msg    Message containing status of the 6LoWPAN
 *                            fragmentation progress
 */
void gnrc_sixlowpan_frag_send(gnrc_sixlowpan_msg_frag_t *fragment_msg);

/**
 * @brief   Handles a packet containing a fragment header.
 *
 * @param[in] pkt   The packet to handle.
 */
void gnrc_sixlowpan_frag_handle_pkt(gnrc_pktsnip_t *pkt);

#ifdef __cplusplus
}
#endif

#endif /* GNRC_SIXLOWPAN_FRAG_H_ */
/** @} */
