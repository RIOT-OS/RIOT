/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_stm32f4discovery
 * @{
 *
 * @file
 * @brief       Board specific definitions for the STM32F4Discovery evaluation board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Sebastian Meiling <s@mlng.net>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name xtimer configuration
 * @{
 */
#define XTIMER_BACKOFF      (10)
/** @} */

/**
 * @name LED pin definitions
 * @{
 */
/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED0_PIN            GPIO_PIN(PORT_D, 13)
#define LED1_PIN            GPIO_PIN(PORT_D, 12)
#define LED2_PIN            GPIO_PIN(PORT_D, 14)
#define LED3_PIN            GPIO_PIN(PORT_D, 15)

#define LED_PORT            GPIOD
#define LED0_MASK           (1 << 13)
#define LED1_MASK           (1 << 12)
#define LED2_MASK           (1 << 14)
#define LED3_MASK           (1 << 15)

#define LED0_ON             (LED_PORT->BSRR = LED0_MASK)
#define LED0_OFF            (LED_PORT->BSRR = (LED0_MASK << 16))
#define LED0_TOGGLE         (LED_PORT->ODR  ^= LED0_MASK)

#define LED1_ON             (LED_PORT->BSRR = LED1_MASK)
#define LED1_OFF            (LED_PORT->BSRR = (LED1_MASK << 16))
#define LED1_TOGGLE         (LED_PORT->ODR  ^= LED1_MASK)

#define LED2_ON             (LED_PORT->BSRR = LED2_MASK)
#define LED2_OFF            (LED_PORT->BSRR = (LED2_MASK << 16))
#define LED2_TOGGLE         (LED_PORT->ODR  ^= LED2_MASK)

#define LED3_ON             (LED_PORT->BSRR = LED3_MASK)
#define LED3_OFF            (LED_PORT->BSRR = (LED3_MASK << 16))
#define LED3_TOGGLE         (LED_PORT->ODR  ^= LED3_MASK)
/** @} */

/**
 * @name User button
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
