/*
 * SPDX-FileCopyrightText: 2019 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_p-nucleo-wb55
 * @{
 *
 * @file
 * @brief       Pin definitions and board configuration options
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#include "board_nucleo.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN_NUM        5
#define LED0_PORT           GPIO_PORT_B /**< GPIO port of LED 0 */
#define LED0_PORT_NUM       PORT_B

#define LED1_PIN_NUM        0
#define LED1_PORT           GPIO_PORT_B /**< GPIO port of LED 1 */
#define LED1_PORT_NUM       PORT_B

#define LED2_PIN_NUM        1
#define LED2_PORT           GPIO_PORT_B /**< GPIO port of LED 2 */
#define LED2_PORT_NUM       PORT_B
/** @} */

/**
 * @name    User button
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PORT_C, 4)
#define BTN0_MODE           GPIO_IN_PU
#define BTN1_PIN            GPIO_PIN(PORT_D, 0)
#define BTN1_MODE           GPIO_IN_PU
#define BTN2_PIN            GPIO_PIN(PORT_D, 1)
#define BTN2_MODE           GPIO_IN_PU
/** @} */

#ifdef __cplusplus
}
#endif

#include "stm32_leds.h"

/** @} */
