/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nucleo-wl55jc
 * @{
 *
 * @file
 * @brief       Pin definitions and board configuration options
 *
 * @author      Akshai M <akshai.m@fu-berlin.de>
 * @author      Hauke Petersen <devel@haukepetersen.de>
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
#define LED0_PORT           GPIOB
#define LED0_PIN            GPIO_PIN(PORT_B, 15)
#define LED0_MASK           (1 << 15)
#define LED0_ON             (LED0_PORT->BSRR = LED0_MASK)
#define LED0_OFF            (LED0_PORT->BSRR = (LED0_MASK << 16))
#define LED0_TOGGLE         (LED0_PORT->ODR  ^= LED0_MASK)

#define LED1_PORT           GPIOB
#define LED1_PIN            GPIO_PIN(PORT_B, 9)
#define LED1_MASK           (1 << 9)
#define LED1_ON             (LED0_PORT->BSRR = LED1_MASK)
#define LED1_OFF            (LED0_PORT->BSRR = (LED1_MASK << 16))
#define LED1_TOGGLE         (LED0_PORT->ODR  ^= LED1_MASK)

#define LED2_PORT           GPIOB
#define LED2_PIN            GPIO_PIN(PORT_B, 11)
#define LED2_MASK           (1 << 11)
#define LED2_ON             (LED0_PORT->BSRR = LED2_MASK)
#define LED2_OFF            (LED0_PORT->BSRR = (LED2_MASK << 16))
#define LED2_TOGGLE         (LED0_PORT->ODR  ^= LED2_MASK)
/** @} */

/* nucleo-wl55jc always use LED0, as there is no dual use of its pin */
#ifndef AUTO_INIT_LED0
#define AUTO_INIT_LED0
#endif
/** @} */

/**
 * @name    User button
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PORT_A, 0)
#define BTN0_MODE           GPIO_IN_PU
#define BTN1_PIN            GPIO_PIN(PORT_A, 1)
#define BTN1_MODE           GPIO_IN_PU
#define BTN2_PIN            GPIO_PIN(PORT_C, 6)
#define BTN2_MODE           GPIO_IN_PU
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
