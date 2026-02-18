/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_pro-micro-nrf52840
 * @{
 *
 * @file
 * @brief       Mapping from MCU pins to Arduino pins for Pro Micro nRF52840 boards
 *
 * You can use the defines in this file for simplified interaction with the
 * Arduino specific pin numbers.
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#include "periph/adc.h"
#include "periph/gpio.h"
#include "periph/i2c.h"
#include "periph/spi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Pro Micro's UART devices
 * @{
 */
#define ARDUINO_UART_DEV    UART_UNDEF  /**< Board uses USB-CDC-ACM by default */
/** @} */

/**
 * @name    Pro Micro's SPI buses
 * @{
 */
#define ARDUINO_SPI_DEV     SPI_DEV(0)  /**< Standard SPI bus for general purpose use */
/** @} */

/**
 * @name    Pro Micro's I2C buses
 * @{
 */
#define ARDUINO_I2C_DEV     I2C_DEV(0)  /**< Standard I2C bus for general purpose use */
/** @} */

/**
 * @name    Pro Micro's on-board LED (LED_BUILTIN)
 * @{
 */
#define ARDUINO_LED         (13)        /**< Internal LED pin (not on header) */
/** @} */

/**
 * @name   Mapping of MCU pins to Arduino pins
 * @{
 */
#define ARDUINO_PIN_0       GPIO_PIN(0,  8)     /**< Pin definition for D0 (RX) */
#define ARDUINO_PIN_1       GPIO_PIN(0,  6)     /**< Pin definition for D1 (TX) */
#define ARDUINO_PIN_2       GPIO_PIN(0, 17)     /**< Pin definition for D2 (SDA) */
#define ARDUINO_PIN_3       GPIO_PIN(0, 20)     /**< Pin definition for D3 (SCL) */
#define ARDUINO_PIN_4       GPIO_PIN(0, 22)     /**< Pin definition for D4 */
#define ARDUINO_PIN_5       GPIO_PIN(0, 24)     /**< Pin definition for D5 */
#define ARDUINO_PIN_6       GPIO_PIN(1,  0)     /**< Pin definition for D6 */
#define ARDUINO_PIN_7       GPIO_PIN(0, 11)     /**< Pin definition for D7 */
#define ARDUINO_PIN_8       GPIO_PIN(1,  4)     /**< Pin definition for D8 */
#define ARDUINO_PIN_9       GPIO_PIN(1,  6)     /**< Pin definition for D9 */
#define ARDUINO_PIN_10      GPIO_PIN(0,  9)     /**< Pin definition for D10 */
#define ARDUINO_PIN_11      GPIO_UNDEF          /**< Pin undefined */
#define ARDUINO_PIN_12      GPIO_UNDEF          /**< Pin undefined */
#define ARDUINO_PIN_13      GPIO_PIN(0, 15)     /**< Pin definition for D13 (LED) */
#define ARDUINO_PIN_14      GPIO_PIN(1, 11)     /**< Pin definition for D14 (MISO) */
#define ARDUINO_PIN_15      GPIO_PIN(1, 13)     /**< Pin definition for D15 (SCLK) */
#define ARDUINO_PIN_16      GPIO_PIN(0, 10)     /**< Pin definition for D11 (MOSI) */
#define ARDUINO_PIN_17      GPIO_UNDEF          /**< Pin undefined */
#define ARDUINO_PIN_18      GPIO_PIN(1, 15)     /**< Pin definition for D18 */
#define ARDUINO_PIN_19      GPIO_PIN(0, 2)      /**< Pin definition for D19 */
#define ARDUINO_PIN_20      GPIO_PIN(0, 29)     /**< Pin definition for D20 */
#define ARDUINO_PIN_21      GPIO_PIN(0, 31)     /**< Pin definition for D21 */

#define ARDUINO_PIN_LAST    21                  /**< Last Arduino Pin */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
