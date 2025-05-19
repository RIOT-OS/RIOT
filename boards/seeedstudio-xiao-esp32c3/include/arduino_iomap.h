/*
 * Copyright (C)  2025 David Picard
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_seeedstudio-xiao-esp32c3
 * @{
 *
 * @file
 * @brief       Mapping from MCU pins to Arduino pins
 *
 * @author      David Picard
 */

#ifndef ARDUINO_IOMAP_H
#define ARDUINO_IOMAP_H

#include "periph/gpio.h"
#include "periph/adc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   Mapping of MCU pins to Arduino pins
 * @{
 */
#define ARDUINO_PIN_0       GPIO2       /**< Arduino pin 0 */
#define ARDUINO_PIN_1       GPIO3       /**< Arduino pin 1 */
#define ARDUINO_PIN_2       GPIO4       /**< Arduino pin 2 */
#define ARDUINO_PIN_3       GPIO5       /**< Arduino pin 3 */
#define ARDUINO_PIN_4       GPIO6       /**< Arduino pin 4 (I2C SDA) */
#define ARDUINO_PIN_5       GPIO7       /**< Arduino pin 5 (I2C SCL) */
#define ARDUINO_PIN_6       GPIO21      /**< Arduino pin 1 (TxD) */
#define ARDUINO_PIN_7       GPIO20      /**< Arduino pin 0 (RxD) */
#define ARDUINO_PIN_8       GPIO8       /**< Arduino pin 8 (SPI SCK) */
#define ARDUINO_PIN_9       GPIO9       /**< Arduino pin 0 (SPI MISO)*/
#define ARDUINO_PIN_10      GPIO10      /**< Arduino pin 0 (SPI MOSI) */

#define ARDUINO_PIN_LAST    10          /**< Last Arduino pin index. */
/** @} */

/**
 * @name    Aliases for analog pins
 * @{
 */
#define ARDUINO_A0          ADC_LINE(0)     /**< ADC line for Arduino pin A0 */
#define ARDUINO_A1          ADC_LINE(1)     /**< ADC line for Arduino pin A1 */
#define ARDUINO_A2          ADC_LINE(2)     /**< ADC line for Arduino pin A2 */

#define ARDUINO_ANALOG_PIN_LAST 2           /**< Last Arduino analog pin index */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_IOMAP_H */
/** @} */
