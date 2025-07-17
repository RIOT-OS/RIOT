/*
 * SPDX-FileCopyrightText: 2021 Luo Jia (HUST IoT Security Lab)
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_alientek-pandora
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Alientek Pandora board
 *
 * @author      Luo Jia <luojia@hust.edu.cn>
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
#define LED0_PIN_NUM        7
#define LED0_PORT           GPIO_PORT_E /**< GPIO port of LED 0 */
#define LED0_PORT_NUM       PORT_E

#define LED1_PIN_NUM        8
#define LED1_PORT           GPIO_PORT_E /**< GPIO port of LED 1 */
#define LED1_PORT_NUM       PORT_E

#define LED2_PIN_NUM        9
#define LED2_PORT           GPIO_PORT_E /**< GPIO port of LED 2 */
#define LED2_PORT_NUM       PORT_E

/** @} */

/**
 * @name   Wake-up and key buttons
 * @{
 */
#define BTNWK_PIN           GPIO_PIN(PORT_C, 13) /**< Wake button pin  */
#define BTNWK_MODE          GPIO_IN_PD          /**< Wake button mode */

#define BTN0_PIN            GPIO_PIN(PORT_D, 10) /**< Button 0 pin   */
#define BTN0_MODE           GPIO_IN_PD          /**< Button 0 mode  */

#define BTN1_PIN            GPIO_PIN(PORT_D, 9) /**< Button 1 pin    */
#define BTN1_MODE           GPIO_IN_PD          /**< Button 1 mode   */

#define BTN2_PIN            GPIO_PIN(PORT_D, 8) /**< Button 2 pin    */
#define BTN2_MODE           GPIO_IN_PD          /**< Button 2 mode   */
/** @} */

#ifdef __cplusplus
}
#endif

#include "stm32_leds.h"

/** @} */
