/*
 * Copyright (C) 2015 Freie Universität Berlin
 * Copyright (C) 2016 Frank Holtz <frank-riot2015@holtznet.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nrf52-minidev
 * @{
 *
 * @file
 * @brief       Board specific configuration for the Arduino API
 *
 * @author      Hauke Petersen  <hauke.petersen@fu-berlin.de>
 */

#ifndef ARDUINO_BOARD_H
#define ARDUINO_BOARD_H

#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ARDUINO_LED         (5)

static const gpio_t arduino_pinmap[] = {
    GPIO_PIN(0, 25), // D00
    GPIO_PIN(0, 26), // D01
    GPIO_PIN(0, 27), // D02
    GPIO_PIN(0, 28), // D03
    GPIO_PIN(0, 29), // D04
    GPIO_PIN(0, 30), // D05, LED0
    GPIO_PIN(0, 31), // D06, LED1
    GPIO_PIN(0,  2), // D07
    GPIO_PIN(0,  3), // D08
    GPIO_PIN(0,  4), // D09, BUTTON
    GPIO_PIN(0,  5), // D10
    GPIO_PIN(0,  0), // D11, 32kHZ XTAL ?
    GPIO_PIN(0,  1), // D12, 32kHZ XTAL ?
    0,               // D13 (not connected)
    GPIO_PIN(0,  7), // D14 
    0,               // D15 (not connected)
    0,               // D16 (not connected)
    0,               // D17 (not connected)
    GPIO_PIN(0, 11), // D18, RXD (not via ISP header)
    GPIO_PIN(0, 12), // D19, TXD (not via ISP header)
    GPIO_PIN(0, 13), // D20
    GPIO_PIN(0, 14), // D21
    GPIO_PIN(0, 15), // D22
    GPIO_PIN(0, 16), // D23
    GPIO_PIN(0, 13), // D24
    GPIO_PIN(0, 18), // D25
    GPIO_PIN(0, 19), // D26
    GPIO_PIN(0, 20), // D27
    GPIO_PIN(0, 22), // D28
    GPIO_PIN(0, 23), // D29
    GPIO_PIN(0, 24), // D30
};

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_BOARD_H */
/** @} */
