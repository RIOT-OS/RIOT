/*
 * board.h - Zolertia Z1 Board.
 * Copyright (C) 2014 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef Z1_BOARD_H
#define Z1_BOARD_H

/**
 * @defgroup    boards_z1 Zolertia Z1
 * @ingroup     boards
 * @brief       Support for the Zolertia Z1 board.
 *
<h2>Components</h2>
\li MSP430F2617
\li CC2420

* @{
*/

/**
 * @file
 * @brief       Zolertia Z1 board configuration
 *
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 *
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MSP430F2617__
#define MSP430F2617__
#endif

// MSP430 core
#define MSP430_INITIAL_CPU_SPEED    8000000uL
#ifndef F_CPU
#define F_CPU                       MSP430_INITIAL_CPU_SPEED
#endif
#define F_RC_OSCILLATOR             32768
#define MSP430_HAS_DCOR             0
#define MSP430_HAS_EXTERNAL_CRYSTAL 1

// LEDs ports
#define LEDS_PxDIR P5DIR
#define LEDS_PxOUT P5OUT
#define LEDS_CONF_RED      0x10
#define LEDS_CONF_BLUE     0x20
#define LEDS_CONF_GREEN    0x40

#define LED_RED_ON         LEDS_PxOUT &=~LEDS_CONF_RED
#define LED_RED_OFF        LEDS_PxOUT |= LEDS_CONF_RED
#define LED_RED_TOGGLE     LEDS_PxOUT ^= LEDS_CONF_RED

#define LED_GREEN_ON       LEDS_PxOUT &=~LEDS_CONF_GREEN
#define LED_GREEN_OFF      LEDS_PxOUT |= LEDS_CONF_GREEN
#define LED_GREEN_TOGGLE   LEDS_PxOUT ^= LEDS_CONF_GREEN

#define LED_BLUE_ON        LEDS_PxOUT &=~LEDS_CONF_BLUE
#define LED_BLUE_OFF       LEDS_PxOUT |= LEDS_CONF_BLUE
#define LED_BLUE_TOGGLE    LEDS_PxOUT ^= LEDS_CONF_BLUE


// User-button port
#define USER_BTN_PxIN      P2IN
#define USER_BTN_MASK      0x20

#define USER_BTN_PRESSED   ((USER_BTN_PxIN & USER_BTN_MASK) == 0)
#define USER_BTN_RELEASED  ((USER_BTN_PxIN & USER_BTN_MASK) != 0)

typedef uint8_t radio_packet_length_t;

#ifdef __cplusplus
}
#endif

/** @} */
#endif // Z1_BOARD_H
