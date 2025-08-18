/*
 * SPDX-FileCopyrightText: 2016-2017 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_arduino-mkr1000
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Arduino MKR1000
 *              board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include "cpu.h"
#include "periph_conf.h"
#include "board_common.h"
#include "arduino_pinmap.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(PA, 20)

#define LED_PORT            PORT->Group[PA]
#define LED0_MASK           (1 << 20)

#define LED0_ON             (LED_PORT.OUTSET.reg = LED0_MASK)
#define LED0_OFF            (LED_PORT.OUTCLR.reg = LED0_MASK)
#define LED0_TOGGLE         (LED_PORT.OUTTGL.reg = LED0_MASK)

#define LED0_NAME           "LED(Green)"
/** @} */

/**
 * @name    Board configuration for ATWINC15x0 WiFi netdev driver
 * @{
 */
#define ATWINC15X0_PARAM_SPI            SPI_DEV(1)
#define ATWINC15X0_PARAM_SSN_PIN        GPIO_PIN(0, 14)
#define ATWINC15X0_PARAM_RESET_PIN      GPIO_PIN(0, 27)
#define ATWINC15X0_PARAM_CHIP_EN_PIN    GPIO_PIN(0, 28)
#define ATWINC15X0_PARAM_WAKE_PIN       GPIO_PIN(1, 8)
#define ATWINC15X0_PARAM_IRQ_PIN        GPIO_PIN(1, 9)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
