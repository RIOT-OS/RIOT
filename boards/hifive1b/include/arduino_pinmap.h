/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_hifive1b
 * @{
 *
 * @file
 * @brief       Mapping from MCU pins to Arduino pins for the SiFive HiFive1b board
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
 * @name    Mapping of MCU pins to Arduino pins
 * @{
 */

/* Digital pins */
#define ARDUINO_PIN_0           GPIO_PIN(0, 16) /* UART0_RX */
#define ARDUINO_PIN_1           GPIO_PIN(0, 17) /* UART0_TX */
#define ARDUINO_PIN_2           GPIO_PIN(0, 18)
#define ARDUINO_PIN_3           GPIO_PIN(0, 19) /* PWM1 */
#define ARDUINO_PIN_4           GPIO_PIN(0, 20) /* PWM1 */
#define ARDUINO_PIN_5           GPIO_PIN(0, 21) /* PWM1 */
#define ARDUINO_PIN_6           GPIO_PIN(0, 22) /* PWM1 */
#define ARDUINO_PIN_7           GPIO_PIN(0, 23)
#define ARDUINO_PIN_8           GPIO_PIN(0, 0)  /* PWM0 */
#define ARDUINO_PIN_9           GPIO_PIN(0, 1)  /* PWM0 */
#define ARDUINO_PIN_10          GPIO_PIN(0, 2)  /* SPI SS/PWM0 */
#define ARDUINO_PIN_11          GPIO_PIN(0, 3)  /* SPI MOSI/PWM0 */
#define ARDUINO_PIN_12          GPIO_PIN(0, 4)  /* SPI MISO */
#define ARDUINO_PIN_13          GPIO_PIN(0, 5)  /* SPI SCK */
#define ARDUINO_PIN_14          GPIO_UNDEF
#define ARDUINO_PIN_15          GPIO_PIN(0, 9)
#define ARDUINO_PIN_16          GPIO_PIN(0, 10) /* PWM2 */
#define ARDUINO_PIN_17          GPIO_PIN(0, 11) /* PWM2 */
#define ARDUINO_PIN_18          GPIO_PIN(0, 12) /* SDA/PWM2 */
#define ARDUINO_PIN_19          GPIO_PIN(0, 13) /* SCL/PWM2 */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_PINMAP_H */
/** @} */
