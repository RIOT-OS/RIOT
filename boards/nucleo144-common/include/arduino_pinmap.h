/*
 * Copyright (C)  2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nucleo144-common
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
#if defined(CPU_MODEL_STM32F303ZE)
#define ARDUINO_PIN_0           GPIO_PIN(PORT_C, 5)
#define ARDUINO_PIN_1           GPIO_PIN(PORT_C, 4)
#else
#define ARDUINO_PIN_0           GPIO_PIN(PORT_G, 9)
#define ARDUINO_PIN_1           GPIO_PIN(PORT_G, 14)
#endif
#define ARDUINO_PIN_2           GPIO_PIN(PORT_F, 15)
#define ARDUINO_PIN_3           GPIO_PIN(PORT_E, 13)
#define ARDUINO_PIN_4           GPIO_PIN(PORT_F, 14)
#define ARDUINO_PIN_5           GPIO_PIN(PORT_E, 11)
#define ARDUINO_PIN_6           GPIO_PIN(PORT_E, 9)
#define ARDUINO_PIN_7           GPIO_PIN(PORT_F, 13)

#define ARDUINO_PIN_8           GPIO_PIN(PORT_F, 12)
#define ARDUINO_PIN_9           GPIO_PIN(PORT_D, 15)
#define ARDUINO_PIN_10          GPIO_PIN(PORT_D, 14)
#define ARDUINO_PIN_11          GPIO_PIN(PORT_A, 7)
#define ARDUINO_PIN_12          GPIO_PIN(PORT_A, 6)
#define ARDUINO_PIN_13          GPIO_PIN(PORT_A, 5)

#define ARDUINO_PIN_14          GPIO_PIN(PORT_B, 8)
#define ARDUINO_PIN_15          GPIO_PIN(PORT_B, 9)

#define ARDUINO_PIN_A0          GPIO_PIN(PORT_A, 3)
#define ARDUINO_PIN_A1          GPIO_PIN(PORT_C, 0)
#define ARDUINO_PIN_A2          GPIO_PIN(PORT_C, 3)
#if defined(CPU_MODEL_STM32F413ZH)
#define ARDUINO_PIN_A3          GPIO_PIN(PORT_C, 1)
#define ARDUINO_PIN_A4          GPIO_PIN(PORT_C, 4)
#define ARDUINO_PIN_A5          GPIO_PIN(PORT_C, 5)
#elif defined(CPU_MODEL_STM32F303ZE)
#define ARDUINO_PIN_A3          GPIO_PIN(PORT_D, 11)
#define ARDUINO_PIN_A4          GPIO_PIN(PORT_D, 12)
#define ARDUINO_PIN_A5          GPIO_PIN(PORT_D, 13)
#else
#define ARDUINO_PIN_A3          GPIO_PIN(PORT_F, 3)
#define ARDUINO_PIN_A4          GPIO_PIN(PORT_F, 5)
#define ARDUINO_PIN_A5          GPIO_PIN(PORT_F, 10)
#endif
/** @ */

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_PINMAP_H */
/** @} */
