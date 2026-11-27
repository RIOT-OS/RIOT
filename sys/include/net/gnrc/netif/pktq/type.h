/*
 * SPDX-FileCopyrightText: 2019-2020 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @addtogroup  net_gnrc_netif_pktq
 * @brief
 * @{
 *
 * @file
 * @brief   @ref net_gnrc_netif_pktq type definitions
 *
 * Contained in its own file, so the type can be included in
 * @ref gnrc_netif_t while the functions in net/gnrc/netif/pktq.h can use
 * @ref gnrc_netif_t as operating type.
 *
 * @author  Martine S. Lenders <m.lenders@fu-berlin.de>
 */

#include "net/gnrc/pktqueue.h"
#include "xtimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   A packet queue for @ref net_gnrc_netif with a de-queue timer
 */
typedef struct {
    gnrc_pktqueue_t *queue;     /**< the actual packet queue class */
    const gnrc_pktsnip_t *busy_pkt; /**< packet to which
                                     *   gnrc_netif_pktq_t::busy_retries
                                     *   refers */
    uint8_t busy_retries;       /**< retries of gnrc_netif_pktq_t::busy_pkt
                                 *   after its transmission was confirmed
                                 *   with -EBUSY */
#if CONFIG_GNRC_NETIF_PKTQ_TIMER_US >= 0
    msg_t dequeue_msg;          /**< message for gnrc_netif_pktq_t::dequeue_timer to send */
    xtimer_t dequeue_timer;     /**< timer to schedule next sending of
                                 *   queued packets */
#endif
} gnrc_netif_pktq_t;

#ifdef __cplusplus
}
#endif

/** @} */
