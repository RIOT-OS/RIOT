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

#define ARDUINO_UART_D0D1       UART_DEV(0) /**< Arduino UART interface */
#define ARDUINO_SPI_D11D12D13   SPI_DEV(0)  /**< Arduino SPI bus */

#if !MODULE_ESP32_WROVER_KIT_CAMERA && !MODULE_PERIPH_SDMMC
#  define ARDUINO_LED           (5)         /**< LED is connected to Arduino pin 5 */
#endif

/**
 * @name   Mapping of MCU pins to Arduino pins
 * @{
 */
#define ARDUINO_PIN_0       GPIO3   /**< Arduino pin 0 (RxD) */
#define ARDUINO_PIN_1       GPIO1   /**< Arduino pin 1 (TxD) */

#define ARDUINO_PIN_2       GPIO19  /**< Arduino pin 2 */
#define ARDUINO_PIN_3       GPIO0   /**< Arduino pin 3 (PWM / BTN0) */
#define ARDUINO_PIN_4       GPIO22  /**< Arduino pin 4 */
#define ARDUINO_PIN_5       GPIO4   /**< Arduino pin 5 (PWM / LED2) */
#define ARDUINO_PIN_6       GPIO23  /**< Arduino pin 6 */
#define ARDUINO_PIN_7       GPIO25  /**< Arduino pin 7 */
#define ARDUINO_PIN_8       GPIO9   /**< Arduino pin 8 */
#define ARDUINO_PIN_9       GPIO10  /**< Arduino pin 9 */

#define ARDUINO_PIN_10      GPIO13  /**< Arduino pin 10 (CS0)  */
#define ARDUINO_PIN_11      GPIO15  /**< Arduino pin 11 (MOSI) */
#define ARDUINO_PIN_12      GPIO2   /**< Arduino pin 12 (MISO) */
#define ARDUINO_PIN_13      GPIO14  /**< Arduino pin 13 (SCK)  */

#define ARDUINO_PIN_14      GPIO34  /**< Arduino pin 14 (A0) */
#define ARDUINO_PIN_15      GPIO35  /**< Arduino pin 15 (A1) */
#define ARDUINO_PIN_16      GPIO36  /**< Arduino pin 16 (A2) */
#define ARDUINO_PIN_17      GPIO39  /**< Arduino pin 17 (A3) */

#define ARDUINO_PIN_18      GPIO26  /**< Arduino pin 18 (SDA) */
#define ARDUINO_PIN_19      GPIO27  /**< Arduino pin 19 (SCL) */

#define ARDUINO_PIN_LAST    19      /**< Last Arduino pin index */
/** @} */

#if !MODULE_ESP32_WROVER_KIT_CAMERA
/**
 * @name    Aliases for analog pins
 * Analog pins are only available if the CAM module is not used.
 * @{
 */
#  define ARDUINO_PIN_A0    ARDUINO_PIN_14  /**< Arduino pin A0 */
#  define ARDUINO_PIN_A1    ARDUINO_PIN_15  /**< Arduino pin A1 */
#  define ARDUINO_PIN_A2    ARDUINO_PIN_16  /**< Arduino pin A2 */
#  define ARDUINO_PIN_A3    ARDUINO_PIN_17  /**< Arduino pin A3 */
/** @} */

/**
 * @name    Mapping of Arduino analog pins to RIOT ADC lines
 * Analog pins are only available if the CAM module is not used.
 * @{
 */
#  define ARDUINO_A0        ADC_LINE(0)     /**< ADC line for Arduino pin A0 */
#  define ARDUINO_A1        ADC_LINE(1)     /**< ADC line for Arduino pin A1 */
#  define ARDUINO_A2        ADC_LINE(2)     /**< ADC line for Arduino pin A2 */
#  define ARDUINO_A3        ADC_LINE(3)     /**< ADC line for Arduino pin A3 */

#  define ARDUINO_ANALOG_PIN_LAST 3           /**< Last Arduino analog pin index */
/** @} */
#endif

/**
 * @name    Mapping of Arduino pins to RIOT PWM dev and channel pairs
 * @{
 */
#if !MODULE_ESP32_WROVER_KIT_CAMERA
#  define ARDUINO_PIN_3_PWM_DEV     PWM_DEV(1)  /**< PWM device for Arduino pin 3 */
#  define ARDUINO_PIN_3_PWM_CHAN    0           /**< PWM channel for Arduino pin 3 */
#  if !MODULE_PERIPH_SDMMC
#    define ARDUINO_PIN_5_PWM_DEV   PWM_DEV(1)  /**< PWM device for Arduino pin 5 */
#    define ARDUINO_PIN_5_PWM_CHAN  1           /**< PWM channel for Arduino pin 5 */
#  endif /* !MODULE_PERIPH_SDMMC */
#endif /* !MODULE_ESP32_WROVER_KIT_CAMERA */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
