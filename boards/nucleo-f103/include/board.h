/*
 * Copyright (C) 2015 TriaGnoSys GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_nucleo-f103 Nucleo-F103
 * @ingroup     boards
 * @brief       Board specific files for the nucleo-f103 board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the nucleo-f103 board
 *
 * @author      Víctor Ariño <victor.arino@triagnosys.com>
 */

#ifndef BOARD_H_
#define BOARD_H_

#include <stdint.h>

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Use the 2nd UART for STDIO on this board
 */
#define STDIO               UART_DEV(1)

/**
 * @name xtimer configuration
 */
#define XTIMER_MASK         (0xffff0000)
#define XTIMER_BACKOFF      5
/** @} */

/**
 * @name LED pin definitions
 * @{
 */
#define LED_GREEN_GPIO      GPIO_PIN(PORT_A, 5)
/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED_RED_ON
#define LED_RED_OFF
#define LED_RED_TOGGLE

#define LED_GREEN_ON        (GPIOA->BSRR = (1 << 5))
#define LED_GREEN_OFF       (GPIOA->BRR = (1 << 5))
#define LED_GREEN_TOGGLE    (GPIOA->ODR ^= (1 << 5))

#define LED_ORANGE_ON
#define LED_ORANGE_OFF
#define LED_ORANGE_TOGGLE
/** @} */

/**
 * @name Buttons
 * @{
 */
#define BUTTON_USER_GPIO    GPIO_PIN(PORT_C, 13)
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
