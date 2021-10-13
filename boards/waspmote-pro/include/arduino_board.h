/*
 * Copyright (C) 2020 J. David Ibáñez <jdavid.ibp@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_waspmote-pro
 * @{
 *
 * @file
 * @brief       Configuration of the Arduino API for Waspmote Pro board
 *
 * @author      J. David Ibáñez <jdavid.ibp@gmail.com>
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
 *
 * The waspmote-pro board has 2 programmable LEDs: green connected to pin 13
 * (aka LED1), and red connected to pin 12 (aka LED0).
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
    ARDUINO_PIN_20,
    ARDUINO_PIN_21,
    ARDUINO_PIN_22,
    ARDUINO_PIN_23,
    ARDUINO_PIN_24,
    ARDUINO_PIN_25,
    ARDUINO_PIN_26,
    ARDUINO_PIN_27,
    ARDUINO_PIN_28,
    ARDUINO_PIN_29,
    ARDUINO_PIN_30,
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
};

/**
 * @brief   PWM frequency
 */
#define ARDUINO_PWM_FREQU       (490U)

/**
 * @brief   List of PWM GPIO mappings
 */
static const arduino_pwm_t arduino_pwm_list[] = {
};

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_BOARD_H */
/** @} */
