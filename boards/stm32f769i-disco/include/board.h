/*
 * Copyright (C) 2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_stm32f769i-disco
 * @{
 *
 * @file
 * @brief       Board specific definitions for the STM32F769I-DISCO board
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Sebastian Meiling <s@mlng.net>
 */

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED0_PIN_NUM        13
#define LED0_PORT           GPIO_PORT_J /**< GPIO port of LED 0 */
#define LED0_PORT_NUM       PORT_J

#define LED1_PIN_NUM        5
#define LED1_PORT           GPIO_PORT_J /**< GPIO port of LED 1 */
#define LED1_PORT_NUM       PORT_J

#define LED2_PIN_NUM        12
#define LED2_PORT           GPIO_PORT_A /**< GPIO port of LED 2 */
#define LED2_PORT_NUM       PORT_A

#define LED3_PIN_NUM        4
#define LED3_PORT           GPIO_PORT_D /**< GPIO port of LED 3 */
#define LED3_PORT_NUM       PORT_D
/** @} */

/**
 * @name User button
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PORT_A, 0)
#define BTN0_MODE           GPIO_IN
/** @} */

#ifdef __cplusplus
}
#endif

#include "stm32_leds.h"

/** @} */
