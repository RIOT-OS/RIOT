/*
 * Copyright (C)  2018 Federico Pellegrin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_saml21-xpro
 * @brief       Board configuration for the Arduino API
 * @file
 * @author      Federico Pellegrin <fede@evolware.org>
 * @{
 */

#ifndef ARDUINO_BOARD_H
#define ARDUINO_BOARD_H

#include "arduino_pinmap.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Arduino's digital pins mappings
 */
static const gpio_t arduino_pinmap[] = {
    ARDUINO_PIN_0,
    ARDUINO_PIN_1
};

/**
 * @brief   Arduino's analog pins mappings
 */
static const adc_t arduino_analog_map[] = {
    ARDUINO_A0,
    ARDUINO_A1,
    ARDUINO_A2
};

/**
 * @brief   On-board LED mapping
 */
#define ARDUINO_LED         (0)

/**
 * @brief   On-board serial port mapping
 */
#define ARDUINO_UART_DEV         UART_DEV(0)

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_BOARD_H */
/** @} */
