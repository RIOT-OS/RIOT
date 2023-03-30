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
 * @brief       Mapping from MCU pins to Arduino pins
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef ARDUINO_PINMAP_H
#define ARDUINO_PINMAP_H

#include "periph/gpio.h"
#include "periph/adc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   Mapping of MCU pins to Arduino pins
 * @{
 */
#define ARDUINO_PIN_0   GPIO20      /**< Arduino Uno pin 0 (RxD) */
#define ARDUINO_PIN_1   GPIO21      /**< Arduino Uno pin 1 (TxD) */
#define ARDUINO_PIN_2   GPIO9       /**< Arduino Uno pin 2 */
#define ARDUINO_PIN_3   GPIO6       /**< Arduino Uno pin 3 (PWM) */
#define ARDUINO_PIN_4   GPIO_UNDEF  /**< Arduino Uno pin 4 */

#ifdef MODULE_ESP32C3_WEMOS_MINI_V1_0_0
#define ARDUINO_PIN_5   GPIO7       /**< Arduino Uno pin 5 (PWM) */
#define ARDUINO_PIN_6   GPIO1       /**< Arduino Uno pin 6 (PWM) */
#define ARDUINO_PIN_7   GPIO0       /**< Arduino Uno pin 7 */
#else /* MODULE_ESP32C3_WEMOS_MINI_V2_1_0 */
#define ARDUINO_PIN_5   GPIO_UNDEF  /**< Arduino Uno pin 5 (PWM) */
#define ARDUINO_PIN_6   GPIO2       /**< Arduino Uno pin 6 (PWM) */
#define ARDUINO_PIN_7   GPIO3       /**< Arduino Uno pin 7 */
#endif

#define ARDUINO_PIN_8   GPIO_UNDEF  /**< Arduino Uno pin 8 */
#define ARDUINO_PIN_9   GPIO_UNDEF  /**< Arduino Uno pin 9 (PWM) */

#ifdef MODULE_ESP32C3_WEMOS_MINI_V1_0_0
#define ARDUINO_PIN_10  GPIO5       /**< Arduino Uno pin 10 (CS0 / PWM)  */
#define ARDUINO_PIN_11  GPIO4       /**< Arduino Uno pin 11 (MOSI / PWM) */
#define ARDUINO_PIN_12  GPIO3       /**< Arduino Uno pin 12 (MISO) */
#define ARDUINO_PIN_13  GPIO2       /**< Arduino Uno pin 13 (SCK)  */
#else /* MODULE_ESP32C3_WEMOS_MINI_V2_1_0 */
#define ARDUINO_PIN_10  GPIO5       /**< Arduino Uno pin 10 (CS0 / PWM)  */
#define ARDUINO_PIN_11  GPIO4       /**< Arduino Uno pin 11 (MOSI / PWM) */
#define ARDUINO_PIN_12  GPIO0       /**< Arduino Uno pin 12 (MISO) */
#define ARDUINO_PIN_13  GPIO1       /**< Arduino Uno pin 13 (SCK)  */
#endif

#define ARDUINO_PIN_A0  GPIO0       /**< Arduino Uno pin A0 */
#define ARDUINO_PIN_A1  GPIO1       /**< Arduino Uno pin A1 */
#define ARDUINO_PIN_A2  GPIO2       /**< Arduino Uno pin A2 */
#define ARDUINO_PIN_A3  GPIO3       /**< Arduino Uno pin A3 */
#define ARDUINO_PIN_A4  GPIO4       /**< Arduino Uno pin A4 (SDA) */
#define ARDUINO_PIN_A5  GPIO5       /**< Arduino Uno pin A5 (SCL) */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_PINMAP_H */
/** @} */
