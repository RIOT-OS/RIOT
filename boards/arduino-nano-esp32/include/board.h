/*
 * SPDX-FileCopyrightText: 2025 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_arduino_nano_esp32
 * @brief       Board definitions for Arduino Nano ESP32 boards
 * @{
 *
 * The board definitions in this file are valid for the Arduino Nano ESP32 board.
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
 * @name    LED (on-board) configuration
 *
 * The board has a RGB-LED connected with separate color LEDs.
 * @{
 */
#define LED0_PIN        GPIO46  /**< LED0 pin (red) */
#define LED0_ACTIVE     (0)     /**< LED0 is low active */

#define LED1_PIN        GPIO0    /**< LED1 pin (green) */
#define LED1_ACTIVE     (0)      /**< LED1 is low active */

#define LED2_PIN        GPIO45   /**< LED2 pin (blue) */
#define LED2_ACTIVE     (0)      /**< LED2 is low active */

#ifdef  LED0_PIN
#  define LED_RED_PIN   LED0_PIN /**< LED0 is a red LED */
#endif

#ifdef  LED1_PIN
#  define LED_GREEN_PIN LED1_PIN /**< LED1 is a green LED */
#endif

#ifdef  LED2_PIN
#  define LED_BLUE_PIN  LED2_PIN /**< LED2 is a blue LED */
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
