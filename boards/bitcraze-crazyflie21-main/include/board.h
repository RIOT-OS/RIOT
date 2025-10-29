/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_bitcraze_crazyflie21_main
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Crazyflie 2.1 main application MCU
 *
 * @author      Leonard Herbst <leonard.herbst@tu-dresden.de>
 */

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED0_IS_INVERTED    1
#define LED0_PIN_NUM        3 /* LED top right red */
#define LED0_PORT           GPIO_PORT_C
#define LED0_PORT_NUM       PORT_C

#define LED1_IS_INVERTED    1
#define LED1_PIN_NUM        2 /* LED top right green */
#define LED1_PORT           GPIO_PORT_C
#define LED1_PORT_NUM       PORT_C

#define LED2_PIN_NUM        2 /* LED bottom left blue */
#define LED2_PORT           GPIO_PORT_D
#define LED2_PORT_NUM       PORT_D

#define LED3_IS_INVERTED    1
#define LED3_PIN_NUM        0 /* LED top left red*/
#define LED3_PORT           GPIO_PORT_C
#define LED3_PORT_NUM       PORT_C

#define LED4_IS_INVERTED    1
#define LED4_PIN_NUM        1 /* LED top left green */
#define LED4_PORT           GPIO_PORT_C
#define LED4_PORT_NUM       PORT_C
/** @} */

#ifdef __cplusplus
}
#endif

#include "stm32_leds.h"
/** @} */
