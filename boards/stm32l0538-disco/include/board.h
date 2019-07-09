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
#define LED0_PIN            GPIO_PIN(PORT_B, 4)
#define LED0_PORT           GPIOB
#define LED0_MASK           (1 << 4)

#define LED0_ON             (LED0_PORT->BSRR = LED0_MASK)
#define LED0_OFF            (LED0_PORT->BRR  = LED0_MASK)
#define LED0_TOGGLE         (LED0_PORT->ODR ^= LED0_MASK)

#define LED1_PIN            GPIO_PIN(PORT_A, 5)
#define LED1_PORT           GPIOA
#define LED1_MASK           (1 << 5)

#define LED1_ON             (LED1_PORT->BSRR = LED1_MASK)
#define LED1_OFF            (LED1_PORT->BRR  = LED1_MASK)
#define LED1_TOGGLE         (LED1_PORT->ODR ^= LED1_MASK)
/** @} */

/**
 * @name User button
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PORT_A, 0)
#define BTN0_PORT           GPIOA
#define BTN0_MODE           GPIO_IN
/** @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
