/*
 * SPDX-FileCopyrightText: 2015 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2016 Laurent Navet <laurent.navet@gmail.com>
 * SPDX-FileCopyrightText: 2017 Thomas Perrot <thomas.perrot@tupi.fr>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_common_arduino-atmega
 * @{
 *
 * @file
 * @brief       Mapping from MCU pins to Arduino pins for Arduino Atmega boards
 *
 * You can use the defines in this file for simplified interaction with the
 * Arduino specific pin numbers.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Daniel Nordahl <nordahl.d@gmail.com>
 * @author      Laurent Navet <laurent.navet@gmail.com>
 * @author      Thomas Perrot <thomas.perrot@tupi.fr>
 */

#include "buildinfo/board.h"
#include "buildinfo/cpu.h"
#include "periph/adc.h"
#include "periph/gpio.h"
#include "periph/i2c.h"
#include "periph/pwm.h"
#include "periph/spi.h"
#include "periph/uart.h"
#include "periph_conf.h" /* For ADC_NUMOF */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Arduino's UART devices
 * @{
 */
#define ARDUINO_UART_D0D1        UART_DEV(0)
/** @} */

/**
 * @name    Arduino's SPI buses
 * @{
 */
/**
 * @brief   The only hardware SPI is connected to the ISP header
 */
#define ARDUINO_SPI_ISP         SPI_DEV(0)
/** @} */

/**
 * @name    Arduino's I2C buses
 * @{
 */
/**
 * @brief   The only hardware I2C on ATmegas
 */
#ifndef BOARD_ARDUINO_NANO
#define ARDUINO_I2C_UNO         I2C_DEV(0)
#else
#define ARDUINO_I2C_NANO        I2C_DEV(0)
#endif
/** @} */

/**
 * @name    Mapping of MCU pins to Arduino pins
 *
 * @note    ISCP pins are not mapped.
 * @{
 */

/* Digital pins */
#ifdef CPU_ATMEGA328P
#  define ARDUINO_PIN_0         GPIO_PIN(PORT_D, 0)
#  define ARDUINO_PIN_1         GPIO_PIN(PORT_D, 1)
#  define ARDUINO_PIN_2         GPIO_PIN(PORT_D, 2)
#  define ARDUINO_PIN_3         GPIO_PIN(PORT_D, 3)
#  define ARDUINO_PIN_4         GPIO_PIN(PORT_D, 4)
#  define ARDUINO_PIN_5         GPIO_PIN(PORT_D, 5)
#  define ARDUINO_PIN_6         GPIO_PIN(PORT_D, 6)
#  define ARDUINO_PIN_7         GPIO_PIN(PORT_D, 7)
#  define ARDUINO_PIN_8         GPIO_PIN(PORT_B, 0)
#  define ARDUINO_PIN_9         GPIO_PIN(PORT_B, 1)
#  define ARDUINO_PIN_10        GPIO_PIN(PORT_B, 2)
#  define ARDUINO_PIN_11        GPIO_PIN(PORT_B, 3)
#  define ARDUINO_PIN_12        GPIO_PIN(PORT_B, 4)
#  define ARDUINO_PIN_13        GPIO_PIN(PORT_B, 5)
/* Analog pins as digital pins: */
#  define ARDUINO_PIN_14        GPIO_PIN(PORT_C, 0)
#  define ARDUINO_PIN_15        GPIO_PIN(PORT_C, 1)
#  define ARDUINO_PIN_16        GPIO_PIN(PORT_C, 2)
#  define ARDUINO_PIN_17        GPIO_PIN(PORT_C, 3)
#  define ARDUINO_PIN_18        GPIO_PIN(PORT_C, 4)
#  define ARDUINO_PIN_19        GPIO_PIN(PORT_C, 5)

#  define ARDUINO_PIN_LAST      19

/* Analog aliases */
#  define ARDUINO_PIN_A0        ARDUINO_PIN_14
#  define ARDUINO_PIN_A1        ARDUINO_PIN_15
#  define ARDUINO_PIN_A2        ARDUINO_PIN_16
#  define ARDUINO_PIN_A3        ARDUINO_PIN_17
#  define ARDUINO_PIN_A4        ARDUINO_PIN_18
#  define ARDUINO_PIN_A5        ARDUINO_PIN_19
#endif

#ifdef CPU_ATMEGA32U4
/* Digital pins */
#  define ARDUINO_PIN_0         GPIO_PIN(PORT_D, 2)
#  define ARDUINO_PIN_1         GPIO_PIN(PORT_D, 3)
#  define ARDUINO_PIN_2         GPIO_PIN(PORT_D, 1)
#  define ARDUINO_PIN_3         GPIO_PIN(PORT_D, 0)
#  define ARDUINO_PIN_5         GPIO_PIN(PORT_C, 6)
#  define ARDUINO_PIN_7         GPIO_PIN(PORT_E, 6)
#  define ARDUINO_PIN_11        GPIO_PIN(PORT_B, 7)
#  define ARDUINO_PIN_13        GPIO_PIN(PORT_C, 7)
#  define ARDUINO_PIN_14        GPIO_PIN(PORT_B, 3)
#  define ARDUINO_PIN_15        GPIO_PIN(PORT_B, 1)
#  define ARDUINO_PIN_16        GPIO_PIN(PORT_B, 2)
#  define ARDUINO_PIN_17        GPIO_PIN(PORT_B, 0)
#  define ARDUINO_PIN_30        GPIO_PIN(PORT_D, 5)

/* Analog pins as digital pins: */
#  define ARDUINO_PIN_4         GPIO_PIN(PORT_D, 4)
#  define ARDUINO_PIN_6         GPIO_PIN(PORT_D, 7)
#  define ARDUINO_PIN_8         GPIO_PIN(PORT_B, 4)
#  define ARDUINO_PIN_9         GPIO_PIN(PORT_B, 5)
#  define ARDUINO_PIN_10        GPIO_PIN(PORT_B, 6)
#  define ARDUINO_PIN_12        GPIO_PIN(PORT_D, 6)
#  define ARDUINO_PIN_18        GPIO_PIN(PORT_F, 7)
#  define ARDUINO_PIN_19        GPIO_PIN(PORT_F, 6)
#  define ARDUINO_PIN_20        GPIO_PIN(PORT_F, 5)
#  define ARDUINO_PIN_21        GPIO_PIN(PORT_F, 4)
#  define ARDUINO_PIN_22        GPIO_PIN(PORT_F, 3)
#  define ARDUINO_PIN_23        GPIO_PIN(PORT_F, 2)

#  define ARDUINO_PIN_LAST      23

/* Analog aliases */
#  define ARDUINO_PIN_A0        ARDUINO_PIN_18
#  define ARDUINO_PIN_A1        ARDUINO_PIN_19
#  define ARDUINO_PIN_A2        ARDUINO_PIN_20
#  define ARDUINO_PIN_A3        ARDUINO_PIN_21
#  define ARDUINO_PIN_A4        ARDUINO_PIN_22
#  define ARDUINO_PIN_A5        ARDUINO_PIN_23
#  define ARDUINO_PIN_A6        ARDUINO_PIN_4
#  define ARDUINO_PIN_A7        ARDUINO_PIN_6
#  define ARDUINO_PIN_A8        ARDUINO_PIN_8
#  define ARDUINO_PIN_A9        ARDUINO_PIN_9
#  define ARDUINO_PIN_A10       ARDUINO_PIN_10
#  define ARDUINO_PIN_A11       ARDUINO_PIN_12
#endif

#ifdef CPU_ATMEGA2560
/* Digital pins */
#  define ARDUINO_PIN_0         GPIO_PIN(PORT_E, 0)
#  define ARDUINO_PIN_1         GPIO_PIN(PORT_E, 1)
#  define ARDUINO_PIN_2         GPIO_PIN(PORT_E, 4)
#  define ARDUINO_PIN_3         GPIO_PIN(PORT_E, 5)
#  define ARDUINO_PIN_4         GPIO_PIN(PORT_G, 5)
#  define ARDUINO_PIN_5         GPIO_PIN(PORT_E, 3)
#  define ARDUINO_PIN_6         GPIO_PIN(PORT_H, 3)
#  define ARDUINO_PIN_7         GPIO_PIN(PORT_H, 4)
#  define ARDUINO_PIN_8         GPIO_PIN(PORT_H, 5)
#  define ARDUINO_PIN_9         GPIO_PIN(PORT_H, 6)
#  define ARDUINO_PIN_10        GPIO_PIN(PORT_B, 4)
#  define ARDUINO_PIN_11        GPIO_PIN(PORT_B, 5)
#  define ARDUINO_PIN_12        GPIO_PIN(PORT_B, 6)
#  define ARDUINO_PIN_13        GPIO_PIN(PORT_B, 7)
#  define ARDUINO_PIN_14        GPIO_PIN(PORT_J, 1)
#  define ARDUINO_PIN_15        GPIO_PIN(PORT_J, 0)
#  define ARDUINO_PIN_16        GPIO_PIN(PORT_H, 1)
#  define ARDUINO_PIN_17        GPIO_PIN(PORT_H, 0)
#  define ARDUINO_PIN_18        GPIO_PIN(PORT_D, 3)
#  define ARDUINO_PIN_19        GPIO_PIN(PORT_D, 2)
#  define ARDUINO_PIN_20        GPIO_PIN(PORT_D, 1)
#  define ARDUINO_PIN_21        GPIO_PIN(PORT_D, 0)
#  define ARDUINO_PIN_22        GPIO_PIN(PORT_A, 0)
#  define ARDUINO_PIN_23        GPIO_PIN(PORT_A, 1)
#  define ARDUINO_PIN_24        GPIO_PIN(PORT_A, 2)
#  define ARDUINO_PIN_25        GPIO_PIN(PORT_A, 3)
#  define ARDUINO_PIN_26        GPIO_PIN(PORT_A, 4)
#  define ARDUINO_PIN_27        GPIO_PIN(PORT_A, 5)
#  define ARDUINO_PIN_28        GPIO_PIN(PORT_A, 6)
#  define ARDUINO_PIN_29        GPIO_PIN(PORT_A, 7)
#  define ARDUINO_PIN_30        GPIO_PIN(PORT_C, 7)
#  define ARDUINO_PIN_31        GPIO_PIN(PORT_C, 6)
#  define ARDUINO_PIN_32        GPIO_PIN(PORT_C, 5)
#  define ARDUINO_PIN_33        GPIO_PIN(PORT_C, 4)
#  define ARDUINO_PIN_34        GPIO_PIN(PORT_C, 3)
#  define ARDUINO_PIN_35        GPIO_PIN(PORT_C, 2)
#  define ARDUINO_PIN_36        GPIO_PIN(PORT_C, 1)
#  define ARDUINO_PIN_37        GPIO_PIN(PORT_E, 0)
#  define ARDUINO_PIN_38        GPIO_PIN(PORT_D, 7)
#  define ARDUINO_PIN_39        GPIO_PIN(PORT_G, 2)
#  define ARDUINO_PIN_40        GPIO_PIN(PORT_G, 1)
#  define ARDUINO_PIN_41        GPIO_PIN(PORT_G, 0)
#  define ARDUINO_PIN_42        GPIO_PIN(PORT_L, 7)
#  define ARDUINO_PIN_43        GPIO_PIN(PORT_L, 6)
#  define ARDUINO_PIN_44        GPIO_PIN(PORT_L, 5)
#  define ARDUINO_PIN_45        GPIO_PIN(PORT_L, 4)
#  define ARDUINO_PIN_46        GPIO_PIN(PORT_L, 3)
#  define ARDUINO_PIN_47        GPIO_PIN(PORT_L, 2)
#  define ARDUINO_PIN_48        GPIO_PIN(PORT_L, 1)
#  define ARDUINO_PIN_49        GPIO_PIN(PORT_L, 0)
#  define ARDUINO_PIN_50        GPIO_PIN(PORT_B, 3)
#  define ARDUINO_PIN_51        GPIO_PIN(PORT_B, 2)
#  define ARDUINO_PIN_52        GPIO_PIN(PORT_B, 1)
#  define ARDUINO_PIN_53        GPIO_PIN(PORT_B, 0)

/* Analog pins as digital pins: */
#  define ARDUINO_PIN_54        GPIO_PIN(PORT_F, 0)
#  define ARDUINO_PIN_55        GPIO_PIN(PORT_F, 1)
#  define ARDUINO_PIN_56        GPIO_PIN(PORT_F, 2)
#  define ARDUINO_PIN_57        GPIO_PIN(PORT_F, 3)
#  define ARDUINO_PIN_58        GPIO_PIN(PORT_F, 4)
#  define ARDUINO_PIN_59        GPIO_PIN(PORT_F, 5)
#  define ARDUINO_PIN_60        GPIO_PIN(PORT_F, 6)
#  define ARDUINO_PIN_61        GPIO_PIN(PORT_F, 7)
#  define ARDUINO_PIN_62        GPIO_PIN(PORT_K, 0)
#  define ARDUINO_PIN_63        GPIO_PIN(PORT_K, 1)
#  define ARDUINO_PIN_64        GPIO_PIN(PORT_K, 2)
#  define ARDUINO_PIN_65        GPIO_PIN(PORT_K, 3)
#  define ARDUINO_PIN_66        GPIO_PIN(PORT_K, 4)
#  define ARDUINO_PIN_67        GPIO_PIN(PORT_K, 5)
#  define ARDUINO_PIN_68        GPIO_PIN(PORT_K, 6)
#  define ARDUINO_PIN_69        GPIO_PIN(PORT_K, 7)

#  define ARDUINO_PIN_LAST      69

/* Analog aliases */
#  define ARDUINO_PIN_A0        ARDUINO_PIN_54
#  define ARDUINO_PIN_A1        ARDUINO_PIN_55
#  define ARDUINO_PIN_A2        ARDUINO_PIN_56
#  define ARDUINO_PIN_A3        ARDUINO_PIN_57
#  define ARDUINO_PIN_A4        ARDUINO_PIN_58
#  define ARDUINO_PIN_A5        ARDUINO_PIN_59
#  define ARDUINO_PIN_A6        ARDUINO_PIN_60
#  define ARDUINO_PIN_A7        ARDUINO_PIN_61
#  define ARDUINO_PIN_A8        ARDUINO_PIN_62
#  define ARDUINO_PIN_A9        ARDUINO_PIN_63
#  define ARDUINO_PIN_A10       ARDUINO_PIN_64
#  define ARDUINO_PIN_A11       ARDUINO_PIN_65
#  define ARDUINO_PIN_A12       ARDUINO_PIN_66
#  define ARDUINO_PIN_A13       ARDUINO_PIN_67
#  define ARDUINO_PIN_A14       ARDUINO_PIN_68
#  define ARDUINO_PIN_A15       ARDUINO_PIN_69
#endif

#if ADC_NUMOF >= 8
#  define ARDUINO_A0            ADC_LINE(0)
#  define ARDUINO_A1            ADC_LINE(1)
#  define ARDUINO_A2            ADC_LINE(2)
#  define ARDUINO_A3            ADC_LINE(3)
#  define ARDUINO_A4            ADC_LINE(4)
#  define ARDUINO_A5            ADC_LINE(5)
#  define ARDUINO_A6            ADC_LINE(6)
#  define ARDUINO_A7            ADC_LINE(7)
#endif
#if ADC_NUMOF >= 16
#  define ARDUINO_A8            ADC_LINE(8)
#  define ARDUINO_A9            ADC_LINE(9)
#  define ARDUINO_A10           ADC_LINE(10)
#  define ARDUINO_A11           ADC_LINE(11)
#  define ARDUINO_A12           ADC_LINE(12)
#  define ARDUINO_A13           ADC_LINE(13)
#  define ARDUINO_A14           ADC_LINE(14)
#  define ARDUINO_A15           ADC_LINE(15)
#endif

/* Either 8, 16 or 0 ADC lines are configured by all ATmega boards */
#if ADC_NUMOF == 8
#  define ARDUINO_ANALOG_PIN_LAST 7
#elif ADC_NUMOF == 16
#  define ARDUINO_ANALOG_PIN_LAST 15
#endif
/** @} */

/**
 * @name    Mapping of Arduino pins to RIOT PWM dev and channel pairs
 * @{
 */
/**
 * @brief   PWM frequency
 */
#define ARDUINO_PWM_FREQU       (490U)

/* keep in sync with PWM config in boards/common/atmega/include/periph_conf_atmega_common.h */
#if defined(CPU_ATMEGA328P)
#  define ARDUINO_PIN_6_PWM_DEV     PWM_DEV(0)
#  define ARDUINO_PIN_6_PWM_CHAN    0

#  define ARDUINO_PIN_5_PWM_DEV     PWM_DEV(0)
#  define ARDUINO_PIN_5_PWM_CHAN    1

#  define ARDUINO_PIN_11_PWM_DEV    PWM_DEV(1)
#  define ARDUINO_PIN_11_PWM_CHAN   0

#  define ARDUINO_PIN_3_PWM_DEV     PWM_DEV(1)
#  define ARDUINO_PIN_3_PWM_CHAN    1
#elif defined(CPU_ATMEGA2560)
#  define ARDUINO_PIN_13_PWM_DEV    PWM_DEV(0)
#  define ARDUINO_PIN_13_PWM_CHAN   0

#  define ARDUINO_PIN_4_PWM_DEV     PWM_DEV(0)
#  define ARDUINO_PIN_4_PWM_CHAN    1

#  define ARDUINO_PIN_10_PWM_DEV    PWM_DEV(1)
#  define ARDUINO_PIN_10_PWM_CHAN   0

#  define ARDUINO_PIN_9_PWM_DEV     PWM_DEV(1)
#  define ARDUINO_PIN_9_PWM_CHAN    1
#elif defined(CPU_ATMEGA32U4)
#  define ARDUINO_PIN_11_PWM_DEV    PWM_DEV(0)
#  define ARDUINO_PIN_11_PWM_CHAN   0

#  define ARDUINO_PIN_3_PWM_DEV     PWM_DEV(0)
#  define ARDUINO_PIN_3_PWM_CHAN    1
#endif

/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
