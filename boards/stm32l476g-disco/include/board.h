/*
 * SPDX-FileCopyrightText: 2018 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup    boards_stm32l476g-disco
 * @brief       Support for the STM32L476G-DISCO board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the STM32L476G-DISCO board
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
#define LED0_PIN_NUM        2
#define LED0_PORT           GPIO_PORT_B /**< GPIO port of LED 0 */
#define LED0_PORT_NUM       PORT_B

#define LED1_PIN_NUM        8
#define LED1_PORT           GPIO_PORT_E /**< GPIO port of LED 1 */
#define LED1_PORT_NUM       PORT_E
/** @} */

/**
 * @name   Joystick buttons
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PORT_A, 0) /**< Center button pin  */
#define BTN0_MODE           GPIO_IN_PD          /**< Center button mode */

#define BTN1_PIN            GPIO_PIN(PORT_A, 1) /**< Left button pin    */
#define BTN1_MODE           GPIO_IN_PD          /**< Left button mode   */

#define BTN2_PIN            GPIO_PIN(PORT_A, 5) /**< Down button pin    */
#define BTN2_MODE           GPIO_IN_PD          /**< Down button mode   */

#define BTN3_PIN            GPIO_PIN(PORT_A, 2) /**< Right button pin   */
#define BTN3_MODE           GPIO_IN_PD          /**< Right button mode  */

#define BTN4_PIN            GPIO_PIN(PORT_A, 3) /**< Up button pin      */
#define BTN4_MODE           GPIO_IN_PD          /**< Up button mode     */
/** @} */

#ifdef __cplusplus
}
#endif

#include "stm32_leds.h"

/** @} */
