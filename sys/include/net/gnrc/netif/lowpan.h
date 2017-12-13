/*
 * Copyright (C) 2017 HAW Hamburg
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup net_gnrc_netif
 * @{
 *
 * @file
 * @brief   LoWPAN definitions for @ref net_gnrc_netif
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 * @author  Cenk Gündoğan <mail-github@cgundogan.de>
 */
#ifndef NET_GNRC_NETIF_LOWPAN_H
#define NET_GNRC_NETIF_LOWPAN_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   LoWPAN component of @ref gnrc_netif_t
 */
typedef struct {
    /**
     * @brief   Maximum fragment size for LoWPAN fragmentation.
     *
     * @note    Only available with module
     *          @ref net_gnrc_lowpan_frag "gnrc_lowpan_frag".
     */
    uint8_t max_frag_size;
} gnrc_netif_lowpan_t;

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_NETIF_LOWPAN_H */
/** @} */
