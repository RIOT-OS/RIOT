/*
 * SPDX-FileCopyrightText: 2018 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_esp32_wroom-32
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

/**
 * @name   Mapping of MCU pins to Arduino pins
 * @{
 */
#define ARDUINO_PIN_0   GPIO3       /**< Arduino pin 0 (RxD) */
#define ARDUINO_PIN_1   GPIO1       /**< Arduino pin 1 (TxD) */

#define ARDUINO_PIN_2   GPIO12      /**< Arduino pin 2 */
#define ARDUINO_PIN_3   GPIO27      /**< Arduino pin 3 (PWM) */
#define ARDUINO_PIN_4   GPIO2       /**< Arduino pin 4 (PWM) */
#define ARDUINO_PIN_5   GPIO32      /**< Arduino pin 5 (PWM) */
#define ARDUINO_PIN_6   GPIO33      /**< Arduino pin 6 (PWM) */
#define ARDUINO_PIN_7   GPIO13      /**< Arduino pin 7 */
#define ARDUINO_PIN_8   GPIO14      /**< Arduino pin 8 */
#define ARDUINO_PIN_9   GPIO0       /**< Arduino pin 9 (PWM / BTN0) */

#define ARDUINO_PIN_10  GPIO5       /**< Arduino pin 10 (CS0)  */
#define ARDUINO_PIN_11  GPIO23      /**< Arduino pin 11 (MOSI) */
#define ARDUINO_PIN_12  GPIO19      /**< Arduino pin 12 (MISO) */
#define ARDUINO_PIN_13  GPIO18      /**< Arduino pin 13 (SCK)  */

/* analog pins as digital pin: */
#define ARDUINO_PIN_14  GPIO25      /**< Arduino pin 14 (A0) */
#define ARDUINO_PIN_15  GPIO26      /**< Arduino pin 15 (A1) */
#define ARDUINO_PIN_16  GPIO4       /**< Arduino pin 16 (A2 / PWM) */
#define ARDUINO_PIN_17  GPIO15      /**< Arduino pin 17 (A3) */
#define ARDUINO_PIN_18  GPIO36      /**< Arduino pin 18 (A4) */
#define ARDUINO_PIN_19  GPIO39      /**< Arduino pin 19 (A5) */

#define ARDUINO_PIN_20  GPIO21      /**< Arduino pin 20 (SDA) */
#define ARDUINO_PIN_21  GPIO22      /**< Arduino pin 21 (SCL) */

#define ARDUINO_PIN_LAST    21      /**< Last Arduino pin index */
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

#define ARDUINO_PIN_DAC0    ARDUINO_PIN_14  /**< Arduino pin DAC0 */
#define ARDUINO_PIN_DAC1    ARDUINO_PIN_15  /**< Arduino pin DAC1 */
/** @} */

/**
 * @name    Mapping of Arduino analog pins to RIOT ADC lines
 * @{
 */
#define ARDUINO_A0          ADC_LINE(7)     /**< ADC line for Arduino pin A0 */
#define ARDUINO_A1          ADC_LINE(8)     /**< ADC line for Arduino pin A1 */
#define ARDUINO_A2          ADC_LINE(2)     /**< ADC line for Arduino pin A2 */
#define ARDUINO_A3          ADC_LINE(6)     /**< ADC line for Arduino pin A3 */
#define ARDUINO_A4          ADC_LINE(14)    /**< ADC line for Arduino pin A4 */
#define ARDUINO_A5          ADC_LINE(15)    /**< ADC line for Arduino pin A5 */

#define ARDUINO_ANALOG_PIN_LAST 5           /**< Last Arduino analog pin index */
/** @} */

/**
 * @name    Mapping of Arduino DAC pins to RIOT DAC lines
 * @{
 */
#define ARDUINO_DAC0        DAC_LINE(0)     /**< DAC line for Arduino pin DAC0 */
#define ARDUINO_DAC1        DAC_LINE(1)     /**< DAC line for Arduino pin DAC1 */

#define ARDUINO_DAC_PIN_LAST    1           /**< Last Arduino DAC pin index */
/** @} */

/**
 * @name    Mapping of Arduino pins to RIOT PWM dev and channel pairs
 * @{
 */
#define ARDUINO_PIN_3_PWM_DEV   PWM_DEV(1)  /**< PWM device for Arduino pin 3 */
#define ARDUINO_PIN_3_PWM_CHAN  0           /**< PWM channel for Arduino pin 3 */

#define ARDUINO_PIN_4_PWM_DEV   PWM_DEV(0)  /**< PWM device for Arduino pin 4 */
#define ARDUINO_PIN_4_PWM_CHAN  1           /**< PWM channel for Arduino pin 4 */

#define ARDUINO_PIN_5_PWM_DEV   PWM_DEV(1)  /**< PWM device for Arduino pin 5 */
#define ARDUINO_PIN_5_PWM_CHAN  1           /**< PWM channel for Arduino pin 5 */

#define ARDUINO_PIN_6_PWM_DEV   PWM_DEV(1)  /**< PWM device for Arduino pin 6 */
#define ARDUINO_PIN_6_PWM_CHAN  2           /**< PWM channel for Arduino pin 6 */

#define ARDUINO_PIN_9_PWM_DEV   PWM_DEV(0)  /**< PWM device for Arduino pin 9 */
#define ARDUINO_PIN_9_PWM_CHAN  0           /**< PWM channel for Arduino pin 9 */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
