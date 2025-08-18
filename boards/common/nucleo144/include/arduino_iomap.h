/*
 * SPDX-FileCopyrightText: 2017 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_common_nucleo144
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

#include "periph/gpio.h"
#include "periph/adc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Mapping of MCU pins to Arduino pins
 * @{
 */
#if defined(CPU_MODEL_STM32F303ZE)
#  define ARDUINO_PIN_0         GPIO_PIN(PORT_C, 5)
#  define ARDUINO_PIN_1         GPIO_PIN(PORT_C, 4)
#elif defined(CPU_FAM_STM32L4) || defined(CPU_FAM_STM32L5)
#  define ARDUINO_PIN_0         GPIO_PIN(PORT_D, 9)
#  define ARDUINO_PIN_1         GPIO_PIN(PORT_D, 8)
#elif defined(CPU_FAM_STM32U5)
#  define ARDUINO_PIN_0         GPIO_PIN(PORT_G, 8)
#  define ARDUINO_PIN_1         GPIO_PIN(PORT_G, 7)
#else
#  define ARDUINO_PIN_0         GPIO_PIN(PORT_G, 9)
#  define ARDUINO_PIN_1         GPIO_PIN(PORT_G, 14)
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

#define ARDUINO_PIN_14          GPIO_PIN(PORT_B, 9)
#define ARDUINO_PIN_15          GPIO_PIN(PORT_B, 8)

#if defined(CPU_FAM_STM32L5) || defined(CPU_FAM_STM32U5)
#  define ARDUINO_PIN_16        GPIO_PIN(PORT_A, 3)
#  define ARDUINO_PIN_17        GPIO_PIN(PORT_A, 2)
#  define ARDUINO_PIN_18        GPIO_PIN(PORT_C, 3)
#  define ARDUINO_PIN_19        GPIO_PIN(PORT_B, 0)
#  define ARDUINO_PIN_20        GPIO_PIN(PORT_C, 1)
#  define ARDUINO_PIN_21        GPIO_PIN(PORT_C, 0)
#else
#  define ARDUINO_PIN_16        GPIO_PIN(PORT_A, 3)
#  define ARDUINO_PIN_17        GPIO_PIN(PORT_C, 0)
#  define ARDUINO_PIN_18        GPIO_PIN(PORT_C, 3)
#  if defined(CPU_MODEL_STM32F413ZH) || defined(CPU_MODEL_STM32F412ZG) || \
      defined(CPU_FAM_STM32L4)
#    define ARDUINO_PIN_19      GPIO_PIN(PORT_C, 1)
#    define ARDUINO_PIN_20      GPIO_PIN(PORT_C, 4)
#    define ARDUINO_PIN_21      GPIO_PIN(PORT_C, 5)
#  elif defined(CPU_MODEL_STM32F303ZE)
#    define ARDUINO_PIN_19      GPIO_PIN(PORT_D, 11)
#    define ARDUINO_PIN_20      GPIO_PIN(PORT_D, 12)
#    define ARDUINO_PIN_21      GPIO_PIN(PORT_D, 13)
#  else
#    define ARDUINO_PIN_19      GPIO_PIN(PORT_F, 3)
#    define ARDUINO_PIN_20      GPIO_PIN(PORT_F, 5)
#    define ARDUINO_PIN_21      GPIO_PIN(PORT_F, 10)
#  endif
#endif

#define ARDUINO_PIN_LAST        21
/** @} */

/**
 * @name    Aliases for analog pins
 * @{
 */
#define ARDUINO_PIN_A0          ARDUINO_PIN_16
#define ARDUINO_PIN_A1          ARDUINO_PIN_17
#define ARDUINO_PIN_A2          ARDUINO_PIN_18
#define ARDUINO_PIN_A3          ARDUINO_PIN_19
#define ARDUINO_PIN_A4          ARDUINO_PIN_20
#define ARDUINO_PIN_A5          ARDUINO_PIN_21
/** @} */

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

#define ARDUINO_ANALOG_PIN_LAST 5
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
