/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_f4vi1  F4VI1
 * @ingroup     boards
 * @brief       Board specific files for the F4VI1 board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the f4vi1 board
 *
 * @author      Stefan Pfeiffer <pfeiffer@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef BOARD_H_
#define BOARD_H_

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(PORT_A, 1)
#define LED1_PIN            GPIO_PIN(PORT_A, 3)
#define LED2_PIN            GPIO_PIN(PORT_A, 2)

#define LED_PORT            GPIOA
#define LED0_MASK           (1 << 1)
#define LED1_MASK           (1 << 3)
#define LED2_MASK           (1 << 2)

#define LED0_ON             (LED_PORT->BSRRH = LED0_MASK)
#define LED0_OFF            (LED_PORT->BSRRL = LED0_MASK)
#define LED0_TOGGLE         (LED_PORT->ODR  ^= LED0_MASK)

#define LED1_ON             (LED_PORT->BSRRH = LED1_MASK)
#define LED1_OFF            (LED_PORT->BSRRL = LED1_MASK)
#define LED1_TOGGLE         (LED_PORT->ODR  ^= LED1_MASK)

#define LED2_ON             (LED_PORT->BSRRH = LED2_MASK)
#define LED2_OFF            (LED_PORT->BSRRL = LED2_MASK)
#define LED2_TOGGLE         (LED_PORT->ODR  ^= LED2_MASK)
/** @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H_ */
/** @} */
