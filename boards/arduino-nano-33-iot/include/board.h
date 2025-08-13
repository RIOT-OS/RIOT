/*
 * SPDX-FileCopyrightText: 2020 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_arduino-nano-33-iot
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Arduino Nano 33 IoT
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include "cpu.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(PA, 17)

#define LED_PORT            PORT->Group[PA]
#define LED0_MASK           (1 << 17)

#define LED0_ON             (LED_PORT.OUTSET.reg = LED0_MASK)
#define LED0_OFF            (LED_PORT.OUTCLR.reg = LED0_MASK)
#define LED0_TOGGLE         (LED_PORT.OUTTGL.reg = LED0_MASK)

#define LED0_NAME           "LED(Yellow)"
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
