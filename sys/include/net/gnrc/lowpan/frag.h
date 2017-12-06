/*
 * Copyright (C) 2015, 2017     Hamburg University of Applied Sciences
 * Copyright (C) 2015           Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_lowpan_frag   LoWPAN Fragmentation
 * @ingroup     net_gnrc_lowpan
 * @brief       LoWPAN Fragmentation headers and functionality
 * @see <a href="https://tools.ietf.org/html/rfc4944#section-5.3">
 *          RFC 4944, section 5.3
 *      </a>
 * @{
 *
 * @file
 * @brief   LoWPAN Fragmentation definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author  Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author  Cenk Gündoğan <mail-github@cgundogan.de>
 */
#ifndef NET_GNRC_LOWPAN_FRAG_H
#define NET_GNRC_LOWPAN_FRAG_H

#include <inttypes.h>
#include <stdbool.h>

#include "byteorder.h"
#include "kernel_types.h"
#include "net/gnrc/pkt.h"
#include "net/lowpan.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Message type for passing one LoWPAN fragment down the network stack
 */
#define GNRC_LOWPAN_MSG_FRAG_SND    (0x0225)

/**
 * @brief   Definition of LoWPAN fragmentation type.
 */
typedef struct {
    kernel_pid_t pid;       /**< PID of the interface */
    gnrc_pktsnip_t *pkt;    /**< Pointer to the packet to be fragmented */
    size_t datagram_size;   /**< Length of just the packet to be fragmented */
    uint16_t offset;        /**< Offset of the Nth fragment from the beginning of the
                             *   payload datagram */
} gnrc_lowpan_msg_frag_t;

/**
 * @brief   Sends a packet fragmented.
 *
 * @param[in] fragment_msg    Message containing status of the LoWPAN
 *                            fragmentation progress
 */
void gnrc_lowpan_frag_send(gnrc_lowpan_msg_frag_t *fragment_msg);

/**
 * @brief   Handles a packet containing a fragment header.
 *
 * @param[in] pkt   The packet to handle.
 */
void gnrc_lowpan_frag_handle_pkt(gnrc_pktsnip_t *pkt);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_LOWPAN_FRAG_H */
/** @} */
