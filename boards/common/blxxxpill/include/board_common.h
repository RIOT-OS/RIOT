/*
 * SPDX-FileCopyrightText: 2015 TriaGnoSys GmbH
 * SPDX-FileCopyrightText: 2017 Alexander Kurth, Sören Tempel, Tristan Bruns
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_common_blxxxpill
 *
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the bluepill/blackpill board
 *
 * @author      Víctor Ariño <victor.arino@triagnosys.com>
 * @author      Sören Tempel <tempel@uni-bremen.de>
 * @author      Tristan Bruns <tbruns@uni-bremen.de>
 * @author      Alexander Kurth <kurth1@uni-bremen.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   Macros for controlling the on-board LED.
 * @{
 */
#ifndef LED0_PORT_NUM
#define LED0_PORT           GPIO_PORT_C /**< GPIO port of LED 0 */
#define LED0_PORT_NUM       PORT_C                                  /**< GPIO Port number the LED is connected to */
#endif
#ifndef LED0_PIN_NUM
#define LED0_PIN_NUM        (13)                                    /**< Pin number the LED is connected to */
#endif
#ifndef LED0_IS_INVERTED
#define LED0_IS_INVERTED    1
#endif
/** @} */

/**
 * @brief   Use the fist UART for STDIO on this board
 */
#define STDIO_UART_DEV      UART_DEV(0)

/**
 * @name    xtimer configuration
 * @{
 */
#define XTIMER_WIDTH        (16)
#define XTIMER_BACKOFF      (19)
/** @} */

#ifdef __cplusplus
}
#endif

#include "stm32_leds.h"

/** @} */
