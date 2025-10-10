/*
 * SPDX-FileCopyrightText: 2024 BISSELL Homecare, Inc.
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     boards_stm32c0116-dk
 * @{
 *
 * @file
 * @brief       Board specific definitions for the STM32C0116-DK
 *
 * @author      Jason Parker <Jason.Parker@bissell.com>
 */

#pragma once

#include "cpu.h"
#include "periph_conf.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LED0_PIN_NUM        6
#define LED0_PORT           GPIO_PORT_B /**< GPIO port of LED 0 */
#define LED0_PORT_NUM       PORT_B

#define BTN0_PIN            GPIO_PIN(PORT_A, 8)
#define BTN0_MODE           GPIO_IN

#ifdef __cplusplus
}
#endif

#include "stm32_leds.h"

/** @} */
