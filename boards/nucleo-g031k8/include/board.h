/*
 * SPDX-FileCopyrightText: 2026 Hudson C. Dalpra
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_nucleo-g031k8
 * @{
 *
 * @file
 * @brief       Pin definitions and board configuration options
 *
 * @author      Hudson C. Dalpra <dalpra.hcd@gmail.com>
 */

#include "board_nucleo.h"
#include "arduino_pinmap.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Macros for controlling the on-board LED (LD3).
 * @{
 */
#define LED0_PIN_NUM        6
#define LED0_PORT           GPIO_PORT_C /**< GPIO port of LED 3 */
#define LED0_PORT_NUM       PORT_C
/** @} */

/**
 * @name Macros for controlling the on-board button (B1).
 *
 * @note B1 is on PF2, which is shared with NRST. The `NRST_MODE` option byte
 *       must be set to `2` so PF2 acts as a GPIO; otherwise the pin stays a
 *       reset line and the button cannot be read. See the board documentation.
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PORT_F, 2)  /**< Button B1 pin  */
#define BTN0_MODE           GPIO_IN_PU           /**< Button B1 mode (released = HIGH) */
/** @} */

#ifdef __cplusplus
}
#endif

#include "stm32_leds.h"

/** @} */
