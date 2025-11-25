/*
 * Copyright (C) 2021 luisan00
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_stm32f469i-disco
 * @{
 *
 * @file
 * @brief       Board specific definitions for the STM32F469I-DISCO board
 *
 * @author      Luis A. Ruiz <luisan00@hotmail.com>
 */
#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name Macros for controlling the on-board LEDs
 * @{
 */
#define LED0_PIN_NUM    6
#define LED0_PORT_NUM   PORT_G
#define LED0_IS_INVERTED    1

#define LED1_PIN_NUM    4
#define LED1_PORT_NUM   PORT_D
#define LED1_IS_INVERTED    1

#define LED2_PIN_NUM    5
#define LED2_PORT_NUM   PORT_D
#define LED2_IS_INVERTED    1

#define LED3_PIN_NUM    3
#define LED3_PORT_NUM   PORT_K
#define LED3_IS_INVERTED    1
/** @} */

/**
 * @name User button
 * @{
 */
#define BTN0_PIN GPIO_PIN(PORT_A, 0)
#define BTN0_MODE GPIO_IN
/** @} */

#ifdef __cplusplus
}
#endif

#include "stm32_leds.h"

#endif /* BOARD_H */
/** @} */
