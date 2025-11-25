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

#ifndef ARDUINO_IOMAP_H
#define ARDUINO_IOMAP_H

#include "periph/gpio.h"
#include "periph/adc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   Mapping of MCU pins to Arduino pins
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
#define ARDUINO_PIN_14          GPIO_PIN(0, 3)
#define ARDUINO_PIN_15          GPIO_PIN(0, 4)
#define ARDUINO_PIN_16          GPIO_PIN(0, 28)
#define ARDUINO_PIN_17          GPIO_PIN(0, 29)
#define ARDUINO_PIN_18          GPIO_PIN(0, 30)
#define ARDUINO_PIN_19          GPIO_PIN(0, 31)
#define ARDUINO_PIN_20          GPIO_PIN(0, 13)
#define ARDUINO_PIN_21          GPIO_PIN(0, 14)
#define ARDUINO_PIN_22          GPIO_PIN(1, 9)
#define ARDUINO_PIN_23          GPIO_PIN(0, 16)

#define ARDUINO_PIN_LAST        23
/** @} */

/**
 * @name    Aliases for analog pins
 * @{
 */
#define ARDUINO_PIN_A0          ARDUINO_PIN_14
#define ARDUINO_PIN_A1          ARDUINO_PIN_15
#define ARDUINO_PIN_A2          ARDUINO_PIN_16
#define ARDUINO_PIN_A3          ARDUINO_PIN_17
#define ARDUINO_PIN_A4          ARDUINO_PIN_18
#define ARDUINO_PIN_A5          ARDUINO_PIN_19
/** @} */

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

#define ARDUINO_ANALOG_PIN_LAST 5
/** @} */

/**
 * @name    Mapping of Arduino pins to RIOT PWM dev and channel pairs
 * @{
 */
/**
 * @brief   PWM frequency
 */
#define ARDUINO_PWM_FREQU       (15625U)

#define ARDUINO_PIN_3_PWM_DEV   PWM_DEV(0)
#define ARDUINO_PIN_3_PWM_CHAN  0

#define ARDUINO_PIN_5_PWM_DEV   PWM_DEV(0)
#define ARDUINO_PIN_5_PWM_CHAN  1

#define ARDUINO_PIN_6_PWM_DEV   PWM_DEV(0)
#define ARDUINO_PIN_6_PWM_CHAN  2

#define ARDUINO_PIN_9_PWM_DEV   PWM_DEV(0)
#define ARDUINO_PIN_9_PWM_CHAN  3

#define ARDUINO_PIN_20_PWM_DEV  PWM_DEV(1)
#define ARDUINO_PIN_20_PWM_CHAN 0

#define ARDUINO_PIN_21_PWM_DEV  PWM_DEV(1)
#define ARDUINO_PIN_21_PWM_CHAN 1

#define ARDUINO_PIN_22_PWM_DEV  PWM_DEV(1)
#define ARDUINO_PIN_22_PWM_CHAN 2

#define ARDUINO_PIN_23_PWM_DEV  PWM_DEV(1)
#define ARDUINO_PIN_23_PWM_CHAN 3
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_IOMAP_H */
/** @} */
