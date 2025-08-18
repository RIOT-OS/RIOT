/*
 * SPDX-FileCopyrightText: 2022 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_esp32c3_devkit
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
#define ARDUINO_PIN_0       GPIO20  /**< Arduino pin 0 (RxD) */
#define ARDUINO_PIN_1       GPIO21  /**< Arduino pin 1 (TxD) */
#define ARDUINO_PIN_2       GPIO0   /**< Arduino pin 2 */
#define ARDUINO_PIN_3       GPIO1   /**< Arduino pin 3 (PWM) */
#define ARDUINO_PIN_4       GPIO9   /**< Arduino pin 4 (BOOT / BTN0) */
#define ARDUINO_PIN_5       GPIO3   /**< Arduino pin 5 (PWM) */
#define ARDUINO_PIN_6       GPIO8   /**< Arduino pin 6 (RGB-LED) */

/* Arduino pins D7 ... D9 are not available */

#define ARDUINO_PIN_10      GPIO10  /**< Arduino pin 10 (CS0 / PWM)  */
#define ARDUINO_PIN_11      GPIO7   /**< Arduino pin 11 (MOSI / PWM) */
#define ARDUINO_PIN_12      GPIO2   /**< Arduino pin 12 (MISO) */
#define ARDUINO_PIN_13      GPIO6   /**< Arduino pin 13 (SCK)  */

/* analog pins as digital pins: */
#define ARDUINO_PIN_14      GPIO0   /**< Arduino pin 14 (A0) */
#define ARDUINO_PIN_15      GPIO1   /**< Arduino pin 15 (A1) */
#define ARDUINO_PIN_16      GPIO2   /**< Arduino pin 16 (A2) */
#define ARDUINO_PIN_17      GPIO3   /**< Arduino pin 17 (A3) */

#define ARDUINO_PIN_18      GPIO5   /**< Arduino pin 18 (A4 / SDA) */
#define ARDUINO_PIN_19      GPIO4   /**< Arduino pin 19 (A5 / SCL) */

#define ARDUINO_PIN_LAST    19      /**< Last Arduino pin index */
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
#define ARDUINO_A4          ADC_LINE(4)     /**< ADC line for Arduino pin A4 */
#define ARDUINO_A5          ADC_LINE(5)     /**< ADC line for Arduino pin A5 */

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

#ifndef MODULE_PERIPH_SPI
#define ARDUINO_PIN_10_PWM_DEV  PWM_DEV(1)  /**< PWM device for Arduino pin 10 */
#define ARDUINO_PIN_10_PWM_CHAN 0           /**< PWM channel for Arduino pin 10 */

#define ARDUINO_PIN_11_PWM_DEV  PWM_DEV(1)  /**< PWM device for Arduino pin 11 */
#define ARDUINO_PIN_11_PWM_CHAN 1           /**< PWM channel for Arduino pin 11 */
#endif
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
