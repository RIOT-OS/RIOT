/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

#include <assert.h>

/**
 * @ingroup     cpu_lpc1768
 * @{
 *
 * @file
 * @brief       LPC1768 default configuration
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Timeout for PHY auto-negotiation completion, in milliseconds.
 */
#ifndef CONFIG_LPC1768_ETH_AN_TIMEOUT_MS
#  define CONFIG_LPC1768_ETH_AN_TIMEOUT_MS      5000U
#endif

/**
 * @brief   Poll interval for checking the link state, in milliseconds.
 */
#ifndef CONFIG_LPC1768_ETH_LINK_POLL_MS
#  define CONFIG_LPC1768_ETH_LINK_POLL_MS       1000U
#endif

/**
 * @brief   Timeout for PHY operations, in microseconds.
 */
#ifndef CONFIG_LPC1768_ETH_PHY_TIMEOUT_US
#  define CONFIG_LPC1768_ETH_PHY_TIMEOUT_US     1000U
#endif

/**
 * @brief   Number of receive buffers.
 */
#ifndef CONFIG_LPC1768_ETH_RX_BUF_NUMOF
#  define CONFIG_LPC1768_ETH_RX_BUF_NUMOF       4U
#endif

static_assert(CONFIG_LPC1768_ETH_RX_BUF_NUMOF > 0,
              "At least one receive buffer is required");

/**
 * @brief   Number of transmit buffers.
 */
#ifndef CONFIG_LPC1768_ETH_TX_BUF_NUMOF
#  define CONFIG_LPC1768_ETH_TX_BUF_NUMOF       4U
#endif

static_assert(CONFIG_LPC1768_ETH_TX_BUF_NUMOF > 0,
              "At least one transmit buffer is required");

#ifdef __cplusplus
}
#endif

/** @} */
