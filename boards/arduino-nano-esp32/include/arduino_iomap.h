/*
 * SPDX-FileCopyrightText: 2018 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_esp32_esp-wrover-kit
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
#define ARDUINO_PIN_0       GPIO44  /**< Arduino pin 0 (RxD) */
#define ARDUINO_PIN_1       GPIO43  /**< Arduino pin 1 (TxD) */

#define ARDUINO_PIN_2       GPIO5   /**< Arduino pin 2 (PWM) */
#define ARDUINO_PIN_3       GPIO6   /**< Arduino pin 3 (PWM) */
#define ARDUINO_PIN_4       GPIO7   /**< Arduino pin 4 (PWM) */
#define ARDUINO_PIN_5       GPIO8   /**< Arduino pin 5 (PWM) */
#define ARDUINO_PIN_6       GPIO9   /**< Arduino pin 6 (PWM) */
#define ARDUINO_PIN_7       GPIO10  /**< Arduino pin 7 (PWM) */
#define ARDUINO_PIN_8       GPIO17  /**< Arduino pin 8 (PWM) */
#define ARDUINO_PIN_9       GPIO18  /**< Arduino pin 9 (PWM) */

#define ARDUINO_PIN_10      GPIO21  /**< Arduino pin 10 (CS0)  */
#define ARDUINO_PIN_11      GPIO38  /**< Arduino pin 11 (MOSI) */
#define ARDUINO_PIN_12      GPIO47  /**< Arduino pin 12 (MISO) */
#define ARDUINO_PIN_13      GPIO48  /**< Arduino pin 13 (SCK)  */

#define ARDUINO_PIN_14      GPIO46  /**< Arduino pin 14 (LED red) */
#define ARDUINO_PIN_15      GPIO0   /**< Arduino pin 15 (LED green) */
#define ARDUINO_PIN_16      GPIO45  /**< Arduino pin 16 (LED blue) */

#define ARDUINO_PIN_17      GPIO1   /**< Arduino pin 17 (A0) */
#define ARDUINO_PIN_18      GPIO2   /**< Arduino pin 18 (A1) */
#define ARDUINO_PIN_19      GPIO3   /**< Arduino pin 19 (A2) */
#define ARDUINO_PIN_20      GPIO4   /**< Arduino pin 20 (A3) */
#define ARDUINO_PIN_21      GPIO11  /**< Arduino pin 21 (A4 / SDA) */
#define ARDUINO_PIN_22      GPIO12  /**< Arduino pin 22 (A5 / SCL) */
#define ARDUINO_PIN_23      GPIO13  /**< Arduino pin 23 (A6) */
#define ARDUINO_PIN_24      GPIO14  /**< Arduino pin 24 (A7) */

#define ARDUINO_PIN_LAST    24      /**< Last Arduino pin index */
/** @} */

/**
 * @name    Aliases for analog pins
 * @{
 */
#define ARDUINO_PIN_A0    ARDUINO_PIN_17  /**< Arduino pin A0 */
#define ARDUINO_PIN_A1    ARDUINO_PIN_18  /**< Arduino pin A1 */
#define ARDUINO_PIN_A2    ARDUINO_PIN_19  /**< Arduino pin A2 */
#define ARDUINO_PIN_A3    ARDUINO_PIN_20  /**< Arduino pin A3 */
#if !MODULE_PERIPH_I2C
#  define ARDUINO_PIN_A4  ARDUINO_PIN_21  /**< Arduino pin A4 */
#  define ARDUINO_PIN_A5  ARDUINO_PIN_22  /**< Arduino pin A5 */
#  define ARDUINO_PIN_A6  ARDUINO_PIN_23  /**< Arduino pin A6 */
#  define ARDUINO_PIN_A7  ARDUINO_PIN_24  /**< Arduino pin A7 */
#endif
/** @} */

/**
 * @name    Mapping of Arduino analog pins to RIOT ADC lines
 * @{
 */
#define ARDUINO_A0        ADC_LINE(0)     /**< ADC line for Arduino pin A0 */
#define ARDUINO_A1        ADC_LINE(1)     /**< ADC line for Arduino pin A1 */
#define ARDUINO_A2        ADC_LINE(2)     /**< ADC line for Arduino pin A2 */
#define ARDUINO_A3        ADC_LINE(3)     /**< ADC line for Arduino pin A3 */
#if !MODULE_PERIPH_I2C
#  define ARDUINO_A4      ADC_LINE(4)     /**< ADC line for Arduino pin A4 */
#  define ARDUINO_A5      ADC_LINE(5)     /**< ADC line for Arduino pin A5 */
#  define ARDUINO_A6      ADC_LINE(6)     /**< ADC line for Arduino pin A6 */
#  define ARDUINO_A7      ADC_LINE(7)     /**< ADC line for Arduino pin A7 */
#endif

#if !MODULE_PERIPH_I2C
#  define ARDUINO_ANALOG_PIN_LAST 7      /**< Last Arduino analog pin index */
#else
#  define ARDUINO_ANALOG_PIN_LAST 3      /**< Last Arduino analog pin index */
#endif
/** @} */

/**
 * @name    Mapping of Arduino pins to RIOT PWM dev and channel pairs
 * @{
 */
#define ARDUINO_PIN_2_PWM_DEV   PWM_DEV(0)  /**< PWM device for Arduino pin 2 */
#define ARDUINO_PIN_2_PWM_CHAN  0           /**< PWM channel for Arduino pin 2 */
#define ARDUINO_PIN_3_PWM_DEV   PWM_DEV(0)  /**< PWM device for Arduino pin 3 */
#define ARDUINO_PIN_3_PWM_CHAN  1           /**< PWM channel for Arduino pin 3 */
#define ARDUINO_PIN_4_PWM_DEV   PWM_DEV(0)  /**< PWM device for Arduino pin 4 */
#define ARDUINO_PIN_4_PWM_CHAN  2           /**< PWM channel for Arduino pin 4 */
#define ARDUINO_PIN_5_PWM_DEV   PWM_DEV(0)  /**< PWM device for Arduino pin 5 */
#define ARDUINO_PIN_5_PWM_CHAN  3           /**< PWM channel for Arduino pin 5 */
#define ARDUINO_PIN_6_PWM_DEV   PWM_DEV(0)  /**< PWM device for Arduino pin 6 */
#define ARDUINO_PIN_6_PWM_CHAN  4           /**< PWM channel for Arduino pin 6 */
#define ARDUINO_PIN_7_PWM_DEV   PWM_DEV(0)  /**< PWM device for Arduino pin 7 */
#define ARDUINO_PIN_7_PWM_CHAN  5           /**< PWM channel for Arduino pin 7 */
#define ARDUINO_PIN_8_PWM_DEV   PWM_DEV(0)  /**< PWM device for Arduino pin 8 */
#define ARDUINO_PIN_8_PWM_CHAN  6           /**< PWM channel for Arduino pin 8 */
#define ARDUINO_PIN_9_PWM_DEV   PWM_DEV(0)  /**< PWM device for Arduino pin 9 */
#define ARDUINO_PIN_9_PWM_CHAN  7           /**< PWM channel for Arduino pin 9 */
#define ARDUINO_PIN_14_PWM_DEV  PWM_DEV(1)  /**< PWM device for Arduino pin 14 */
#define ARDUINO_PIN_14_PWM_CHAN 0           /**< PWM channel for Arduino pin 14 */
#define ARDUINO_PIN_15_PWM_DEV  PWM_DEV(1)  /**< PWM device for Arduino pin 15 */
#define ARDUINO_PIN_15_PWM_CHAN 1           /**< PWM channel for Arduino pin 15 */
#define ARDUINO_PIN_16_PWM_DEV  PWM_DEV(1)  /**< PWM device for Arduino pin 16 */
#define ARDUINO_PIN_16_PWM_CHAN 2           /**< PWM channel for Arduino pin 16 */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
