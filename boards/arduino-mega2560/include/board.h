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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Define the nominal CPU core clock in this board
 */
#define F_CPU               (16000000L)


/**
 * Assign the hardware timer
 */
#define HW_TIMER            TIMER_0

/**
* @name Define UART device and baudrate for stdio
* @{
*/
#define STDIO               UART_0
#define STDIO_BAUDRATE      (9600U)
#define STDIO_RX_BUFSIZE    (64U)
/** @} */

/**
 * @name LED pin definitions
 * @{
 */
#define LED_PORT            PORTB
#define LED_PIN             (1 << 7)
/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED_ENABLE_PORT     DDRB |= (1 << DDB7)
#define LED_ON              LED_PORT |= LED_PIN
#define LED_OFF             LED_PORT &= ~LED_PIN
#define LED_TOGGLE          LED_PORT ^= LED_PIN;

/* for compatability to other boards */
#define LED_GREEN_ON        LED_ON
#define LED_GREEN_OFF       LED_OFF
#define LED_GREEN_TOGGLE    LED_TOGGLE
#define LED_RED_ON          /* not available */
#define LED_RED_OFF         /* not available */
#define LED_RED_TOGGLE      /* not available */
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
