/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32_esp_eth
 * @{
 *
 * @file
 * @brief       Parameters for the netdev interface for ESP32 Ethernet MAC module
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef ESP_ETH_PARAMS_H
#define ESP_ETH_PARAMS_H

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
#endif /* ESP_ETH_PARAMS_H */
