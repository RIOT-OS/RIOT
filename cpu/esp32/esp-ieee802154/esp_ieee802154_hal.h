/*
 * SPDX-FileCopyrightText: 2025 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    cpu_esp32_esp_ieee802154 ESP32x IEEE 802.15.4 driver
 * @ingroup     drivers_netdev
 * @{
 *
 * @file
 * @brief       ESP32x IEEE 802.15.4 driver
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include <stdbool.h>

#include "net/ieee802154/radio.h"

/**
 * @brief   Setup ESP32x in order to be used with the IEEE 802.15.4 Radio HAL
 *
 * @param[in] dev  pointer to the HAL descriptor associated to the device.
 */
void esp_ieee802154_setup(ieee802154_dev_t *dev);

/**
 * @brief Initialize the ESP32x IEEE 802.15.4 module.
 *
 * The function
 * - initializes the ESP32x 802.15.4 subsystem,
 * - clears the source matching table for short and extended addresses,
 * - enables the radio in idle state,
 * - sets the CCA mode to energy detection only (IEEE802154_CCA_MODE_ED_THRESHOLD),
 * - sets the timeout for the ACK frame, and
 * - disables the promicuous mode.
 *
 * @retval 0 on success
 * @retval negative errno on error
 */
int esp_ieee802154_init(void);

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

/** @} */
