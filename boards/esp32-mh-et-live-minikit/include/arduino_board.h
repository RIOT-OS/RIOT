/*
 * Copyright (C)  2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_esp32_mh-et-live-minikit
 * @{
 *
 * @file
 * @brief       Board specific configuration for the Arduino API
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef ARDUINO_BOARD_H
#define ARDUINO_BOARD_H

#include "periph/gpio.h"
#include "periph/adc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The on-board LED is connected to Arduino pin 3 on this board
 */
#define ARDUINO_LED         (3)

/**
 * @brief   Look-up table for the Arduino's digital pins
 */
static const gpio_t arduino_pinmap[] = {
    GPIO3,      /* ARDUINO_PIN_0 (RxD) */
    GPIO1,      /* ARDUINO_PIN_1 (TxD) */

    GPIO32,     /* ARDUINO_PIN_2 */
    GPIO2,      /* ARDUINO_PIN_3 (PWM) */
    GPIO27,     /* ARDUINO_PIN_4 */
    GPIO0,      /* ARDUINO_PIN_5 (PWM) */
    GPIO4,      /* ARDUINO_PIN_6 (PWM) */
    GPIO33,     /* ARDUINO_PIN_7 */
    GPIO25,     /* ARDUINO_PIN_8 */
    GPIO15,     /* ARDUINO_PIN_9 (PWM) */

    GPIO5,      /* ARDUINO_PIN_10 (CS0 / PWM)  */
    GPIO23,     /* ARDUINO_PIN_11 (MOSI / PWM) */
    GPIO19,     /* ARDUINO_PIN_12 (MISO) */
    GPIO18,     /* ARDUINO_PIN_13 (SCK)  */

    GPIO34,     /* ARDUINO_PIN_A0 */
    GPIO35,     /* ARDUINO_PIN_A1 */
    GPIO36,     /* ARDUINO_PIN_A2 */
    GPIO39,     /* ARDUINO_PIN_A3 */

    GPIO21,     /* ARDUINO_PIN_A4 (SDA) */
    GPIO22,     /* ARDUINO_PIN_A5 (SCL) */
};

/**
 * @brief   Look-up table for the Arduino's analog pins
 */
static const adc_t arduino_analog_map[] = {
    GPIO34,     /* ARDUINO_PIN_A0 */
    GPIO35,     /* ARDUINO_PIN_A1 */
    GPIO36,     /* ARDUINO_PIN_A2 */
    GPIO39,     /* ARDUINO_PIN_A3 */

    GPIO13,     /* ARDUINO_PIN_A4 (SDA) */
    GPIO26,     /* ARDUINO_PIN_A5 (SCL) */
};

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_BOARD_H */
/** @} */
