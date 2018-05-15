/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_stm32l476g-disco STM32L476G-DISCO
 * @ingroup     boards
 * @brief       Support for the STM32L476G-DISCO board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the STM32L476G-DISCO board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(PORT_B, 2)
#define LED0_MASK           (1 << 2)

#define LED0_ON             (GPIOB->BSRR = LED0_MASK)
#define LED0_OFF            (GPIOB->BSRR = (LED0_MASK << 16))
#define LED0_TOGGLE         (GPIOB->ODR  ^= LED0_MASK)

#define LED1_PIN            GPIO_PIN(PORT_E, 8)
#define LED1_MASK           (1 << 8)

#define LED1_ON             (GPIOE->BSRR = LED1_MASK)
#define LED1_OFF            (GPIOE->BSRR = (LED1_MASK << 16))
#define LED1_TOGGLE         (GPIOE->ODR  ^= LED1_MASK)
/** @} */

/**
 * @name   Joystick buttons
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PORT_A, 0) /**< Center button pin  */
#define BTN0_MODE           GPIO_IN_PD          /**< Center button mode */

#define BTN1_PIN            GPIO_PIN(PORT_A, 1) /**< Left button pin    */
#define BTN1_MODE           GPIO_IN_PD          /**< Left button mode   */

#define BTN2_PIN            GPIO_PIN(PORT_A, 5) /**< Down button pin    */
#define BTN2_MODE           GPIO_IN_PD          /**< Down button mode   */

#define BTN3_PIN            GPIO_PIN(PORT_A, 2) /**< Right button pin   */
#define BTN3_MODE           GPIO_IN_PD          /**< Right button mode  */

#define BTN4_PIN            GPIO_PIN(PORT_A, 3) /**< Up button pin      */
#define BTN4_MODE           GPIO_IN_PD          /**< Up button mode     */
/** @} */

/**
 * @brief   Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
