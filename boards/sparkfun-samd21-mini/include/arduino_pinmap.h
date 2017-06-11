/*
 * Copyright (C)  2016 Inria
 *                2017 Tom Keddie
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_sparkfun-samd-mini
 * @{
 *
 * @file
 * @brief       Mapping from MCU pins to Arduino pins
 *
 * You can use the defines in this file for simplified interaction with the
 * Arduino specific pin numbers.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Tom Keddie <github@bronwenandtom.com>
 */

#ifndef ARDUINO_PINMAP_H
#define ARDUINO_PINMAP_H

#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Mapping of MCU pins to Arduino pins
 * @{
 */
#define ARDUINO_PIN_0           GPIO_PIN(PA, 11)
#define ARDUINO_PIN_1           GPIO_PIN(PA, 10)
#define ARDUINO_PIN_2           GPIO_PIN(PA, 14)
#define ARDUINO_PIN_3           GPIO_PIN(PA, 9)
#define ARDUINO_PIN_4           GPIO_PIN(PA, 8)
#define ARDUINO_PIN_5           GPIO_PIN(PA, 15)
#define ARDUINO_PIN_6           GPIO_PIN(PA, 20)
#define ARDUINO_PIN_7           GPIO_PIN(PA, 21)

#define ARDUINO_PIN_8           GPIO_PIN(PA, 6)
#define ARDUINO_PIN_9           GPIO_PIN(PA, 7)
#define ARDUINO_PIN_10          GPIO_PIN(PA, 18)
#define ARDUINO_PIN_11          GPIO_PIN(PA, 16)
#define ARDUINO_PIN_12          GPIO_PIN(PA, 19)
#define ARDUINO_PIN_13          GPIO_PIN(PA, 17) /* on-board LED */

#define ARDUINO_PIN_A0          GPIO_PIN(PA, 2)
#define ARDUINO_PIN_A1          GPIO_PIN(PB, 8)
#define ARDUINO_PIN_A2          GPIO_PIN(PB, 9)
/** @ */

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_PINMAP_H */
/** @} */
