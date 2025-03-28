/*
 * Copyright (C) 2024 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_seeedstudio-xiao-nrf52840
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

#ifndef ARDUINO_IOMAP_H
#define ARDUINO_IOMAP_H

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
#define ARDUINO_UART_DEV    UART_UNDEF
/** @} */

/**
 * @name    XIAO's SPI buses
 * @{
 */
#define ARDUINO_SPI_DEV     SPI_DEV(0)
/** @} */

/**
 * @name    XIAO's I2C buses
 * @{
 */
#define ARDUINO_I2C_DEV     I2C_DEV(0)
/** @} */

/**
 * @name    XIAO's on-board LED (LED_BUILTIN)
 * @{
 */
#define ARDUINO_LED     (11)
/** @} */

/**
 * @name   Mapping of MCU pins to Arduino pins
 * @{
 */
/* Left pins */
#define ARDUINO_PIN_0       GPIO_PIN(0,  2)
#define ARDUINO_PIN_1       GPIO_PIN(0,  3)
#define ARDUINO_PIN_2       GPIO_PIN(0, 28)
#define ARDUINO_PIN_3       GPIO_PIN(0, 29)
#define ARDUINO_PIN_4       GPIO_PIN(0,  4)
#define ARDUINO_PIN_5       GPIO_PIN(0,  5)
#define ARDUINO_PIN_6       GPIO_PIN(1, 11)

/* Right side */
#define ARDUINO_PIN_7       GPIO_PIN(1, 12)
#define ARDUINO_PIN_8       GPIO_PIN(1, 13)
#define ARDUINO_PIN_9       GPIO_PIN(1, 14)
#define ARDUINO_PIN_10      GPIO_PIN(1, 15)

/* Internal (LEDs) */
#define ARDUINO_PIN_11      GPIO_PIN(0, 26)
#define ARDUINO_PIN_12      GPIO_PIN(0,  6)
#define ARDUINO_PIN_13      GPIO_PIN(0, 30)

#define ARDUINO_PIN_LAST    13
/** @} */

/**
 * @name    Aliases for analog pins
 * @{
 */
#define ARDUINO_PIN_A0      ARDUINO_PIN_0
#define ARDUINO_PIN_A1      ARDUINO_PIN_1
#define ARDUINO_PIN_A2      ARDUINO_PIN_2
#define ARDUINO_PIN_A3      ARDUINO_PIN_3
#define ARDUINO_PIN_A4      ARDUINO_PIN_4
#define ARDUINO_PIN_A5      ARDUINO_PIN_5
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
#define ARDUINO_A0          ADC_LINE(0)
#define ARDUINO_A1          ADC_LINE(1)
#define ARDUINO_A2          ADC_LINE(4)
#define ARDUINO_A3          ADC_LINE(5)
#define ARDUINO_A4          ADC_LINE(2)
#define ARDUINO_A5          ADC_LINE(3)

#define ARDUINO_ANALOG_PIN_LAST 5
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_IOMAP_H */
/** @} */
