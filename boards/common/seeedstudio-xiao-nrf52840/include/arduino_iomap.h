/*
 * SPDX-FileCopyrightText: 2024 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_seeedstudio-xiao-nrf52840
 * @ingroup     boards_seeedstudio-xiao-nrf52840-sense
 * @{
 *
 * @file
 * @brief       Mapping from MCU pins to Arduino pins
 *
 * You can use the defines in this file for simplified interaction with the
 * Arduino specific pin numbers.
 *
 * @author      Mikolai Gütschow <mikolai.guetschow@tu-dresden.de>
 */

#include "periph/gpio.h"
#include "periph/adc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    XIAO's UART devices
 * @{
 * @brief   Arduino's Serial uses USB-CDC-ACM stdio by default
 */
#define ARDUINO_UART_DEV    UART_UNDEF  /**< The Xiao nRF52840 uses USB-CDC-ACM by default */
/** @} */

/**
 * @name    XIAO's SPI buses
 * @{
 */
#define ARDUINO_SPI_DEV     SPI_DEV(0)  /**< Standard SPI bus for general purpose use */
/** @} */

/**
 * @name    XIAO's I2C buses
 * @{
 */
#define ARDUINO_I2C_DEV     I2C_DEV(0)  /**< Standard I2C bus for general purpose use */
/** @} */

/**
 * @name    XIAO's on-board LED (LED_BUILTIN)
 * @{
 */
#define ARDUINO_LED     (11)            /**< Use the Red LED */
/** @} */

/**
 * @name   Mapping of MCU pins to Arduino pins
 * @{
 */
/* Left pins */
#define ARDUINO_PIN_0       GPIO_PIN(0,  2) /**< Pin Definition for D0 */
#define ARDUINO_PIN_1       GPIO_PIN(0,  3) /**< Pin Definition for D1 */
#define ARDUINO_PIN_2       GPIO_PIN(0, 28) /**< Pin Definition for D2 */
#define ARDUINO_PIN_3       GPIO_PIN(0, 29) /**< Pin Definition for D3 */
#define ARDUINO_PIN_4       GPIO_PIN(0,  4) /**< Pin Definition for D4 */
#define ARDUINO_PIN_5       GPIO_PIN(0,  5) /**< Pin Definition for D5 */
#define ARDUINO_PIN_6       GPIO_PIN(1, 11) /**< Pin Definition for D6 */

/* Right side */
#define ARDUINO_PIN_7       GPIO_PIN(1, 12) /**< Pin Definition for D7 */
#define ARDUINO_PIN_8       GPIO_PIN(1, 13) /**< Pin Definition for D8 */
#define ARDUINO_PIN_9       GPIO_PIN(1, 14) /**< Pin Definition for D9 */
#define ARDUINO_PIN_10      GPIO_PIN(1, 15) /**< Pin Definition for D10 */

/* Internal (LEDs) */
#define ARDUINO_PIN_11      GPIO_PIN(0, 26) /**< Pin Definition for Red LED */
#define ARDUINO_PIN_12      GPIO_PIN(0,  6) /**< Pin Definition for Blue LED */
#define ARDUINO_PIN_13      GPIO_PIN(0, 30) /**< Pin Definition for Green LED */

#define ARDUINO_PIN_LAST    13              /**< Last Arduino Pin */
/** @} */

/**
 * @name    Aliases for analog pins
 * @{
 */
#define ARDUINO_PIN_A0      ARDUINO_PIN_0   /**< Pin Definition for A0 */
#define ARDUINO_PIN_A1      ARDUINO_PIN_1   /**< Pin Definition for A1 */
#define ARDUINO_PIN_A2      ARDUINO_PIN_2   /**< Pin Definition for A2 */
#define ARDUINO_PIN_A3      ARDUINO_PIN_3   /**< Pin Definition for A3 */
#define ARDUINO_PIN_A4      ARDUINO_PIN_4   /**< Pin Definition for A4 */
#define ARDUINO_PIN_A5      ARDUINO_PIN_5   /**< Pin Definition for A5 */
/** @} */

/**
 * @name    Mapping of Arduino analog pins to RIOT ADC lines
 * @{
 */
/* The Seeed Studio XIAO nRF52840 has a fixed ADC to GPIO mapping:
 *
 * nRF  | MCU pin   | Exposed as Arduino pin
 * -----|-----------|-----------------------
 * AIN0 | P0.02     | A0 (D0)
 * AIN1 | P0.03     | A1 (D1)
 * AIN2 | P0.04     | A4 (D4)
 * AIN3 | P0.05     | A5 (D5)
 * AIN4 | P0.28     | A2 (D2)
 * AIN5 | P0.29     | A3 (D3)
 * AIN6 | P0.30     | - (D13)
 * AIN7 | P0.31     | - (-)
 */
#define ARDUINO_A0          ADC_LINE(0)     /**< ADC Channel 0 for A0 */
#define ARDUINO_A1          ADC_LINE(1)     /**< ADC Channel 1 for A1 */
#define ARDUINO_A2          ADC_LINE(4)     /**< ADC Channel 4 for A2 */
#define ARDUINO_A3          ADC_LINE(5)     /**< ADC Channel 5 for A3 */
#define ARDUINO_A4          ADC_LINE(2)     /**< ADC Channel 2 for A4 */
#define ARDUINO_A5          ADC_LINE(3)     /**< ADC Channel 3 for A5 */

#define ARDUINO_ANALOG_PIN_LAST 5           /**< Last Analog Input Pin */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
