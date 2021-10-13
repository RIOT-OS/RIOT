/*
 * Copyright (C)  2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_esp32_mh-et-live-minikit
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
#define ARDUINO_PIN_0   GPIO3       /**< Arduino Uno pin 0 (RxD) */
#define ARDUINO_PIN_1   GPIO1       /**< Arduino Uno pin 1 (TxD) */

#define ARDUINO_PIN_2   GPIO32      /**< Arduino Uno pin 2 */
#define ARDUINO_PIN_3   GPIO2       /**< Arduino Uno pin 3 (PWM) */
#define ARDUINO_PIN_4   GPIO27      /**< Arduino Uno pin 4 */
#define ARDUINO_PIN_5   GPIO0       /**< Arduino Uno pin 5 (PWM) */
#define ARDUINO_PIN_6   GPIO4       /**< Arduino Uno pin 6 (PWM) */
#define ARDUINO_PIN_7   GPIO33      /**< Arduino Uno pin 7 */
#define ARDUINO_PIN_8   GPIO25      /**< Arduino Uno pin 8 */
#define ARDUINO_PIN_9   GPIO15      /**< Arduino Uno pin 9 (PWM) */

#define ARDUINO_PIN_10  GPIO5       /**< Arduino Uno pin 10 (CS0 / PWM)  */
#define ARDUINO_PIN_11  GPIO23      /**< Arduino Uno pin 11 (MOSI / PWM) */
#define ARDUINO_PIN_12  GPIO19      /**< Arduino Uno pin 12 (MISO) */
#define ARDUINO_PIN_13  GPIO18      /**< Arduino Uno pin 13 (SCK)  */

#define ARDUINO_PIN_A0  GPIO34      /**< Arduino Uno pin A0 */
#define ARDUINO_PIN_A1  GPIO35      /**< Arduino Uno pin A1 */
#define ARDUINO_PIN_A2  GPIO36      /**< Arduino Uno pin A2 */
#define ARDUINO_PIN_A3  GPIO39      /**< Arduino Uno pin A3 */

#define ARDUINO_PIN_A4  GPIO21      /**< Arduino Uno pin A4 (SDA) */
#define ARDUINO_PIN_A5  GPIO22      /**< Arduino Uno pin A5 (SCL) */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_PINMAP_H */
/** @} */
