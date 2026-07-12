/*
 * SPDX-FileCopyrightText: 2024 TU Dresden
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_seeedstudio-xiao-esp32c6
 * @{
 *
 * @file
 * @brief       Mapping from MCU pins to Arduino pins
 *
 * @author      Mikolai Gütschow <mikolai.guetschow@tu-dresden.de>
 * @author      Stepan Konoplev <stepan.konoplev@haw-hamburg.de>
 */

#include "periph/gpio.h"
#include "periph/adc.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @name    XIAO's UART buses
 * @{
 */
#define ARDUINO_UART_D0D1   UART_DEV(0) /**< Standard UART bus for general purpose use */
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
 * @name    XIAO's USER LED (LED_BUILTIN)
 * @{
 */
#define ARDUINO_LED     (15)            /**< Use the USER LED */
/** @} */

/**
 * @name   Mapping of MCU pins to Arduino pins
 * @{
 */
/* Left pins */
#define ARDUINO_PIN_0       GPIO0   /**< Pin Definition for D0 */
#define ARDUINO_PIN_1       GPIO1   /**< Pin Definition for D1 */
#define ARDUINO_PIN_2       GPIO2   /**< Pin Definition for D2 */
#define ARDUINO_PIN_3       GPIO21  /**< Pin Definition for D3 */
#define ARDUINO_PIN_4       GPIO22  /**< Pin Definition for D4 */
#define ARDUINO_PIN_5       GPIO23  /**< Pin Definition for D5 */
#define ARDUINO_PIN_6       GPIO16  /**< Pin Definition for D6 */

/* Right side */
#define ARDUINO_PIN_7       GPIO17  /**< Pin Definition for D7 */
#define ARDUINO_PIN_8       GPIO19  /**< Pin Definition for D8 */
#define ARDUINO_PIN_9       GPIO20  /**< Pin Definition for D9 */
#define ARDUINO_PIN_10      GPIO18  /**< Pin Definition for D10 */

#define ARDUINO_PIN_LAST    10      /**< Last Arduino pin index. */
/** @} */

/**
 * @name    Aliases for analog pins
 * @{
 */
#define ARDUINO_PIN_A0      ARDUINO_PIN_0   /**< Arduino pin A0 */
#define ARDUINO_PIN_A1      ARDUINO_PIN_1   /**< Arduino pin A1 */
#define ARDUINO_PIN_A2      ARDUINO_PIN_2   /**< Arduino pin A2 */
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
/** @} */
