/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_arduino-atmega-common
 * @{
 *
 * @file
 * @brief       Board specific configuration for the Arduino API
 *
 * @author      Hauke Petersen  <hauke.petersen@fu-berlin.de>
 * @author      Laurent Navet   <laurent.navet@gmail.com>
 */

#ifndef ARDUINO_BOARD_H
#define ARDUINO_BOARD_H

#include "arduino_pinmap.h"

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
#ifdef CPU_ATMEGA2560
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

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_BOARD_H */
/** @} */
