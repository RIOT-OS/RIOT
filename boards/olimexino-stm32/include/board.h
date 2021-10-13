/*
 * Copyright (C) 2020 Scallog
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_olimexino-stm32
 * @{
 *
 * @file
 * @brief       Board specific definitions for the OLIMEXINO STM32 board
 *
 * @author      Corentin Vigourt <cvigourt@scallog.com>
 */

#ifndef BOARD_H
#define BOARD_H

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
#define LED0_PORT           GPIOA
#define LED0_PIN            GPIO_PIN(PORT_A, 1)
#define LED0_MASK           (1 << 1)

#define LED1_PORT           GPIOA
#define LED1_PIN            GPIO_PIN(PORT_A, 5)
#define LED1_MASK           (1 << 5)

#define LED0_ON             (LED0_PORT->BSRR = LED0_MASK)
#define LED0_OFF            (LED0_PORT->BSRR = (LED0_MASK << 16))
#define LED0_TOGGLE         (LED0_PORT->ODR  ^= LED0_MASK)

#define LED1_ON             (LED1_PORT->BSRR = LED1_MASK)
#define LED1_OFF            (LED1_PORT->BSRR = (LED1_MASK << 16))
#define LED1_TOGGLE         (LED1_PORT->ODR  ^= LED1_MASK)

#define LED_PANIC           LED0_ON
/** @} */

/**
 * @name User button
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PORT_C, 9)
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
