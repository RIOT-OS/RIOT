/*
 * SPDX-FileCopyrightText: 2026 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_adafruit_metro_esp32s2
 * @brief       Board definitions for the Adafruit Metro ESP32-S2 board
 * @{
 *
 * The board definitions in this file are valid for the Adafruit Metro ESP32-S2
 * board.
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
 * The Adafruit Metro ESP32-S2 board has a BOOT button connected to GPIO0,
 * which can be used as button during normal operation. Since the GPIO0 pin
 * is pulled up, the button signal is inverted, i.e., pressing the button will
 * give a low signal.
 */
#define BTN0_PIN        GPIO0

/**
 * @brief   Default button GPIO mode definition
 */
#define BTN0_MODE       GPIO_IN_PU

/**
 * @brief   Default interrupt flank definition for the button GPIO
 */
#ifndef BTN0_INT_FLANK
#  define BTN0_INT_FLANK  GPIO_FALLING
#endif

/**
 * @brief   Definition for compatibility with previous versions
 */
#define BUTTON0_PIN     BTN0_PIN

/** @} */

/**
 * @name    LED (on-board) configuration
 *
 * The red LED is connected to GPIO42, which is also available as Arduino
 * pin D13.
 *
 * Furthermore, the board has a NeoPixel smart RGB-LED connected to GPIO45.
 * The WS281x driver module `ws281x` can be used to control it.
 * @{
 */
#define LED0_PIN        GPIO42  /**< red LED is connected to GPIO42 */
#define LED0_ACTIVE     1       /**< LED is high active */

#ifndef WS281X_PARAM_PIN
#  define WS281X_PARAM_PIN    (GPIO45)   /**< GPIO pin connected to the data pin */
#endif
#ifndef WS281X_PARAM_NUMOF
#  define WS281X_PARAM_NUMOF  (1U)       /**< Number of LEDs chained */
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
