/*
 * Copyright (C)  2020 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_waveshare_nrf52840_eval_kit
 * @{
 *
 * @file
 * @brief       Mapping from MCU pins to Arduino pins
 *
 * You can use the defines in this file for simplified interaction with the
 * Arduino specific pin numbers.
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
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
#define ARDUINO_PIN_0           GPIO_PIN(1, 1)
#define ARDUINO_PIN_1           GPIO_PIN(1, 2)
#define ARDUINO_PIN_2           GPIO_PIN(1, 3)
#define ARDUINO_PIN_3           GPIO_PIN(1, 4)
#define ARDUINO_PIN_4           GPIO_PIN(1, 5)
#define ARDUINO_PIN_5           GPIO_PIN(1, 6)
#define ARDUINO_PIN_6           GPIO_PIN(1, 7)
#define ARDUINO_PIN_7           GPIO_PIN(1, 8)
#define ARDUINO_PIN_8           GPIO_PIN(1, 10)
#define ARDUINO_PIN_9           GPIO_PIN(1, 11)
#define ARDUINO_PIN_10          GPIO_PIN(1, 12)
#define ARDUINO_PIN_11          GPIO_PIN(1, 13)
#define ARDUINO_PIN_12          GPIO_PIN(1, 14)
#define ARDUINO_PIN_13          GPIO_PIN(1, 15)
#define ARDUINO_PIN_A0          GPIO_PIN(0, 3)
#define ARDUINO_PIN_A1          GPIO_PIN(0, 4)
#define ARDUINO_PIN_A2          GPIO_PIN(0, 28)
#define ARDUINO_PIN_A3          GPIO_PIN(0, 29)
#define ARDUINO_PIN_A4          GPIO_PIN(0, 30)
#define ARDUINO_PIN_A5          GPIO_PIN(0, 31)
#define ARDUINO_PIN_20          GPIO_PIN(0, 13)
#define ARDUINO_PIN_21          GPIO_PIN(0, 14)
#define ARDUINO_PIN_22          GPIO_PIN(1, 9)
#define ARDUINO_PIN_23          GPIO_PIN(0, 16)

/** @ */

/**
 * @name    Mapping of Arduino analog pins to RIOT ADC lines
 * @{
 */
#define ARDUINO_A0              ADC_LINE(1)
#define ARDUINO_A1              ADC_LINE(2)
#define ARDUINO_A2              ADC_LINE(4)
#define ARDUINO_A3              ADC_LINE(5)
#define ARDUINO_A4              ADC_LINE(6)
#define ARDUINO_A5              ADC_LINE(7)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_PINMAP_H */
/** @} */
