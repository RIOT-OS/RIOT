/*
 * Copyright (C) 2015 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_limifrog-v1
 * @{
 *
 * @file
 * @brief       Board specific definitions for the limifrog-v1 board
 *
 * @author      Katja Kirstein <katja.kirstein@haw-hamburg.de>
 */

#ifndef BOARD_H
#define BOARD_H

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

#endif /* BOARD_H */
/** @} */
