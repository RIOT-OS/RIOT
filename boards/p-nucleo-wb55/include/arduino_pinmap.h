/*
 * Copyright (C)  2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_p-nucleo-wb55
 * @{
 *
 * @file
 * @brief       Mapping from MCU pins to Arduino pins
 *
 * You can use the defines in this file for simplified interaction with the
 * Arduino specific pin numbers.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 */

#ifndef ARDUINO_PINMAP_H
#define ARDUINO_PINMAP_H

#include "periph/gpio.h"
#include "periph/adc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Mapping of MCU pins to Arduino pins
 * @{
 */
#define ARDUINO_PIN_0           GPIO_PIN(PORT_A, 3)
#define ARDUINO_PIN_1           GPIO_PIN(PORT_A, 2)
#define ARDUINO_PIN_2           GPIO_PIN(PORT_C, 6)
#define ARDUINO_PIN_3           GPIO_PIN(PORT_A, 10)
#define ARDUINO_PIN_4           GPIO_PIN(PORT_C, 10)
#define ARDUINO_PIN_5           GPIO_PIN(PORT_A, 15)
#define ARDUINO_PIN_6           GPIO_PIN(PORT_A, 8)
#define ARDUINO_PIN_7           GPIO_PIN(PORT_C, 13)
#define ARDUINO_PIN_8           GPIO_PIN(PORT_C, 12)
#define ARDUINO_PIN_9           GPIO_PIN(PORT_A, 9)
#define ARDUINO_PIN_10          GPIO_PIN(PORT_A, 4)
#define ARDUINO_PIN_11          GPIO_PIN(PORT_A, 7)
#define ARDUINO_PIN_12          GPIO_PIN(PORT_A, 6)
#define ARDUINO_PIN_13          GPIO_PIN(PORT_A, 5)
#define ARDUINO_PIN_14          GPIO_PIN(PORT_B, 9)
#define ARDUINO_PIN_15          GPIO_PIN(PORT_B, 8)

#define ARDUINO_PIN_A0          GPIO_PIN(PORT_C, 0)
#define ARDUINO_PIN_A1          GPIO_PIN(PORT_C, 1)
#define ARDUINO_PIN_A2          GPIO_PIN(PORT_A, 1)
#define ARDUINO_PIN_A3          GPIO_PIN(PORT_A, 0)
#define ARDUINO_PIN_A4          GPIO_PIN(PORT_C, 3)
#define ARDUINO_PIN_A5          GPIO_PIN(PORT_C, 2)
/** @ */

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
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_PINMAP_H */
/** @} */
