/*
 * Copyright (C) 2019-20 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
