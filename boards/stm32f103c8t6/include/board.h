/*
 * Copyright (C) 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_stm32f130c8t6
 * @{
 *
 * @file
 * @brief       Board specific definitions for the STM32F103C8T6 demo board
 *
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
 * @name    xtimer configuration
 */
#define XTIMER_WIDTH        (16U)
/** @} */

/**
 * @name Macros for controlling the on-board LED.
 * @{
 */
#define LED0_PIN            GPIO_PIN(PORT_C, 13)

#define LED0_PORT           GPIOC

#define LED0_MASK           (1 << 13)

#define LED0_ON             (LED0_PORT->BSRR = LED0_MASK)
#define LED0_OFF            (LED0_PORT->BSRR = (LED0_MASK << 16))
#define LED0_TOGGLE         (LED0_PORT->ODR  ^= LED0_MASK)
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
