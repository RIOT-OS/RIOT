/*
 * Copyright (C)  2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_esp32_olimex-esp32-evb
 * @{
 *
 * @file
 * @brief       Mapping from MCU pins to Arduino pins
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include "periph/gpio.h"
#include "periph/adc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ARDUINO_UART_D0D1       UART_DEV(0) /**< Arduino UART interface */
#define ARDUINO_SPI_D11D12D13   SPI_DEV(0)  /**< Arduino SPI bus */

#if MODULE_OLIMEX_ESP32_GATEWAY
#  define ARDUINO_LED           (3)         /**< LED is connected to Arduino pin 3 */
#endif

/**
 * @name   Mapping of MCU pins to Arduino pins
 * @{
 */
#define ARDUINO_PIN_0       GPIO3   /**< Arduino pin 0 (RxD) */
#define ARDUINO_PIN_1       GPIO1   /**< Arduino pin 1 (TxD) */

#define ARDUINO_PIN_2       GPIO34  /**< Arduino pin 2 (BTN0), input only! */
#define ARDUINO_PIN_3       GPIO33  /**< Arduino pin 3 (LED) */
#define ARDUINO_PIN_4       GPIO4   /**< Arduino pin 4 */
#define ARDUINO_PIN_5       GPIO9   /**< Arduino pin 5 */
#define ARDUINO_PIN_6       GPIO10  /**< Arduino pin 6 */

/* Arduino pins 7 ... 9 are not available */

#define ARDUINO_PIN_10      GPIO17  /**< Arduino pin 10 (CS0) */
#define ARDUINO_PIN_11      GPIO15  /**< Arduino pin 11 (MOSI) */
#define ARDUINO_PIN_12      GPIO2   /**< Arduino pin 12 (MISO) */
#define ARDUINO_PIN_13      GPIO14  /**< Arduino pin 13 (SCK) */

/* analog pins as digital pin */
#define ARDUINO_PIN_14      GPIO32  /**< Arduino pin 14 (A0) */
#define ARDUINO_PIN_15      GPIO35  /**< Arduino pin 15 (A1), input only! */
#define ARDUINO_PIN_16      GPIO36  /**< Arduino pin 16 (A2), input only! */
#define ARDUINO_PIN_17      GPIO39  /**< Arduino pin 17 (A3), input only! */

/* Arduino I2C pins */
#if MODULE_OLIMEX_ESP32_GATEWAY
#  define ARDUINO_PIN_18    GPIO17  /**< Arduino pin 18 (SDA) */
#  define ARDUINO_PIN_19    GPIO16  /**< Arduino pin 19 (SCL) */
#else /* ESP32-EVB */
#  define ARDUINO_PIN_18    GPIO13  /**< Arduino pin 18 (SDA) */
#  define ARDUINO_PIN_19    GPIO16  /**< Arduino pin 19 (SCL) */
#endif

#define ARDUINO_PIN_LAST    19
/** @} */

/**
 * @name    Aliases for analog pins
 * @{
 */
#if MODULE_OLIMEX_ESP32_GATEWAY
#  define ARDUINO_PIN_A0    ARDUINO_PIN_14  /**< Arduino pin A0 */
#  define ARDUINO_PIN_A1    ARDUINO_PIN_15  /**< Arduino pin A1 */
#  define ARDUINO_PIN_A2    ARDUINO_PIN_16  /**< Arduino pin A2 */
#  define ARDUINO_PIN_A3    ARDUINO_PIN_17  /**< Arduino pin A3 */
#endif
/** @} */

/**
 * @name    Mapping of Arduino analog pins to RIOT ADC lines
 * @{
 */
#if MODULE_OLIMEX_ESP32_GATEWAY
#  define ARDUINO_A0        ADC_LINE(0)     /**< ADC line for Arduino pin A0 */
#  define ARDUINO_A1        ADC_LINE(1)     /**< ADC line for Arduino pin A1 */
#  define ARDUINO_A2        ADC_LINE(2)     /**< ADC line for Arduino pin A2 */
#  define ARDUINO_A3        ADC_LINE(3)     /**< ADC line for Arduino pin A3 */

#  define ARDUINO_ANALOG_PIN_LAST   3       /**< Last Arduino analog pin index */
#endif
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
