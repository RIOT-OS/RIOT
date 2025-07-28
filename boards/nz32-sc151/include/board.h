/*
 * SPDX-FileCopyrightText: 2016 Fundacion Inria Chile
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_nz32-sc151
 * @{
 *
 * @file
 * @brief       Board specific definitions for the nz32-sc151 board.
 *
 * @author      Francisco Molina <francisco.molina@inria.cl>
 */

#include <stdint.h>

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    User LED pin definitions and handlers
 * @{
 */
#define LED0_PIN_NUM        2
#define LED0_PORT           GPIO_PORT_B /**< GPIO port of LED 0 */
#define LED0_PORT_NUM       PORT_B
 /** @} */

#ifdef __cplusplus
}
#endif

#include "stm32_leds.h"

/** @} */
