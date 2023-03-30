/*
 * Copyright (C)  2022 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_esp32c3_wemos_mini
 * @{
 *
 * @file
 * @brief       Board specific configuration for the Arduino API
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef ARDUINO_BOARD_H
#define ARDUINO_BOARD_H

#include "arduino_board_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The on-board LED (not available on board version 2.1.0)
 */
#ifdef MODULE_ESP32C3_WEMOS_MINI_V1_0_0
#define ARDUINO_LED         (5)
#else /* MODULE_ESP32C3_WEMOS_MINI_V2_1_0 */
#define ARDUINO_LED         (0)
#endif

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_BOARD_H */
/** @} */
