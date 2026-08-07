/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

#include <assert.h>

/**
 * @ingroup     cpu_efm32
 * @{
 *
 * @file
 * @brief       EFM32 default configuration
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Timeout for PHY auto-negotiation completion, in milliseconds.
 */
#ifndef CONFIG_EFM32_ETH_AN_TIMEOUT_MS
#  define CONFIG_EFM32_ETH_AN_TIMEOUT_MS        5000U
#endif

/**
 * @brief   Poll interval for checking the link state, in milliseconds.
 */
#ifndef CONFIG_EFM32_ETH_LINK_POLL_MS
#  define CONFIG_EFM32_ETH_LINK_POLL_MS         1000U
#endif

/**
 * @brief   Timeout for PHY operations, in microseconds.
 */
#ifndef CONFIG_EFM32_ETH_PHY_TIMEOUT_US
#  define CONFIG_EFM32_ETH_PHY_TIMEOUT_US       1000U
#endif

/**
 * @brief   Number of receive buffers.
 */
#ifndef CONFIG_EFM32_ETH_RX_BUF_NUMOF
#  define CONFIG_EFM32_ETH_RX_BUF_NUMOF         4U
#endif

static_assert(CONFIG_EFM32_ETH_RX_BUF_NUMOF > 0,
              "At least one receive buffer is required");

/**
 * @brief   Number of transmit buffers.
 */
#ifndef CONFIG_EFM32_ETH_TX_BUF_NUMOF
#  define CONFIG_EFM32_ETH_TX_BUF_NUMOF         4U
#endif

static_assert(CONFIG_EFM32_ETH_TX_BUF_NUMOF > 0,
              "At least one transmit buffer is required");

/**
 * @brief   Use LETIMER as the base timer for xtimer (effectively ztimer).
 *
 * @deprecated  This option is deprecated and should be replaced by
 *              @ref CONFIG_EFM32_ZTIMER_USE_LETIMER.
 */
#ifndef CONFIG_EFM32_XTIMER_USE_LETIMER
#  define CONFIG_EFM32_XTIMER_USE_LETIMER       0
#endif

/**
 * @brief   Use LETIMER as the base timer for ztimer.
 */
#ifndef CONFIG_EFM32_ZTIMER_USE_LETIMER
#  if CONFIG_EFM32_XTIMER_USE_LETIMER
#    define CONFIG_EFM32_ZTIMER_USE_LETIMER     1
#  else
#    define CONFIG_EFM32_ZTIMER_USE_LETIMER     0
#  endif
#endif

#ifdef __cplusplus
}
#endif

/** @} */
