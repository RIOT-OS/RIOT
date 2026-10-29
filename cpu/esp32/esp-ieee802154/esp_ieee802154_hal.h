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
 * The radio itself is enabled and configured when it is switched on with
 * @ref ieee802154_radio_request_on. Each time the radio is switched on,
 * - the ESP32x 802.15.4 subsystem is initialized if not already done,
 * - the source matching table for short and extended addresses is cleared,
 * - the CCA mode is set to energy detection only (IEEE802154_CCA_MODE_ED_THRESHOLD),
 * - the timeout for the ACK frame is set, and
 * - the promiscuous mode is disabled.
 *
 * The radio is in idle state after it has been switched on.
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
