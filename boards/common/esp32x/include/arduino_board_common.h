/*
 * Copyright (C)  2023 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_esp32x
 * @{
 *
 * @file
 * @brief       Definitions for the Arduino API that are common for all ESP32x boards
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef ARDUINO_BOARD_COMMON_H
#define ARDUINO_BOARD_COMMON_H

#include "arduino_pinmap.h"

#ifdef __cplusplus
extern "C"
{
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
    ARDUINO_PIN_A0,
    ARDUINO_PIN_A1,
    ARDUINO_PIN_A2,
    ARDUINO_PIN_A3,
    ARDUINO_PIN_A4,
    ARDUINO_PIN_A5
};

/**
 * @brief   Look-up table for the Arduino's analog pins
 */
static const adc_t arduino_analog_map[] = {
    ARDUINO_PIN_A0,
    ARDUINO_PIN_A1,
    ARDUINO_PIN_A2,
    ARDUINO_PIN_A3,
    ARDUINO_PIN_A4,
    ARDUINO_PIN_A5
};

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* ARDUINO_BOARD_COMMON_H */
