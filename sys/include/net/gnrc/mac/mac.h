/*
 * Copyright (C) 2015 Daniel Krebs
 *               2016 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    net_gnrc_mac Common MAC module
 * @ingroup     net_gnrc
 * @brief       A MAC module for providing common MAC parameters and helper functions.
 * @deprecated  This module is deprecated and will be removed after the 2024.10 release.
 *              As an alternative, you can use @ref pkg_opendsme.
 *
 * @{
 *
 * @file
 * @brief       Definitions of GNRC_MAC
 *
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @author      Shuguo Zhuo  <shuguo.zhuo@inria.fr>
 */

#include "modules.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup net_gnrc_mac_conf    GNRC MAC compile configurations
 * @ingroup  net_gnrc_conf
 * @{
 */
/**
 * @brief   Default message queue size to use for the incoming packets (as
 *          exponent of 2^n).
 *
 * As the queue size ALWAYS needs to be power of two, this option represents the
 * exponent of 2^n, which will be used as the size of the queue.
 */
#ifndef CONFIG_GNRC_MAC_RX_QUEUE_SIZE_EXP
#define CONFIG_GNRC_MAC_RX_QUEUE_SIZE_EXP   (3U)
#endif

/**
 * @brief   Default buffer size to use for storing dispatching packets (as
 *          exponent of 2^n).
 *
 * As the buffer size ALWAYS needs to be power of two, this option represents
 * the exponent of 2^n, which will be used as the size of the buffer.
 */
#ifndef CONFIG_GNRC_MAC_DISPATCH_BUFFER_SIZE_EXP
#define CONFIG_GNRC_MAC_DISPATCH_BUFFER_SIZE_EXP   (3U)
#endif

/**
 * @brief   Count of neighbor nodes in one-hop distance.
 */
#ifndef CONFIG_GNRC_MAC_NEIGHBOR_COUNT
#define CONFIG_GNRC_MAC_NEIGHBOR_COUNT      (8U)
#endif

/**
 * @brief   Default queue size for transmission packets coming from higher
 *          layers (as exponent of 2^n).
 *
 * As the queue size ALWAYS needs to be power of two, this option represents
 * the exponent of 2^n, which will be used as the size of the buffer.
 */
#ifndef CONFIG_GNRC_MAC_TX_QUEUE_SIZE_EXP
#define CONFIG_GNRC_MAC_TX_QUEUE_SIZE_EXP   (3U)
#endif

/**
 * @brief Disable MAC radio duty-cycle recording and displaying.
 */
#ifdef DOXYGEN
#define CONFIG_GNRC_MAC_DISABLE_DUTYCYCLE_RECORD
#endif

/**
 * @brief Enable/disable MAC radio duty-cycle recording and displaying.
 *
 * Set "1" to enable, set "0" to disable.
 * @deprecated Use inverse @ref CONFIG_GNRC_MAC_DISABLE_DUTYCYCLE_RECORD
 * instead. Will be removed after 2021.01 release.
 */
#ifndef GNRC_MAC_ENABLE_DUTYCYCLE_RECORD
#if IS_ACTIVE(CONFIG_GNRC_MAC_DISABLE_DUTYCYCLE_RECORD)
#define GNRC_MAC_ENABLE_DUTYCYCLE_RECORD    (0)
#else
#define GNRC_MAC_ENABLE_DUTYCYCLE_RECORD    (1U)
#endif
#endif
/** @} */

/**
 * @brief   The default rx queue size for incoming packets
 */
#ifndef GNRC_MAC_RX_QUEUE_SIZE
#define GNRC_MAC_RX_QUEUE_SIZE       (1 << CONFIG_GNRC_MAC_RX_QUEUE_SIZE_EXP)
#endif

/**
 * @brief   The default buffer size for storing dispatching packets
 */
#ifndef GNRC_MAC_DISPATCH_BUFFER_SIZE
#define GNRC_MAC_DISPATCH_BUFFER_SIZE  (1 << CONFIG_GNRC_MAC_DISPATCH_BUFFER_SIZE_EXP)
#endif

/**
 * @brief   The default queue size for transmission packets coming from higher
 *          layers
 */
#ifndef GNRC_MAC_TX_QUEUE_SIZE
#define GNRC_MAC_TX_QUEUE_SIZE          (1 << CONFIG_GNRC_MAC_TX_QUEUE_SIZE_EXP)
#endif

#ifdef __cplusplus
}
#endif

/** @} */
