/*
 * Copyright (C)  2020 Koen Zandberg <koen@bergzand.net>
 *                2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_frdm-k28f
 * @{
 *
 * @file
 * @brief       Board specific configuration for the Arduino API
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef ARDUINO_BOARD_H
#define ARDUINO_BOARD_H

#include "periph/adc.h"
#include "periph/gpio.h"

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
    GPIO_PIN(PORT_A, 25),
    GPIO_PIN(PORT_A, 24),
    GPIO_PIN(PORT_B, 4),
    GPIO_PIN(PORT_C, 11),
    GPIO_PIN(PORT_C, 6),
    GPIO_PIN(PORT_B, 12),
    GPIO_PIN(PORT_C, 13),
    GPIO_PIN(PORT_C, 18),
    GPIO_PIN(PORT_C, 19),
    GPIO_PIN(PORT_B, 19),
    GPIO_PIN(PORT_D, 15),
    GPIO_PIN(PORT_D, 13),
    GPIO_PIN(PORT_D, 14),
    GPIO_PIN(PORT_D, 12),

    GPIO_PIN(PORT_A, 7),
    GPIO_PIN(PORT_A, 8),
    GPIO_PIN(PORT_D, 1),
    GPIO_PIN(PORT_D, 6),
    GPIO_PIN(PORT_B, 2),
    GPIO_PIN(PORT_B, 3),
};

/**
 * @brief   Look-up table for the Arduino's analog pins
 */
static const adc_t arduino_analog_map[] = {
    ADC_LINE(2),
    ADC_LINE(3),
    ADC_LINE(4),
    ADC_LINE(5),
    ADC_LINE(6),
    ADC_LINE(7),
};

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_BOARD_H */
/** @} */
