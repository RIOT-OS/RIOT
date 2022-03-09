/*
 * Copyright (C) 2016 Freie Universität Berlin
 *               2017 Thomas Perrot <thomas.perrot@tupi.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup    boards_common_arduino-atmega
 * @{
 *
 * @file
 * @brief       Configuration of the Arduino API for Arduino Atmega boards
 *
 * @author      Hauke Petersen  <hauke.petersen@fu-berlin.de>
 * @author      Laurent Navet   <laurent.navet@gmail.com>
 * @author      Thomas Perrot   <thomas.perrot@tupi.fr>
 */

#ifndef ARDUINO_BOARD_H
#define ARDUINO_BOARD_H

#include "arduino_pinmap.h"
#include "periph/pwm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The on-board LED is connected to pin 13 on this board
 */
#define ARDUINO_LED         (13)

/**
 * @brief   Look-up table for the Arduino's digital pins
 */
static const gpio_t arduino_pinmap[] = {
    ARDUINO_PIN_0,
    ARDUINO_PIN_1,
    ARDUINO_PIN_2,
    ARDUINO_PIN_3,
    ARDUINO_PIN_4,
    ARDUINO_PIN_5,
    ARDUINO_PIN_6,
    ARDUINO_PIN_7,
    ARDUINO_PIN_8,
    ARDUINO_PIN_9,
    ARDUINO_PIN_10,
    ARDUINO_PIN_11,
    ARDUINO_PIN_12,
    ARDUINO_PIN_13,
    ARDUINO_PIN_14,
    ARDUINO_PIN_15,
    ARDUINO_PIN_16,
    ARDUINO_PIN_17,
    ARDUINO_PIN_18,
    ARDUINO_PIN_19,
#if defined(CPU_ATMEGA2560) || defined(CPU_ATMEGA32U4)
    ARDUINO_PIN_20,
    ARDUINO_PIN_21,
    ARDUINO_PIN_22,
    ARDUINO_PIN_23,
#endif
#ifdef CPU_ATMEGA2560
    ARDUINO_PIN_24,
    ARDUINO_PIN_25,
    ARDUINO_PIN_26,
    ARDUINO_PIN_27,
    ARDUINO_PIN_28,
    ARDUINO_PIN_29,
#endif
#if defined(CPU_ATMEGA2560) || defined(CPU_ATMEGA32U4)
    ARDUINO_PIN_30,
#endif
#ifdef CPU_ATMEGA2560
    ARDUINO_PIN_31,
    ARDUINO_PIN_32,
    ARDUINO_PIN_33,
    ARDUINO_PIN_34,
    ARDUINO_PIN_35,
    ARDUINO_PIN_36,
    ARDUINO_PIN_37,
    ARDUINO_PIN_38,
    ARDUINO_PIN_39,
    ARDUINO_PIN_40,
    ARDUINO_PIN_41,
    ARDUINO_PIN_42,
    ARDUINO_PIN_43,
    ARDUINO_PIN_44,
    ARDUINO_PIN_45,
    ARDUINO_PIN_46,
    ARDUINO_PIN_47,
    ARDUINO_PIN_48,
    ARDUINO_PIN_49,
    ARDUINO_PIN_50,
    ARDUINO_PIN_51,
    ARDUINO_PIN_52,
    ARDUINO_PIN_53,
    ARDUINO_PIN_54,
    ARDUINO_PIN_55,
    ARDUINO_PIN_56,
    ARDUINO_PIN_57,
    ARDUINO_PIN_58,
    ARDUINO_PIN_59,
    ARDUINO_PIN_60,
    ARDUINO_PIN_61,
    ARDUINO_PIN_62,
    ARDUINO_PIN_63,
    ARDUINO_PIN_64,
    ARDUINO_PIN_65,
    ARDUINO_PIN_66,
    ARDUINO_PIN_67,
    ARDUINO_PIN_68,
    ARDUINO_PIN_69
#endif
};

/**
 * @brief   Look-up table for the Arduino's analog pins
 */
static const adc_t arduino_analog_map[] = {
    ARDUINO_A0,
    ARDUINO_A1,
    ARDUINO_A2,
    ARDUINO_A3,
    ARDUINO_A4,
    ARDUINO_A5,
    ARDUINO_A6,
    ARDUINO_A7,
#ifdef CPU_ATMEGA2560
    ARDUINO_A8,
    ARDUINO_A9,
    ARDUINO_A10,
    ARDUINO_A11,
    ARDUINO_A12,
    ARDUINO_A13,
    ARDUINO_A14,
    ARDUINO_A15,
#endif
};

/**
 * @brief   PWM frequency
 */
#define ARDUINO_PWM_FREQU       (490U)

/**
 * @brief   List of PWM GPIO mappings
 */
static const arduino_pwm_t arduino_pwm_list[] = {
#if defined(CPU_ATMEGA2560)
    { .pin = 13, .dev = PWM_DEV(0), .chan = 0 },
    { .pin = 4,  .dev = PWM_DEV(0), .chan = 1 },
#elif defined(CPU_ATMEGA32U4)
    { .pin = 11, .dev = PWM_DEV(0), .chan = 0 },
    { .pin = 3,  .dev = PWM_DEV(0), .chan = 1 },
#else /* CPU_ATMEGA328p */
    { .pin = 6, .dev = PWM_DEV(0), .chan = 0 },
    { .pin = 5, .dev = PWM_DEV(0), .chan = 1 },
#endif
#if defined(CPU_ATMEGA2560)
    { .pin = 10, .dev = PWM_DEV(1), .chan = 0 },
    { .pin = 9,  .dev = PWM_DEV(1), .chan = 1 },
#else /* CPU_ATMEGA328p */
    { .pin = 11, .dev = PWM_DEV(1), .chan = 0 },
    { .pin = 3,  .dev = PWM_DEV(1), .chan = 1 },
#endif
};

/**
 * @brief F_CPU defines the CPU frequency in Hz.
 *
 * This is used in AVR's libc delay.h and setbaud.h
 *
 * In RIOT delay() has a different implementation using ztimer, and F_CPU is
 * already defined when using setbaud.h (see cpu/atmega_common/periph/uart.c)
 *
 * However Arduino libraries and sketches may expect F_CPU to be defined and
 * fail otherwise (for example the Arduino SDI-12 package expects this, for AVR
 * cpus). For this reason we define F_CPU here, if not already defined.
 */
#ifndef F_CPU
#define F_CPU CLOCK_CORECLOCK
#endif

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_BOARD_H */
/** @} */
