/*
 * Copyright (C) 2021 J. David Ibáñez <jdavid.ibp@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_feather-m0
 * @{
 *
 * @file
 * @brief       Board specific configuration for the Arduino API
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
#define ARDUINO_LED         (13)

/**
 * @brief   On-board serial port mapping, stdio is used for Serial
 */
#ifndef ARDUINO_UART_DEV
#define ARDUINO_UART_DEV    UART_UNDEF
#endif

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
    ADC_UNDEF,
    ARDUINO_A7,
};

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_BOARD_H */
/** @} */
