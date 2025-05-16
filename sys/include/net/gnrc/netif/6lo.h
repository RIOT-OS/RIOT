/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup net_gnrc_netif
 * @{
 *
 * @file
 * @brief   6LoWPAN definitions for @ref net_gnrc_netif
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Local 6LoWPAN capability flags
 * @anchor  net_gnrc_netif_6lo_local_flags
 * @see     gnrc_netif_6lo_t::local_flags
 *
 * Like the capability flags in the [6LoWPAN Capability Indication Option
 * (6CIO)](https://tools.ietf.org/html/rfc7400#section-3.3) are less about
 * hardware capabilities than about the implementation status within the
 * network. For the flags in this group it is currently undefined how to
 * exchange the capabilities between nodes, but they might be added to the 6CIO
 * at a later point. Once the 6CIO is implemented in GNRC and the flag is
 * supported by it, the corresponding flag in these local flags can be removed.
 * @{
 */
/**
 * @brief   Selective Fragment Recovery enabled
 * @see     [RFC 8931](https://tools.ietf.org/html/rfc8931)
 */
#define GNRC_NETIF_6LO_LOCAL_FLAGS_SFR  (0x01)
/** @} */

/**
 * @brief   6Lo component of @ref gnrc_netif_t
 */
typedef struct {
    /**
     * @brief   Maximum fragment size for 6Lo fragmentation.
     *
     * @note    Only available with module
     *          @ref net_gnrc_sixlowpan_frag "gnrc_sixlowpan_frag".
     */
    uint16_t max_frag_size;
    /**
     * @brief   6LoWPAN capability flags beyond the ones advertised in
     *          [6LoWPAN Capability Indication Option
     *          (6CIO)](https://tools.ietf.org/html/rfc7400#section-3.3)
     *
     * @see     [Local 6LoWPAN capability flags](@ref
     *          net_gnrc_netif_6lo_local_flags)
     */
    uint8_t local_flags;
} gnrc_netif_6lo_t;

#ifdef __cplusplus
}
#endif

/** @} */
