/*
 * Copyright (C)  2016 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_arduino-mkr1000
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
#define ARDUINO_PIN_0           GPIO_PIN(PA, 22)
#define ARDUINO_PIN_1           GPIO_PIN(PA, 23)
#define ARDUINO_PIN_2           GPIO_PIN(PA, 10)
#define ARDUINO_PIN_3           GPIO_PIN(PA, 11)
#define ARDUINO_PIN_4           GPIO_PIN(PB, 10)
#define ARDUINO_PIN_5           GPIO_PIN(PB, 11)
#define ARDUINO_PIN_6           GPIO_PIN(PA, 20) /* on-board LED */
#define ARDUINO_PIN_7           GPIO_PIN(PA, 21)

#define ARDUINO_PIN_A0          GPIO_PIN(PA, 2)
#define ARDUINO_PIN_A1          GPIO_PIN(PB, 2)
#define ARDUINO_PIN_A2          GPIO_PIN(PB, 3)
#define ARDUINO_PIN_A3          GPIO_PIN(PA, 4)
#define ARDUINO_PIN_A4          GPIO_PIN(PA, 5)
#define ARDUINO_PIN_A5          GPIO_PIN(PA, 6)
#define ARDUINO_PIN_A6          GPIO_PIN(PA, 7)
/** @ */

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_PINMAP_H */
/** @} */
