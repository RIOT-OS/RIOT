/*
 * SPDX-FileCopyrightText: 2014 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_f4vi1
 * @{
 *
 * @file
 * @brief       Board specific definitions for the f4vi1 board
 *
 * @author      Stefan Pfeiffer <pfeiffer@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN_NUM        1
#define LED0_PORT           GPIO_PORT_A /**< GPIO port of LED 0 */
#define LED0_PORT_NUM       PORT_A

#define LED1_PIN_NUM        3
#define LED1_PORT           GPIO_PORT_A /**< GPIO port of LED 1 */
#define LED1_PORT_NUM       PORT_A

#define LED2_PIN_NUM        2
#define LED2_PORT           GPIO_PORT_A /**< GPIO port of LED 2 */
#define LED2_PORT_NUM       PORT_A
/** @} */

#ifdef __cplusplus
}
#endif

#include "stm32_leds.h"

/** @} */
