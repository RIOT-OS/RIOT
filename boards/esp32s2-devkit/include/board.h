/*
 * Copyright (C) 2022 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_esp32s2_devkit
 * @brief       Board definitions for ESP32-S2-DevKit boards
 * @{
 *
 * The board definitions in this file are valid for Espressif
 * ESP32-S2-DevKitx boards that use one of the following modules:
 *
 * - ESP32-S2-MINI-1x module (ESP32-S2-DevKitM-1 board)
 * - ESP32-S2-WROOM-1x module (ESP32-S2-DevKitC-1 board)
 * - ESP32-S2-WROOM-2x module (ESP32-S2-DevKitC-1 board)
 *
 * where x stands for the module version with and without U
 * (external antenna connector).
 *
 * Any modifications required for specific applications
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
 * ESP32-S2-DevKit boards have a BOOT button connected to GPIO9, which can be
 * used as button during normal operation. Since the GPIO9 pin is pulled up,
 * the button signal is inverted, i.e., pressing the button will give a
 * low signal.
 */
#define BTN0_PIN        GPIO0

/**
 * @brief   Default button GPIO mode definition
 *
 * Since the GPIO of the button is pulled up with an external resistor, the
 * mode for the GPIO pin has to be GPIO_IN.
 */
#define BTN0_MODE       GPIO_IN_PU

/**
 * @brief   Default interrupt flank definition for the button GPIO
 */
#ifndef BTN0_INT_FLANK
#define BTN0_INT_FLANK  GPIO_FALLING
#endif

/**
 * @brief   Definition for compatibility with previous versions
 */
#define BUTTON0_PIN     BTN0_PIN

/** @} */

/**
 * @name    LED (on-board) configuration
 *
 * ESP32-S2-DevKit boards have a SK68XXMINI-HS smart RGB-LED connected to
 * GPIO18 on-board. The WS281x driver module `ws281x` can be used to control it.
 *
 * @note GPIO18 is also be defined as DAC channel. The RGB-LED can be
 * used as long as GPIO18 is not initialized as DAC channel with the
 * function `dac_init`.
 * @{
 */
#ifndef WS281X_PARAM_PIN
#define WS281X_PARAM_PIN    (GPIO18)  /**< GPIO pin connected to the data pin */
#endif
#ifndef WS281X_PARAM_NUMOF
#define WS281X_PARAM_NUMOF  (1U)      /**< Number of LEDs chained */
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
