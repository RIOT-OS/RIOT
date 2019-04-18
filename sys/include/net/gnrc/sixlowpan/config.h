/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_sixlowpan_config GNRC 6LoWPAN compile configurations
 * @ingroup     net_gnrc_sixlowpan
 * @ingroup     config
 * @brief
 * @{
 *
 * @file
 * @brief   Configuration macros for @ref net_gnrc_sixlowpan
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef NET_GNRC_SIXLOWPAN_CONFIG_H
#define NET_GNRC_SIXLOWPAN_CONFIG_H

#include "timex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default stack size to use for the 6LoWPAN thread.
 */
#ifndef GNRC_SIXLOWPAN_STACK_SIZE
#define GNRC_SIXLOWPAN_STACK_SIZE           (THREAD_STACKSIZE_DEFAULT)
#endif

/**
 * @brief   Default priority for the 6LoWPAN thread.
 */
#ifndef GNRC_SIXLOWPAN_PRIO
#define GNRC_SIXLOWPAN_PRIO                 (THREAD_PRIORITY_MAIN - 4)
#endif

/**
 * @brief   Default message queue size to use for the 6LoWPAN thread.
 */
#ifndef GNRC_SIXLOWPAN_MSG_QUEUE_SIZE
#define GNRC_SIXLOWPAN_MSG_QUEUE_SIZE       (8U)
#endif

/**
 * @brief   Size of the reassembly buffer
 *
 * @note    Only applicable with
 *          [gnrc_sixlowpan_frag](@ref net_gnrc_sixlowpan_frag) module
 */
#ifndef GNRC_SIXLOWPAN_FRAG_RBUF_SIZE
#define GNRC_SIXLOWPAN_FRAG_RBUF_SIZE       (4U)
#endif

/**
 * @brief   Timeout for reassembly buffer entries in microseconds
 *
 * @note    Only applicable with
 *          [gnrc_sixlowpan_frag](@ref net_gnrc_sixlowpan_frag) module
 */
#ifndef GNRC_SIXLOWPAN_FRAG_RBUF_TIMEOUT_MS
#define GNRC_SIXLOWPAN_FRAG_RBUF_TIMEOUT_MS (3U * US_PER_SEC)
#endif

/**
 * @brief   Registration lifetime in minutes for the address registration option
 *
 * This value should be adapted to the devices power-lifecycle so that it is
 * greater than the time the device spends sleeping.
 *
 * @see [RFC 6775, section 5.8.1](https://tools.ietf.org/html/rfc6775#section-5.8.1)
 *
 * @note    Only applicable with [gnrc_ipv6_nib](@ref net_gnrc_ipv6_nib) and
 *          [gnrc_sixlowpan_nd](@ref net_gnrc_sixlowpan_nd) modules. The first
 *          provides automatic sending of neighbor solicitations, the latter
 *          provides capabilities to build the address registration option as a
 *          @ref gnrc_pktsnip_t
 */
#ifndef GNRC_SIXLOWPAN_ND_AR_LTIME
#define GNRC_SIXLOWPAN_ND_AR_LTIME          (15U)
#endif

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_SIXLOWPAN_CONFIG_H */
/** @} */
