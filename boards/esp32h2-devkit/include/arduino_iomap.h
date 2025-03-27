/*
 * Copyright (C)  2025 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_esp32h2_devkit
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

/**
 * @name   Mapping of MCU pins to Arduino pins
 * @{
 */
#define ARDUINO_PIN_0       GPIO23  /**< Arduino pin 0 (RxD) */
#define ARDUINO_PIN_1       GPIO24  /**< Arduino pin 1 (TxD) */
#define ARDUINO_PIN_2       GPIO25  /**< Arduino pin 2 */
#define ARDUINO_PIN_3       GPIO2   /**< Arduino pin 3 (PWM) */
#if !defined(MODULE_ESP_RTC_TIMER_32K)
#  define ARDUINO_PIN_4     GPIO13  /**< Arduino pin 4 */
#endif
#define ARDUINO_PIN_5       GPIO12  /**< Arduino pin 5 (PWM) */
#define ARDUINO_PIN_6       GPIO22  /**< Arduino pin 6 (PWM) */
#define ARDUINO_PIN_7       GPIO9   /**< Arduino pin 7 (BTN0 input) */
#define ARDUINO_PIN_8       GPIO8   /**< Arduino pin 8 (RGB output) */
#define ARDUINO_PIN_9       GPIO3   /**< Arduino pin 9 (PWM) */

#define ARDUINO_PIN_10      GPIO1   /**< Arduino pin 10 (CS0 / PWM)  */
#define ARDUINO_PIN_11      GPIO5   /**< Arduino pin 11 (MOSI / PWM) */
#define ARDUINO_PIN_12      GPIO0   /**< Arduino pin 12 (MISO) */
#define ARDUINO_PIN_13      GPIO4   /**< Arduino pin 13 (SCK)  */

/* analog pins as digital pins: */
#define ARDUINO_PIN_14      GPIO1   /**< Arduino pin A0 */
#define ARDUINO_PIN_15      GPIO2   /**< Arduino pin A1 */
#define ARDUINO_PIN_16      GPIO3   /**< Arduino pin A2 */
#define ARDUINO_PIN_17      GPIO4   /**< Arduino pin A3 */

/* I2C digital pins: */
#define ARDUINO_PIN_18      GPIO11  /**< Arduino SDA */
#define ARDUINO_PIN_19      GPIO10  /**< Arduino SCL */

#define ARDUINO_PIN_LAST    19      /**< Last Arduino pin */
/** @} */

/**
 * @name    Aliases for analog pins
 * @{
 */
#define ARDUINO_PIN_A0      ARDUINO_PIN_14  /**< Arduino pin A0 */
#define ARDUINO_PIN_A1      ARDUINO_PIN_15  /**< Arduino pin A1 */
#define ARDUINO_PIN_A2      ARDUINO_PIN_16  /**< Arduino pin A2 */
#define ARDUINO_PIN_A3      ARDUINO_PIN_17  /**< Arduino pin A3 */

#define ARDUINO_ANALOG_PIN_LAST 3           /**< Last analogue Arduino pin */
/** @} */

/**
 * @name    Mapping of Arduino analog pins to RIOT ADC lines
 * @{
 */
#define ARDUINO_A0          ADC_LINE(0)     /**< ADC line for Arduino pin A0 */
#define ARDUINO_A1          ADC_LINE(1)     /**< ADC line for Arduino pin A1 */
#define ARDUINO_A2          ADC_LINE(2)     /**< ADC line for Arduino pin A2 */
#define ARDUINO_A3          ADC_LINE(3)     /**< ADC line for Arduino pin A3 */
/** @} */

/**
 * @name    Mapping of Arduino pins to RIOT PWM dev and channel pairs
 * @{
 */
#define ARDUINO_PIN_3_PWM_DEV   PWM_DEV(0)  /**< PWM device for Arduino pin3 */
#define ARDUINO_PIN_3_PWM_CHAN  0           /**< PWM channel for Arduino pin3 */

#define ARDUINO_PIN_5_PWM_DEV   PWM_DEV(0)  /**< PWM device for Arduino pin5 */
#define ARDUINO_PIN_5_PWM_CHAN  1           /**< PWM channel for Arduino pin5 */

#define ARDUINO_PIN_6_PWM_DEV   PWM_DEV(0)  /**< PWM device for Arduino pin6 */
#define ARDUINO_PIN_6_PWM_CHAN  2           /**< PWM channel for Arduino pin6 */

#define ARDUINO_PIN_9_PWM_DEV   PWM_DEV(1)  /**< PWM device for Arduino pin9 */
#define ARDUINO_PIN_9_PWM_CHAN  1           /**< PWM channel for Arduino pin9 */

#define ARDUINO_PIN_10_PWM_DEV  PWM_DEV(1)  /**< PWM device for Arduino pin10 */
#define ARDUINO_PIN_10_PWM_CHAN 0           /**< PWM channel for Arduino pin10 */

#define ARDUINO_PIN_11_PWM_DEV  PWM_DEV(1)  /**< PWM device for Arduino pin11 */
#define ARDUINO_PIN_11_PWM_CHAN 2           /**< PWM channel for Arduino pin11 */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
