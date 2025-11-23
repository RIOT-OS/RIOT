/*
 * SPDX-FileCopyrightText: 2019 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    net_gnrc_netif_dedup    Link-layer Broadcast deduplication
 * @ingroup     net_gnrc_netif
 * @brief       Deduplicates broadcast link-layer packets best-effort style
 *
 * To activate, use `USEMODULE += gnrc_netif_dedup` in your applications
 * Makefile. Also make sure the link-layer you use supports the module.
 * Currently supported are
 *
 * - IEEE 802.15.4
 *
 * @{
 *
 * @file
 * @brief
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <stdint.h>

#include "net/gnrc/netif/conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Structure to store information on the last broadcast packet received
 */
typedef struct {
    uint8_t src[GNRC_NETIF_L2ADDR_MAXLEN];  /**< link-layer source address */
    uint16_t seq;                           /**< link-layer sequence number */
    uint8_t src_len;                        /**< length of gnrc_netif_dedup_t:src */
} gnrc_netif_dedup_t;

#ifdef __cplusplus
}
#endif

/** @} */
