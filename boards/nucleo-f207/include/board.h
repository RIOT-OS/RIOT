/*
 * Copyright (C) 2016  OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_nucleo-f207 Nucleo-F207
 * @ingroup     boards
 * @brief       Board specific files for the nucleo-f207 board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the nucleo-f207 board
 *
 * @author      Vincent Dupont <vincent@otakeys.com
 * @author      Toon Stegen <toon.stegen@altran.com>
 */

#ifndef BOARD_H_
#define BOARD_H_

#include "board_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   LED pin definitions and handlers
 * @{
 */
#undef LED0_PIN
#undef LED0_MASK
#undef LED0_ON
#undef LED0_OFF
#undef LED0_TOGGLE

#define LED0_PIN            GPIO_PIN(PORT_B, 0)
#define LED0_MASK           (1 << 0)
#define LED0_ON             (GPIOB->BSRR = LED0_MASK)
#define LED0_OFF            (GPIOB->BSRR = (LED0_MASK << 16))
#define LED0_TOGGLE         (GPIOB->ODR  ^= LED0_MASK)

#define LED1_PIN            GPIO_PIN(PORT_B, 7)
#define LED1_MASK           (1 << 7)
#define LED1_ON             (GPIOB->BSRR = LED1_MASK)
#define LED1_OFF            (GPIOB->BSRR = (LED1_MASK << 16))
#define LED1_TOGGLE         (GPIOB->ODR  ^= LED1_MASK)

#define LED2_PIN            GPIO_PIN(PORT_B, 14)
#define LED2_MASK           (1 << 14)
#define LED2_ON             (GPIOB->BSRR = LED2_MASK)
#define LED2_OFF            (GPIOB->BSRR = (LED2_MASK << 16))
#define LED2_TOGGLE         (GPIOB->ODR  ^= LED2_MASK)
/** @} */

/**
 * @brief   Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H_ */
/** @} */
