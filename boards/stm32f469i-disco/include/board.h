/*
 * Copyright (C) 2021 luisan00
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#pragma once

/**
 * @ingroup     boards_stm32f469i-disco
 * @{
 *
 * @file
 * @brief       Board specific definitions for the STM32F469I-DISCO board
 *
 * @author      Luis A. Ruiz <luisan00@hotmail.com>
 */
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
#define LED0_PIN_NUM        6               /**< pin number of LED 0*/
#define LED0_PORT           GPIO_PORT_G     /**< GPIO port of LED 0 */
#define LED0_PORT_NUM       PORT_G          /**< GPIO port number of LED 0 */
#define LED0_IS_INVERTED    1               /**< LED 0 is active-low */

#define LED1_PIN_NUM        4               /**< pin number of LED 1*/
#define LED1_PORT           GPIO_PORT_D     /**< GPIO port of LED 1 */
#define LED1_PORT_NUM       PORT_D          /**< GPIO port number of LED 0 */
#define LED1_IS_INVERTED    1               /**< LED 1 is active-low */

#define LED2_PIN_NUM        5               /**< pin number of LED 2*/
#define LED2_PORT           GPIO_PORT_D     /**< GPIO port of LED 2 */
#define LED2_PORT_NUM       PORT_D          /**< GPIO port number of LED 0 */
#define LED2_IS_INVERTED    1               /**< LED 2 is active-low */

#define LED3_PIN_NUM        3               /**< pin number of LED 3*/
#define LED3_PORT           GPIO_PORT_K     /**< GPIO port of LED 3 */
#define LED3_PORT_NUM       PORT_K          /**< GPIO port number of LED 0 */
#define LED3_IS_INVERTED    1               /**< LED 3 is active-low */
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

/** @} */
