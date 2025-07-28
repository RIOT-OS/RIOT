/*
 * SPDX-FileCopyrightText: 2019 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_p-l496g-cell02
 * @{
 *
 * @file
 * @brief       Board specific definitions for the P-L496G-CELL02 board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
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
#define LED0_PIN_NUM        5
#define LED0_PORT           GPIO_PORT_A /**< GPIO port of LED 0 */
#define LED0_PORT_NUM       PORT_A

#define LED1_PIN_NUM        13
#define LED1_PORT           GPIO_PORT_B /**< GPIO port of LED 1 */
#define LED1_PORT_NUM       PORT_B
/** @} */

/**
 * @name   Joystick buttons
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PORT_C, 13)    /**< Center button pin  */
#define BTN0_MODE           GPIO_IN_PD              /**< Center button mode */

#define BTN1_PIN            GPIO_PIN(PORT_I, 9)     /**< Left button pin    */
#define BTN1_MODE           GPIO_IN_PD              /**< Left button mode   */

#define BTN2_PIN            GPIO_PIN(PORT_I, 10)    /**< Down button pin    */
#define BTN2_MODE           GPIO_IN_PD              /**< Down button mode   */

#define BTN3_PIN            GPIO_PIN(PORT_F, 11)    /**< Right button pin   */
#define BTN3_MODE           GPIO_IN_PD              /**< Right button mode  */

#define BTN4_PIN            GPIO_PIN(PORT_I, 8)     /**< Up button pin      */
#define BTN4_MODE           GPIO_IN_PD              /**< Up button mode     */
/** @} */

#ifdef __cplusplus
}
#endif

#include "stm32_leds.h"

/** @} */
