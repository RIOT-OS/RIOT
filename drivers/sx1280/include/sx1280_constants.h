/*
 * Copyright (C) 2022 Inria
 * Copyright (C) 2020-2022 Université Grenoble Alpes
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_sx1280
 * @{
 *
 * @file
 * @brief       Internal addresses, registers and constants
 *
 * @author      Aymeric Brochier <aymeric.brochier@univ-grenoble-alpes.fr>
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    SX1280 Single RX mode.
 *
 * sx1280_set_rx constant to be in single mode
 *
 * There is a known issue about this in some circonstances described in section 16.1 of the datasheet:
 * https://manualzz.com/doc/55353537/semtech-sx1280-2.4-ghz-transceiver-datasheet
 *
 * When subjected to a high co-channel traffic conditions
 * (for example in BLE mode when the SX1280 receives more than 220 packets per second)
 * and only when configured in continuous receiver
 * The SX1280 busy line will remain high and the radio unresponsive.
 *
 * If the radio may be subject to high levels of BLE traffic, to allow the radio to remain in operation RX single mode must be used
 */
#define SX1280_RX_SINGLE_MODE 0x0000

/**
 * @brief    SX1280 Continuous RX mode.
 *
 * @note     This addresses a known issue detailed in section 16.1 of the datasheet, see
 *           @ref SX1280_RX_SINGLE_MODE for more details.
 *
 * sx1280_set_rx require this constant to be in continuous mode
 *
 */
#define SX1280_RX_CONTINUOUS_MODE 0xFFFF

/**
 * @brief    SX1280 wakeup time in ms
 * @note From the datasheet wakeup time is usually 1200us but rounded up to 2 to only use the ZTIMER_MSEC
 *
 */
#define SX1280_WAKEUP_TIME_MS 2

/**
 * @brief    SX1280 reset time in ms
 * hold NRST pin low for 1ms, nothing is specified in the datasheet, 1ms worked fine
 *
 */
#define SX1280_RESET_MS 1

#ifdef __cplusplus
}
#endif

/** @} */
