/*
 * SPDX-FileCopyrightText: 2015 Hamburg University of Applied Sciences
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_limifrog-v1
 * @{
 *
 * @file
 * @brief       Board specific definitions for the limifrog-v1 board
 *
 * @author      Katja Kirstein <katja.kirstein@haw-hamburg.de>
 */

#include <stdint.h>

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN_NUM        3
#define LED0_PORT           GPIO_PORT_C /**< GPIO port of LED 0 */
#define LED0_PORT_NUM       PORT_C
 /** @} */

 /**
  * @name    xtimer configuration
  * @{
  */
 #define XTIMER_WIDTH        (16U)
 /** @} */

 /**
 * @name Define the interface to the LIS3MDL 3-axis magnetometer
 * @{
 */
#define LIS3MDL_PARAM_I2C        (I2C_DEV(1))
#define LIS3MDL_PARAM_ADDR       (0x28)
/** @} */

#ifdef __cplusplus
}
#endif

#include "stm32_leds.h"

/** @} */
