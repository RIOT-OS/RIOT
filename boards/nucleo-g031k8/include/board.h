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
 * @note PF2 is shared with NRST so the behaviour depends on `NRST_MODE`.
 *       See `boards/nucleo-g031k8/doc.md` (Reset configuration).
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
