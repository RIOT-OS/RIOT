/*
 * Copyright (C) 2020 J. David Ibáñez <jdavid.ibp@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup    boards_remote-revb
 * @{
 *
 * @file
 * @brief       Configuration of the Arduino API for Remote revision B board
 *
 * You can use the defines in this file for simplified interaction with the
 * Arduino specific pin numbers.
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
 */
#define ARDUINO_LED         (1)

/**
 * @brief   Look-up table for the Arduino's digital pins
 */
static const gpio_t arduino_pinmap[] = {
    GPIO_UNDEF,
    ARDUINO_PIN_1,
    ARDUINO_PIN_2,
    ARDUINO_PIN_3,
    ARDUINO_PIN_4,
    ARDUINO_PIN_5,
    ARDUINO_PIN_6,
    ARDUINO_PIN_7,
    ARDUINO_PIN_8,
    GPIO_UNDEF,
    GPIO_UNDEF,
    ARDUINO_PIN_11,
    ARDUINO_PIN_12,
    ARDUINO_PIN_13,
    ARDUINO_PIN_14,
    GPIO_UNDEF,
    GPIO_UNDEF,
    ARDUINO_PIN_17,
    GPIO_UNDEF,
    GPIO_UNDEF,
    GPIO_UNDEF,
    ARDUINO_PIN_21,
    ARDUINO_PIN_22,
    ARDUINO_PIN_23,
    ARDUINO_PIN_24,
    ARDUINO_PIN_25,
    GPIO_UNDEF,
    GPIO_UNDEF,
    ARDUINO_PIN_28,
    ARDUINO_PIN_29,
    GPIO_UNDEF,
    GPIO_UNDEF,
    ARDUINO_PIN_32,
    ARDUINO_PIN_33,
    ARDUINO_PIN_34,
    ARDUINO_PIN_35,
    ARDUINO_PIN_36,
};

/**
 * @brief   Look-up table for the Arduino's analog pins
 */
static const adc_t arduino_analog_map[] = {
    ADC_UNDEF,
    ARDUINO_A1,
    ARDUINO_A2,
    ARDUINO_A3,
    ARDUINO_A4,
    ARDUINO_A5,
};

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_BOARD_H */
/** @} */
