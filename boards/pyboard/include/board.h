/*
 * Copyright (C) 2019 Inria
 *               2019 Freie Universität Berlin
 *               2019 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_pyboard
 * @{
 *
 * @file
 * @brief       Board specific definitions for the pyboard board
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#include <stdint.h>

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN_NUM        4
#define LED0_PORT           GPIO_PORT_B /**< GPIO port of LED 0 */
#define LED0_PORT_NUM       PORT_B
/** @} */

/**
 * @name    User button pin configuration
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PORT_B, 3)     /**< User button pin */
#define BTN0_MODE           GPIO_IN_PU              /**< User button pin mode */
/** @} */

#ifdef __cplusplus
}
#endif

#include "stm32_leds.h"

/** @} */
