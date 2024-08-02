/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_stm32l0538-disco
 * @{
 *
 * @file
 * @brief       Board specific definitions for the STM32L0538-DISCO evaluation board.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Xtimer configuration
 * @{
 */
#define XTIMER_WIDTH                (16)
/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED0_PIN_NUM        4
#define LED0_PORT           GPIO_PORT_B /**< GPIO port of LED 0 */
#define LED0_PORT_NUM       PORT_B

#define LED1_PIN_NUM        5
#define LED1_PORT           GPIO_PORT_A /**< GPIO port of LED 1 */
#define LED1_PORT_NUM       PORT_A
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

#endif /* BOARD_H */
/** @} */
