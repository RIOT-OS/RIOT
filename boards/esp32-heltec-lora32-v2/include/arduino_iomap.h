/*
 * SPDX-FileCopyrightText: 2019 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_esp32_heltec-lora32-v2
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

#define ARDUINO_LED             (3)         /**< LED is connected to Arduino pin 3 */

/**
 * @name   Mapping of MCU pins to Arduino pins
 * @{
 */
#define ARDUINO_PIN_0       GPIO3           /**< Arduino pin 0 (RxD) */
#define ARDUINO_PIN_1       GPIO1           /**< Arduino pin 1 (TxD) */

#define ARDUINO_PIN_2       GPIO0           /**< Arduino pin 2 (BTN0) */
#define ARDUINO_PIN_3       GPIO25          /**< Arduino pin 3 (PWM / LED) */
#define ARDUINO_PIN_4       GPIO12          /**< Arduino pin 4 */
#define ARDUINO_PIN_5       GPIO17          /**< Arduino pin 5 (PWM) */
#define ARDUINO_PIN_6       GPIO2           /**< Arduino pin 6 (PWM) */
#define ARDUINO_PIN_9       GPIO13          /**< Arduino pin 7 */
#define ARDUINO_PIN_7       GPIO22          /**< Arduino pin 8 (PWM) */
#define ARDUINO_PIN_8       GPIO23          /**< Arduino pin 9 (PWM) */

#define ARDUINO_PIN_10      GPIO18          /**< Arduino pin 10 (CS0)  */
#define ARDUINO_PIN_11      GPIO27          /**< Arduino pin 11 (MOSI) */
#define ARDUINO_PIN_12      GPIO19          /**< Arduino pin 12 (MISO) */
#define ARDUINO_PIN_13      GPIO5           /**< Arduino pin 13 (SCK)  */

/* analog pins as digital pin: */
#define ARDUINO_PIN_14      GPIO36          /**< Arduino pin 14 (A0) */
#define ARDUINO_PIN_15      GPIO39          /**< Arduino pin 15 (A1) */
#define ARDUINO_PIN_16      GPIO37          /**< Arduino pin 16 (A2) */
#define ARDUINO_PIN_17      GPIO38          /**< Arduino pin 17 (A3) */

#define ARDUINO_PIN_18      GPIO4           /**< Arduino pin 18 (A4 / SDA) */
#define ARDUINO_PIN_19      GPIO15          /**< Arduino pin 19 (A5 / SCL) */

#define ARDUINO_PIN_LAST    19
/** @} */

/**
 * @name    Aliases for analog pins
 * @{
 */
#define ARDUINO_PIN_A0      ARDUINO_PIN_14  /**< Arduino pin A0 */
#define ARDUINO_PIN_A1      ARDUINO_PIN_15  /**< Arduino pin A1 */
#define ARDUINO_PIN_A2      ARDUINO_PIN_16  /**< Arduino pin A2 */
#define ARDUINO_PIN_A3      ARDUINO_PIN_17  /**< Arduino pin A3 */
#define ARDUINO_PIN_A4      ARDUINO_PIN_18  /**< Arduino pin A4 */
#define ARDUINO_PIN_A5      ARDUINO_PIN_19  /**< Arduino pin A5 */
/** @} */

/**
 * @name    Mapping of Arduino analog pins to RIOT ADC lines
 * @{
 */
#define ARDUINO_A0          ADC_LINE(0)     /**< ADC line for Arduino pin A0 */
#define ARDUINO_A1          ADC_LINE(1)     /**< ADC line for Arduino pin A1 */
#define ARDUINO_A2          ADC_LINE(2)     /**< ADC line for Arduino pin A2 */
#define ARDUINO_A3          ADC_LINE(3)     /**< ADC line for Arduino pin A3 */
#define ARDUINO_A4          ADC_LINE(8)     /**< ADC line for Arduino pin A4 */
#define ARDUINO_A5          ADC_LINE(9)     /**< ADC line for Arduino pin A5 */

#define ARDUINO_ANALOG_PIN_LAST 5           /**< Last Arduino analog pin index */
/** @} */

/**
 * @name    Mapping of Arduino pins to RIOT PWM dev and channel pairs
 * @{
 */
#define ARDUINO_PIN_3_PWM_DEV   PWM_DEV(0)  /**< PWM device for Arduino pin 3 */
#define ARDUINO_PIN_3_PWM_CHAN  0           /**< PWM channel for Arduino pin 3 */

#define ARDUINO_PIN_5_PWM_DEV   PWM_DEV(0)  /**< PWM device for Arduino pin 5 */
#define ARDUINO_PIN_5_PWM_CHAN  1           /**< PWM channel for Arduino pin 5 */

#define ARDUINO_PIN_6_PWM_DEV   PWM_DEV(0)  /**< PWM device for Arduino pin 6 */
#define ARDUINO_PIN_6_PWM_CHAN  2           /**< PWM channel for Arduino pin 6 */

#define ARDUINO_PIN_8_PWM_DEV   PWM_DEV(1)  /**< PWM device for Arduino pin 8 */
#define ARDUINO_PIN_8_PWM_CHAN  0           /**< PWM channel for Arduino pin 8 */

#define ARDUINO_PIN_9_PWM_DEV   PWM_DEV(1)  /**< PWM device for Arduino pin 9 */
#define ARDUINO_PIN_9_PWM_CHAN  1           /**< PWM channel for Arduino pin 9 */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
