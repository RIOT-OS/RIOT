/*
 * SPDX-FileCopyrightText: 2018 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     cpu_esp32_esp_eth
 * @{
 *
 * @file
 * @brief       Parameters for the netdev interface for ESP32 Ethernet MAC module
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#if defined(MODULE_ESP_ETH) || defined(DOXYGEN)

/**
 * @name    Set default configuration parameters for the ESP-ETH netdev driver
 * @{
 */
#ifndef ESP_ETH_STACKSIZE
/** The size of the stack used for the ESP-ETH netdev driver thread */
#define ESP_ETH_STACKSIZE    THREAD_STACKSIZE_DEFAULT
#endif

#ifndef ESP_ETH_PRIO
/** The priority of the ESP-ETH netdev driver thread */
#define ESP_ETH_PRIO         GNRC_NETIF_PRIO
#endif
/** @} */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* MODULE_ESP_ETH || DOXYGEN */

/** @} */
