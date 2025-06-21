/*
 * Copyright (C) 2025 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_esp32h2_devkit
 * @brief       Board definitions for ESP32-H2-DevKitM-1 boards
 * @{
 *
 * The board definitions in this file are valid for the Espressif
 * ESP32-H2-DevKitM-1 board the ESP32-H2-MINI-1x module.
 *
 * Since the number of GPIOs and their possible uses on the ESP32-H2 are quite
 * limited, these board definitions can also be used for most other
 * ESP32-H2 boards. Any modifications required for specific applications
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
 * ESP32-H2-DevKitM-1 boards have a BOOT button connected to GPIO9, which can be
 * used as button during normal operation. Since the button connects GPIO9 pin
 * to GND, the button signal is inverted, i.e., pressing the button will give a
 * low signal.
 */
#define BTN0_PIN            GPIO9

/**
 * @brief   Default button GPIO mode definition
 *
 * Since the GPIO of the button is not pulled up with an external resistor, the
 * mode for the GPIO pin has to be GPIO_IN_PU.
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
 * ESP32-H2-DevKitM-1 boards have a SK68XXMINI-HS smart RGB-LED connected to
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
