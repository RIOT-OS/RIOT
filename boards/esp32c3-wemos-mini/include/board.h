/*
 * SPDX-FileCopyrightText: 2023 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_esp32c3_wemos_mini
 * @brief       Board definitions for the Wemos ESP32-C3 mini board
 * @{
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
 * The Wemos ESP32-C3 mini board has a BOOT button connected to GPIO9, which can
 * be used as button during normal operation. Since the GPIO9 pin is pulled up,
 * the button signal is inverted, i.e., pressing the button will give a
 * low signal.
 */
#define BTN0_PIN        GPIO9

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
 * LED configuration depends on Wemos ESP32-C3 mini board version:
 *
 * - v1.0.0 has one LED connected to GPIO7.
 * - v2.1.0 has one 24-bit RGB-LED WS2812B connected to GPIO7.
 * @{
 */
#ifdef MODULE_ESP32C3_WEMOS_MINI_V1_0_0

#define LED0_PIN    GPIO7   /**< GPIO7 is used as LED Pin */
#define LED0_ACTIVE (1)     /**< LED is HIGH active */

#else /* MODULE_ESP32C3_WEMOS_MINI_V2_1_0 */

#ifndef WS281X_PARAM_PIN
#define WS281X_PARAM_PIN    (GPIO7)   /**< GPIO pin connected to the data pin */
#endif
#ifndef WS281X_PARAM_NUMOF
#define WS281X_PARAM_NUMOF  (1U)      /**< Number of LEDs chained */
#endif

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
