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
 * @name    Mapping of MCU pins to Arduino pins
 * @{
 */
#define ARDUINO_PIN_0           GPIO_PIN(PA, 22) /* TC4-W0 */
#define ARDUINO_PIN_1           GPIO_PIN(PA, 23) /* TC4-W1 */
#define ARDUINO_PIN_2           GPIO_PIN(PA, 10) /* TCC0-W2 */
#define ARDUINO_PIN_3           GPIO_PIN(PA, 11) /* TCC0-W3 */
#define ARDUINO_PIN_4           GPIO_PIN(PB, 10) /* TCC0-W4 */
#define ARDUINO_PIN_5           GPIO_PIN(PB, 11) /* TCC0-W5 */
#define ARDUINO_PIN_6           GPIO_PIN(PA, 20) /* TCC0-W6, on-board LED */
#define ARDUINO_PIN_7           GPIO_PIN(PA, 21) /* TCC0-W7 */

#define ARDUINO_PIN_8           GPIO_PIN(PA, 16) /* SERCOM1-MOSI */
#define ARDUINO_PIN_9           GPIO_PIN(PA, 17) /* SERCOM1-SCK */
#define ARDUINO_PIN_10          GPIO_PIN(PA, 19) /* SERCOM1-MISO */
#define ARDUINO_PIN_11          GPIO_PIN(PA, 8)  /* SERCOM0-SDA, on-board pull-up */
#define ARDUINO_PIN_12          GPIO_PIN(PA, 9)  /* SERCOM0-SCL, on-board pull-up */
#define ARDUINO_PIN_13          GPIO_PIN(PB, 23) /* SERCOM5-RX from MCU */
#define ARDUINO_PIN_14          GPIO_PIN(PB, 22) /* SERCOM5-TX from MCU */

#define ARDUINO_PIN_A0          GPIO_PIN(PA, 2)  /* AIN0, DAC0 */
#define ARDUINO_PIN_A1          GPIO_PIN(PB, 2)  /* AIN10 */
#define ARDUINO_PIN_A2          GPIO_PIN(PB, 3)  /* AIN11 */
#define ARDUINO_PIN_A3          GPIO_PIN(PA, 4)  /* AIN4 */
#define ARDUINO_PIN_A4          GPIO_PIN(PA, 5)  /* AIN5 */
#define ARDUINO_PIN_A5          GPIO_PIN(PA, 6)  /* AIN6 */
#define ARDUINO_PIN_A6          GPIO_PIN(PA, 7)  /* AIN7 */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_PINMAP_H */
/** @} */
