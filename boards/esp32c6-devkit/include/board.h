/*
 * SPDX-FileCopyrightText: 2025 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_esp32c6_devkit
 * @brief       Board definitions for ESP32-C6-DevKit boards
 * @{
 *
 * The board definitions in this file are valid for Espressif
 * ESP32-C6-DevKitx boards that use one of the following modules:
 *
 * - ESP32-C6-MINI-1x module (ESP32-C6-DevKitM-1 board)
 * - ESP32-C6-WROOM-1x module (ESP32-C6-DevKitC-1 board)
 *
 * Since the number of GPIOs and their possible uses on the ESP32-C6 are quite
 * limited, these board definitions can also be used for most other
 * ESP32-C6 boards. Any modifications required for specific applications
 * can be overridden by \ref esp32_application_specific_configurations
 * "application-specific board configuration".
 *
 * @file
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include <stdint.h>

/**
 * @name    Button pin definitions
 * @{
 */

/**
 * @brief   Default button GPIO pin definition
 *
 * ESP32-C6-DevKit boards have a BOOT button connected to GPIO9, which can be
 * used as button during normal operation. Since the GPIO9 pin is pulled up,
 * the button signal is inverted, i.e., pressing the button will give a
 * low signal.
 */
#define BTN0_PIN            GPIO9

/**
 * @brief   Default button GPIO mode definition
 *
 * Since the GPIO pin of the button needs to be pulled up for normal operation
 * and the board does not have an external resistor connected to the GPIO pin,
 * the mode for the GPIO pin must be set to GPIO_IN_PU.
 */
#define BTN0_MODE           GPIO_IN_PU

/**
 * @brief   Default interrupt flank definition for the button GPIO
 */
#ifndef BTN0_INT_FLANK
#  define BTN0_INT_FLANK    GPIO_FALLING
#endif

/**
 * @brief   Definition for compatibility with previous versions
 */
#define BUTTON0_PIN         BTN0_PIN

/** @} */

/**
 * @name    LED (on-board) configuration
 *
 * ESP32-C6-DevKit boards have a SK68XXMINI-HS smart RGB-LED connected to
 * GPIO8 on-board. The WS281x driver module `ws281x` can be used to control it.
 * @{
 */
#ifndef WS281X_PARAM_PIN
#  define WS281X_PARAM_PIN      (GPIO8)   /**< GPIO pin connected to the data pin */
#endif
#ifndef WS281X_PARAM_NUMOF
#  define WS281X_PARAM_NUMOF    (1U)      /**< Number of LEDs chained */
#endif
/** @} */

/* include common board definitions as last step */
#include "board_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
