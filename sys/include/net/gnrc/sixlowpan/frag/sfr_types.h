/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  net_gnrc_sixlowpan_frag_sfr
 * @{
 *
 * @file
 * @brief   6LoWPAN selective fragment recovery type definitions for GNRC
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef NET_GNRC_SIXLOWPAN_FRAG_SFR_TYPES_H
#define NET_GNRC_SIXLOWPAN_FRAG_SFR_TYPES_H

#include <stdint.h>

#include "bitfield.h"
#include "clist.h"
#include "evtimer_msg.h"
#include "msg.h"
#include "xtimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Bitmap type to simplify comparisons
 */
typedef union {
    uint32_t u32;                       /**< numerical version of bitmap */
    BITFIELD(bf, 32U);                  /**< bitfield version of bitmap */
} gnrc_sixlowpan_frag_sfr_bitmap_t;

/**
 * @brief   Extension for @ref net_gnrc_sixlowpan_frag_fb for selective
 *          fragment recovery
 */
typedef struct gnrc_sixlowpan_frag_sfr_fb {
    /**
     * @brief   Acknowledgment request timeout event
     */
    evtimer_msg_event_t arq_timeout_event;
    uint32_t arq_timeout;       /**< Time in microseconds the sender should
                                 *   wait for an RFRAG Acknowledgment */
    uint8_t cur_seq;            /**< Sequence number for next fragment */
    uint8_t frags_sent;         /**< Number of fragments sent */
    uint8_t window_size;        /**< Current window size in number of
                                 *   fragments */
    uint8_t retrans;            /**< Datagram retransmissions */
    clist_node_t window;        /**< Sent fragments of the current window */
} gnrc_sixlowpan_frag_sfr_fb_t;

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_SIXLOWPAN_FRAG_SFR_TYPES_H */
/** @} */
