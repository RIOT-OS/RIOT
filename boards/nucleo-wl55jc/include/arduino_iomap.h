/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nucleo-wl55jc
 * @{
 *
 * @file
 * @brief       Mapping from MCU pins to Arduino pins
 *
 * You can use the defines in this file for simplified interaction with the
 * Arduino specific pin numbers.
 *
 * @author      Akshai M <akshai.m@fu-berlin.de>
 *
 */

#ifndef ARDUINO_IOMAP_H
#define ARDUINO_IOMAP_H

#include "periph/gpio.h"
#include "periph/adc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Arduino's I2C buses
 * @{
 */
/**
 * @brief   The first I2C bus is where shields for the Arduino UNO expect it
 */
#define ARDUINO_I2C_UNO         I2C_DEV(0)
/** @} */

/**
 * @brief   Mapping of MCU pins to Arduino pins
 * @{
 */
#define ARDUINO_PIN_0           GPIO_PIN(PORT_B, 7)
#define ARDUINO_PIN_1           GPIO_PIN(PORT_B, 6)
#define ARDUINO_PIN_2           GPIO_PIN(PORT_B, 12)
#define ARDUINO_PIN_3           GPIO_PIN(PORT_B, 3)
#define ARDUINO_PIN_4           GPIO_PIN(PORT_B, 5)
#define ARDUINO_PIN_5           GPIO_PIN(PORT_B, 8)
#define ARDUINO_PIN_6           GPIO_PIN(PORT_B, 10)
#define ARDUINO_PIN_7           GPIO_PIN(PORT_C, 1)
#define ARDUINO_PIN_8           GPIO_PIN(PORT_C, 2)
#define ARDUINO_PIN_9           GPIO_PIN(PORT_A, 9)
#define ARDUINO_PIN_10          GPIO_PIN(PORT_A, 4)
#define ARDUINO_PIN_11          GPIO_PIN(PORT_A, 7)
#define ARDUINO_PIN_12          GPIO_PIN(PORT_A, 6)
#define ARDUINO_PIN_13          GPIO_PIN(PORT_A, 5)
#define ARDUINO_PIN_14          GPIO_PIN(PORT_A, 11)
#define ARDUINO_PIN_15          GPIO_PIN(PORT_A, 12)

/* analog pins in digital mode: */
#define ARDUINO_PIN_16          GPIO_PIN(PORT_B, 1)
#define ARDUINO_PIN_17          GPIO_PIN(PORT_B, 2)
#define ARDUINO_PIN_18          GPIO_PIN(PORT_A, 10)
#define ARDUINO_PIN_19          GPIO_PIN(PORT_B, 4)
#define ARDUINO_PIN_20          GPIO_PIN(PORT_B, 14)
#define ARDUINO_PIN_21          GPIO_PIN(PORT_B, 13)

#define ARDUINO_PIN_LAST        21
/** @} */

/**
 * @brief   Aliases for analog pins
 * @{
 */
#define ARDUINO_PIN_A0          ARDUINO_PIN_16
#define ARDUINO_PIN_A1          ARDUINO_PIN_17
#define ARDUINO_PIN_A2          ARDUINO_PIN_18
#define ARDUINO_PIN_A3          ARDUINO_PIN_19
#define ARDUINO_PIN_A4          ARDUINO_PIN_20
#define ARDUINO_PIN_A5          ARDUINO_PIN_21
/** @} */

/**
 * @name    Mapping of Arduino analog pins to RIOT ADC lines
 * @{
 */
#define ARDUINO_A0              ADC_LINE(0)
#define ARDUINO_A1              ADC_LINE(1)
#define ARDUINO_A2              ADC_LINE(2)
#define ARDUINO_A3              ADC_LINE(3)
#define ARDUINO_A4              ADC_LINE(4)
#define ARDUINO_A5              ADC_LINE(5)

#define ARDUINO_ANALOG_PIN_LAST 5
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_IOMAP_H */
/** @} */
