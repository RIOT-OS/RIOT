/*
 * Copyright (C) 2019 Yegor Yefremov
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_esp32_ttgo-t-beam
 * @{
 *
 * @file
 * @brief       Board specific configuration for the Arduino API
 *
 * @author      Yegor Yefremov <yegorslists@googlemail.com>
 */

#ifndef ARDUINO_BOARD_H
#define ARDUINO_BOARD_H

#include "arduino_board_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The on-board LED is connected to Arduino pin 3 on this board
 */
#ifndef MODULE_ESP32_TTGO_T_BEAM_V1_0
#define ARDUINO_LED         (3)
#endif

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_BOARD_H */
/** @} */
