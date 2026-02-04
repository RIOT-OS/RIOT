/*
 * SPDX-FileCopyrightText: 2017 Inria
 * SPDX-FileCopyrightText: 2017 OTAKeys
 * SPDX-FileCopyrightText: 2018 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_common_nucleo144
 * @{
 *
 * @file
 * @brief       Common pin definitions and board configuration options
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Sebastian Meiling <s@mlng.net>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include "arduino_pinmap.h"
#include "board_nucleo.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#if defined(CPU_MODEL_STM32L496ZG) || defined(CPU_MODEL_STM32L4R5ZI) || \
    defined(CPU_MODEL_STM32L552ZE) || defined(CPU_MODEL_STM32U575ZI)
#define LED0_PIN_NUM        7
#define LED0_PORT           GPIO_PORT_C /**< GPIO port of LED 0 */
#define LED0_PORT_NUM       PORT_C
#else
#define LED0_PIN_NUM        0
#define LED0_PORT           GPIO_PORT_B /**< GPIO port of LED 0 */
#define LED0_PORT_NUM       PORT_B
#endif

#define LED1_PIN_NUM        7
#define LED1_PORT           GPIO_PORT_B /**< GPIO port of LED 1 */
#define LED1_PORT_NUM       PORT_B

#if defined(CPU_MODEL_STM32L552ZE)
#define LED2_PIN_NUM        9
#define LED2_PORT           GPIO_PORT_A /**< GPIO port of LED 2 */
#define LED2_PORT_NUM       PORT_A
#elif defined(CPU_MODEL_STM32U575ZI)
#define LED2_PIN_NUM        2
#define LED2_PORT           GPIO_PORT_G /**< GPIO port of LED 2 */
#define LED2_PORT_NUM       PORT_G
#else
#define LED2_PIN_NUM        14
#define LED2_PORT           GPIO_PORT_B /**< GPIO port of LED 2 */
#define LED2_PORT_NUM       PORT_B
#endif
/** @} */

/**
 * @name    User button
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PORT_C, 13)
#define BTN0_MODE           GPIO_IN_PD
/** @} */

#ifdef __cplusplus
}
#endif

#include "stm32_leds.h"

/** @} */
