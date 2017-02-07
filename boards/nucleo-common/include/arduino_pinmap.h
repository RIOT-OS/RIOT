/*
 * Copyright (C)  2016 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nucleo-common
 * @{
 *
 * @file
 * @brief       Mapping from MCU pins to Arduino pins
 *
 * You can use the defines in this file for simplified interaction with the
 * Arduino specific pin numbers.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef ARDUINO_PINMAP_H
#define ARDUINO_PINMAP_H

#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Mapping of MCU pins to Arduino pins
 * @{
 */
#define ARDUINO_PIN_0           GPIO_PIN(PORT_A, 3)
#define ARDUINO_PIN_1           GPIO_PIN(PORT_A, 2)
#define ARDUINO_PIN_2           GPIO_PIN(PORT_A, 10)
#define ARDUINO_PIN_3           GPIO_PIN(PORT_B, 3)
#define ARDUINO_PIN_4           GPIO_PIN(PORT_B, 5)
#define ARDUINO_PIN_5           GPIO_PIN(PORT_B, 4)
#define ARDUINO_PIN_6           GPIO_PIN(PORT_B, 10)
#define ARDUINO_PIN_7           GPIO_PIN(PORT_A, 8)

#define ARDUINO_PIN_8           GPIO_PIN(PORT_A, 9)
#define ARDUINO_PIN_9           GPIO_PIN(PORT_C, 7)
#define ARDUINO_PIN_10          GPIO_PIN(PORT_B, 6)
#define ARDUINO_PIN_11          GPIO_PIN(PORT_A, 7)
#define ARDUINO_PIN_12          GPIO_PIN(PORT_A, 6)
#define ARDUINO_PIN_13          GPIO_PIN(PORT_A, 5) /* on-board LED */

#define ARDUINO_PIN_A0          GPIO_PIN(PORT_C, 0)
#define ARDUINO_PIN_A1          GPIO_PIN(PORT_C, 1)
#define ARDUINO_PIN_A2          GPIO_PIN(PORT_B, 0)
#define ARDUINO_PIN_A3          GPIO_PIN(PORT_A, 4)
#define ARDUINO_PIN_A4          GPIO_PIN(PORT_A, 1)
#define ARDUINO_PIN_A5          GPIO_PIN(PORT_A, 0)
/** @ */

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_PINMAP_H */
/** @} */
