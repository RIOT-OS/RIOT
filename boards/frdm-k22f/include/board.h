/*
 * Copyright (C) 2017 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_frdm-k22f NXP FRDM-K22F Board
 * @ingroup     boards
 * @brief       Board specific implementations for the FRDM-K22F
 * @{
 *
 * @file
 * @brief       Board specific definitions for the FRDM-K22F
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
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
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(PORT_A,  1)
#define LED1_PIN            GPIO_PIN(PORT_A,  2)
#define LED2_PIN            GPIO_PIN(PORT_D,  5)

#define LED0_MASK           (1 <<  1)
#define LED1_MASK           (1 <<  2)
#define LED2_MASK           (1 <<  5)

#define LED0_ON            (GPIOB->PCOR = LED0_MASK)
#define LED0_OFF           (GPIOB->PSOR = LED0_MASK)
#define LED0_TOGGLE        (GPIOB->PTOR = LED0_MASK)

#define LED1_ON            (GPIOE->PCOR = LED1_MASK)
#define LED1_OFF           (GPIOE->PSOR = LED1_MASK)
#define LED1_TOGGLE        (GPIOE->PTOR = LED1_MASK)

#define LED2_ON            (GPIOB->PCOR = LED2_MASK)
#define LED2_OFF           (GPIOB->PSOR = LED2_MASK)
#define LED2_TOGGLE        (GPIOB->PTOR = LED2_MASK)
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
