/*
 * SPDX-FileCopyrightText: 2019 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    net_gnrc_sixlowpan_frag_hint    Fragment size hint
 * @ingroup     net_gnrc_sixlowpan_frag
 * @brief       Provides a hint for smaller fragment sizes than the link-layer
 *              PDU for the next fragment to sent
 * @{
 *
 * @file
 * @brief   Definitions to provide a hint on the final fragment size
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   A fragment size hint
 */
typedef struct {
    /**
     * @brief   Provides a hint of the size for the next fragment to send
     *
     * Set to 0 for fitting to the maximum fragment size of the interface the
     * datagram is sent over.
     *
     * @see gnrc_netif_6lo_t::max_frag_size
     */
    uint16_t fragsz;
    /**
     * @brief   The size of the data bound by gnrc_sixlowpan_frag_hint_t::fragsz
     *          uncompressed
     *
     * This is only evaluated when gnrc_sixlowpan_frag_hint_t::fragsz is greater
     * than 0.
     * Required to calculate the proper offset for the next fragment.
     */
    uint16_t fragsz_uncomp;
} gnrc_sixlowpan_frag_hint_t;

#ifdef __cplusplus
}
#endif

/** @} */
