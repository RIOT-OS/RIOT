/*
 * SPDX-FileCopyrightText: 2017 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_stm32f4discovery
 * @{
 *
 * @file
 * @brief       Mapping from MCU pins to Arduino pins
 *
 * You can use the defines in this file for simplified interaction with the
 * Arduino specific pin numbers.
 *
 * @author      Sebastian Meiling <s@mlng.net>
 */

#include "periph/gpio.h"
#include "periph/adc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   Mapping of MCU pins to Arduino pins
 * @{
 */
#define ARDUINO_PIN_0           GPIO_PIN(PORT_D, 12)
#define ARDUINO_PIN_1           GPIO_PIN(PORT_D, 13)
#define ARDUINO_PIN_2           GPIO_PIN(PORT_D, 14)
#define ARDUINO_PIN_3           GPIO_PIN(PORT_D, 15)
#define ARDUINO_PIN_4           GPIO_PIN(PORT_A, 12)
#define ARDUINO_PIN_5           GPIO_PIN(PORT_A, 15)
#define ARDUINO_PIN_6           GPIO_PIN(PORT_B, 1)
#define ARDUINO_PIN_7           GPIO_PIN(PORT_B, 2)

#define ARDUINO_PIN_LAST        7
/** @} */

/**
 * @name    Mapping of Arduino analog pins to RIOT ADC lines
 * @{
 */
#define ARDUINO_A0              ADC_LINE(0)
#define ARDUINO_A1              ADC_LINE(1)
#define ARDUINO_A2              ADC_LINE(2)
#define ARDUINO_A3              ADC_LINE(3)

#define ARDUINO_ANALOG_PIN_LAST 3
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
