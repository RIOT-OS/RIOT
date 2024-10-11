/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_sixlowpan_frag_stats Fragmentation and reassembly statistics
 * @ingroup     net_gnrc_sixlowpan_frag
 * @brief       Counter for certain 6LoWPAN fragmentation and reassembly events.
 * @{
 *
 * @file
 * @brief   Fragmentation and reassembly statistics definitions
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef NET_GNRC_SIXLOWPAN_FRAG_STATS_H
#define NET_GNRC_SIXLOWPAN_FRAG_STATS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Statistics on fragmentation and reassembly
 *
 * @note    Only available with the `gnrc_sixlowpan_frag_stats` module
 */
typedef struct {
    unsigned rbuf_full;     /**< counts the number of events where the
                             *   reassembly buffer is full */
    unsigned frag_full;     /**< counts the number of events that there where
                             *   no @ref gnrc_sixlowpan_frag_fb_t available */
    unsigned datagrams;     /**< reassembled datagrams */
    unsigned fragments;     /**< total fragments of reassembled fragments */
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

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NET_GNRC_SIXLOWPAN_FRAG_STATS_H */
