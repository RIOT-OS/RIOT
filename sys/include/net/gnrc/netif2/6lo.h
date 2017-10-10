/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup net_gnrc_netif2
 * @{
 *
 * @file
 * @brief   6LoWPAN definitions for @ref net_gnrc_netif2
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef NET_GNRC_NETIF2_6LO_H
#define NET_GNRC_NETIF2_6LO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   6Lo component of @ref gnrc_netif2_t
 */
typedef struct {
    /**
     * @brief   Maximum fragment size for 6Lo fragmentation.
     *
     * @note    Only available with module
     *          @ref net_gnrc_sixlowpan_frag "gnrc_sixlowpan_frag".
     */
    uint8_t max_frag_size;
} gnrc_netif2_6lo_t;

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_NETIF2_6LO_H */
/** @} */
