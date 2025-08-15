/*
 * SPDX-FileCopyrightText: 2017 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_sodaq-explorer
 * @{
 *
 * @file
 * @brief       Board specific definitions for the SODAQ ExpLoRer board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include "cpu.h"
#include "board_common.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(PA, 21)

#define LED0_PORT           PORT->Group[PA]
#define LED0_MASK           (1 << 21)

#define LED0_ON             (LED0_PORT.OUTSET.reg = LED0_MASK)
#define LED0_OFF            (LED0_PORT.OUTCLR.reg = LED0_MASK)
#define LED0_TOGGLE         (LED0_PORT.OUTTGL.reg = LED0_MASK)
/** @} */

/**
 * @name    User button
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PA, 14)
#define BTN0_MODE           GPIO_IN_PU
/** @} */

/**
 * @name    RN2483 configuration
 * @{
 */
#define RN2XX3_PARAM_UART         UART_DEV(2)
#define RN2XX3_PARAM_PIN_RESET    GPIO_PIN(PA, 7)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
