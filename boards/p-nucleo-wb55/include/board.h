/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_p-nucleo-wb55
 * @{
 *
 * @file
 * @brief       Pin definitions and board configuration options
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#ifndef BOARD_H
#define BOARD_H

#include "board_nucleo.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN_NUM        5
#define LED0_PORT_NUM       PORT_B

#define LED1_PIN_NUM        0
#define LED1_PORT_NUM       PORT_B

#define LED2_PIN_NUM        1
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

#endif /* BOARD_H */
/** @} */
