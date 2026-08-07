/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    cpu_efm32_drivers_eth EFM32 Ethernet peripheral
 * @ingroup     cpu_efm32_drivers
 * @ingroup     drivers_netdev
 *
 * ## Link State Events
 *
 * To enable Link Events, use the (pseudo) module `efm32_eth_link_up`. This
 * module will monitor the link state, and will emit events when the link goes
 * up or down.
 *
 * ## Link Auto Negotiation
 *
 * To enable Link Auto Negotiation, use the (pseudo) module `efm32_eth_auto`.
 *
 * In general, it is highly recommended to use auto-negotiation, as this can
 * avoid various communication issues on the PHY layer due to configuration
 * mismatch of the link partners.
 *
 * This feature depends on the link state events feature.
 *
 * @{
 *
 * @file
 * @brief       Setup for netdev for the EFM32 ethernet peripheral
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#include "net/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The netdev driver for the EFM32 ethernet peripheral
 */
extern const netdev_driver_t efm32_eth_driver;

/**
 * @brief   Bind the given netdev to the EFM32 ethernet driver
 *
 * Does not touch hardware. The netdev `init` callback performs the actual
 * peripheral initialization.
 *
 * @param[in,out] netdev    netdev to bind
 */
void efm32_eth_netdev_setup(netdev_t *netdev);

#ifdef __cplusplus
}
#endif

/** @} */
