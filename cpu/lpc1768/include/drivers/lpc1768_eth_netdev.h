/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    cpu_lpc1768_drivers_eth LPC1768 Ethernet peripheral
 * @ingroup     cpu_lpc1768_drivers
 * @ingroup     drivers_netdev
 *
 * @brief       Driver for the LPC1768 Ethernet peripheral
 *
 * ## Link State Monitoring
 *
 * To monitor the actual state of the link, use the generic (pseudo) module
 * `netdev_eth_link_state`, which selects the `lpc1768_eth_link_up`
 * implementation of this driver automatically, and emits events when the link
 * goes up or down. Without this module, the driver assumes the link is up and
 * signals `NETDEV_EVENT_LINK_UP` once during initialization, and never signals
 * `NETDEV_EVENT_LINK_DOWN`. To force the `lpc1768_eth` specific implementation
 * instead of the generic one, use the (pseudo) module `lpc1768_eth_link_up`
 * directly.
 *
 * ## Link Autonegotiation
 *
 * To enable Link Autonegotiation, use the generic (pseudo) module
 * `netdev_eth_autoneg`, which selects the `lpc1768_eth_auto` implementation of
 * this driver automatically.
 *
 * In general, it is highly recommended to use auto-negotiation, as this can
 * avoid various communication issues on the PHY layer due to configuration
 * mismatch of the link partners.
 *
 * This feature depends on the link state events feature. To force the
 * `lpc1768_eth` specific implementation instead of the generic one, use the
 * (pseudo) module `lpc1768_eth_auto` directly.
 *
 * @{
 *
 * @file
 * @brief       Setup for netdev for the LPC1768 ethernet peripheral
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#include "net/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The netdev driver for the LPC1768 EMAC
 */
extern const netdev_driver_t lpc1768_eth_driver;

/**
 * @brief   Bind the given netdev to the LPC1768 EMAC driver
 *
 * Does not touch hardware. The netdev `init` callback performs the actual
 * peripheral initialization.
 *
 * @param[in,out] netdev    netdev to bind
 */
void lpc1768_eth_netdev_setup(netdev_t *netdev);

#ifdef __cplusplus
}
#endif

/** @} */
