/*
 * Copyright (C) 2017 Inria
 *               2017 OTAKeys
 *               2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_common_nucleo144 STM32 Nucleo-144
 * @ingroup     boards
 * @brief       Support for STM32 Nucleo-144 boards
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

#ifndef BOARD_H
#define BOARD_H

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
#define LED0_PORT_NUM       PORT_C
#else
#define LED0_PIN_NUM        0
#define LED0_PORT_NUM       PORT_B
#endif

#define LED1_PIN_NUM        7
#define LED1_PORT_NUM       PORT_B

#if defined(CPU_MODEL_STM32L552ZE)
#define LED2_PIN_NUM        9
#define LED2_PORT_NUM       PORT_A
#elif defined(CPU_MODEL_STM32U575ZI)
#define LED2_PIN_NUM        2
#define LED2_PORT_NUM       PORT_G
#else
#define LED2_PIN_NUM        14
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

#endif /* BOARD_H */
/** @} */
