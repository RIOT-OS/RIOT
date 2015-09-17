/*
 * Copyright (C) 2015 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_limifrog-v1 LimiFrog Version 1
 * @ingroup     boards
 * @brief       Board specific files for the limifrog-v1 board.
 * @{
 *
 * @file
 * @brief       Board specific definitions for the limifrog-v1  board.
 *
 * @author      Katja Kirstein <katja.kirstein@haw-hamburg.de>
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
 * @name Define the nominal CPU core clock in this board
 */
#define F_CPU               CLOCK_CORECLOCK
/** @} */

/**
 * @name Define the UART to be used as stdio and its baudrate
 * @{
 */
#define STDIO               UART_0
#define STDIO_BAUDRATE      (115200U)
#define STDIO_RX_BUFSIZE    (64U)
/** @} */

/**
 * @name LED pin definitions
 * @{
 */
#define LED_RED_PORT      (GPIOC)
#define LED_RED_PIN       (3)
#define LED_RED_GPIO GPIO(PORT_C,3)
/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED_GREEN_ON
#define LED_GREEN_OFF
#define LED_GREEN_TOGGLE
#define LED_ORANGE_ON
#define LED_ORANGE_OFF
#define LED_ORANGE_TOGGLE
#define LED_RED_ON          (LED_RED_PORT->BSRRL = (1 << LED_RED_PIN))
#define LED_RED_OFF         (LED_RED_PORT->BSRRH = (1 << LED_RED_PIN))
#define LED_RED_TOGGLE      (LED_RED_PORT->ODR ^= (1 << LED_RED_PIN))
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
