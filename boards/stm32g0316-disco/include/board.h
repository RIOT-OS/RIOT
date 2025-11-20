/*
 * SPDX-FileCopyrightText: 2021 BISSELL Homecare, Inc.
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_stm32g0316-disco
 * @{
 *
 * @file
 * @brief       Board specific definitions for the STM32G0316-DISCO
 *
 * @author      Dave VanKampen <david.vankampen@bissell.com>
 */

#include "cpu.h"
#include "periph_conf.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LED0_PIN_NUM        12
#define LED0_PORT           GPIO_PORT_A /**< GPIO port of LED 0 */
#define LED0_PORT_NUM       PORT_A

#define BTN0_PIN            GPIO_PIN(PORT_A, 0)
#define BTN0_MODE           GPIO_IN

#ifdef __cplusplus
}
#endif

#include "stm32_leds.h"

/** @} */
