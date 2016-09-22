/*
 * Copyright (C) 2015 Freie Universität Berlin
 *               2016 Laurent Navet <laurent.navet@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_arduino-common
 * @{
 *
 * @file
 * @brief       Mapping from MCU pins to Arduino pins
 *
 * You can use the defines in this file for simplified interaction with the
 * Arduino specific pin numbers.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Daniel Nordahl <nordahl.d@gmail.com>
 * @author      Laurent Navet <laurent.navet@gmail.com>
 */

#ifndef ARDUINO_PINMAP_H
#define ARDUINO_PINMAP_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Mapping of MCU pins to Arduino pins
 *
 * @note    ISCP pins are not mapped.
 */

/* Digital pins */
#define ARDUINO_PIN_0           GPIO_PIN(PORT_D, 0)
#define ARDUINO_PIN_1           GPIO_PIN(PORT_D, 1)
#define ARDUINO_PIN_2           GPIO_PIN(PORT_D, 2)
#define ARDUINO_PIN_3           GPIO_PIN(PORT_D, 3)
#define ARDUINO_PIN_4           GPIO_PIN(PORT_D, 4)
#define ARDUINO_PIN_5           GPIO_PIN(PORT_D, 5)
#define ARDUINO_PIN_6           GPIO_PIN(PORT_D, 6)
#define ARDUINO_PIN_7           GPIO_PIN(PORT_D, 7)
#define ARDUINO_PIN_8           GPIO_PIN(PORT_B, 0)
#define ARDUINO_PIN_9           GPIO_PIN(PORT_B, 1)
#define ARDUINO_PIN_10          GPIO_PIN(PORT_B, 2)
#define ARDUINO_PIN_11          GPIO_PIN(PORT_B, 3)
#define ARDUINO_PIN_12          GPIO_PIN(PORT_B, 4)
#define ARDUINO_PIN_13          GPIO_PIN(PORT_B, 5)
/* Analog pins */
#define ARDUINO_PIN_14          GPIO_PIN(PORT_C, 0)
#define ARDUINO_PIN_15          GPIO_PIN(PORT_C, 1)
#define ARDUINO_PIN_16          GPIO_PIN(PORT_C, 2)
#define ARDUINO_PIN_17          GPIO_PIN(PORT_C, 3)
#define ARDUINO_PIN_18          GPIO_PIN(PORT_C, 4)
#define ARDUINO_PIN_19          GPIO_PIN(PORT_C, 5)
/* Analog input */
#define ARDUINO_PIN_A0          ARDUINO_PIN_14
#define ARDUINO_PIN_A1          ARDUINO_PIN_15
#define ARDUINO_PIN_A2          ARDUINO_PIN_16
#define ARDUINO_PIN_A3          ARDUINO_PIN_17
#define ARDUINO_PIN_A4          ARDUINO_PIN_18
#define ARDUINO_PIN_A5          ARDUINO_PIN_19
/** @ */

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_PINMAP_H */
/** @} */
