/*
 * Copyright (C) 2020 Scallog
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_olimexino-stm32
 * @{
 *
 * @file
 * @brief       Board specific definitions for the OLIMEXINO STM32 board
 *
 * @author      Corentin Vigourt <cvigourt@scallog.com>
 */

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Xtimer configuration
 * @{
 */
#define XTIMER_WIDTH                (16)
#define XTIMER_BACKOFF              (19)
/** @} */

/**
 * @name LED pin definitions and handlers
 * @{
 */
#define LED0_PIN_NUM        1
#define LED0_PORT           GPIO_PORT_A /**< GPIO port of LED 0 */
#define LED0_PORT_NUM       PORT_A

#define LED1_PIN_NUM        5
#define LED1_PORT           GPIO_PORT_A /**< GPIO port of LED 1 */
#define LED1_PORT_NUM       PORT_A

#define LED_PANIC           LED0_ON
/** @} */

/**
 * @name User button
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PORT_C, 9)
#define BTN0_MODE           GPIO_IN
/** @} */

#ifdef __cplusplus
}
#endif

#include "stm32_leds.h"

/** @} */
