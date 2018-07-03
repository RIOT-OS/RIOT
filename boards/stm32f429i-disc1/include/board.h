/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_stm32f429i-disc1
 * @{
 *
 * @file
 * @brief       Board specific definitions for the STM32F429I-DISC1 evaluation board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED0_PIN            GPIO_PIN(PORT_G, 13)
#define LED1_PIN            GPIO_PIN(PORT_G, 14)

#define LED0_MASK           (1 << 13)
#define LED1_MASK           (1 << 14)

#define LED0_ON             (GPIOG->BSRR = LED0_MASK)
#define LED0_OFF            (GPIOG->BSRR = (LED0_MASK << 16))
#define LED0_TOGGLE         (GPIOG->ODR  ^= LED0_MASK)

#define LED1_ON             (GPIOG->BSRR = LED1_MASK)
#define LED1_OFF            (GPIOG->BSRR = (LED1_MASK << 16))
#define LED1_TOGGLE         (GPIOG->ODR  ^= LED1_MASK)
/** @} */

/**
 * @brief User button
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PORT_A, 0)
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
