/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_arduino-mega2560 Arduino Mega 2560
 * @ingroup     boards
 * @brief       Board specific files for the Arduino Mega 2560 board.
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Arduino Mega 2560 board.
 *
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 */

#ifndef BOARD_H_
#define BOARD_H_

#include "cpu.h"
#include "arduino_pinmap.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* @brief As the CPU is too slow to handle 115200 baud, we set the default
*        baudrate to 9600 for this board
*/
#define UART_STDIO_BAUDRATE (9600U)

/**
 * @brief   LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(1, 7)

#define LED0_MASK           (1 << DDB7)

#define LED0_ON             (PORTB |=  LED0_MASK)
#define LED0_OFF            (PORTB &= ~LED0_MASK)
#define LED0_TOGGLE         (PORTB ^=  LED0_MASK)
/** @} */

/**
 * @brief xtimer configuration values
 * @{
 */
#define XTIMER_MASK                 (0xffff0000)
#define XTIMER_SHIFT                (2)
#define XTIMER_BACKOFF              (40)
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
