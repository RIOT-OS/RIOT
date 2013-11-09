/**
  * board.h - TelosB Board.
  * Copyright (C) 2013 INRIA
  *
  * This source code is licensed under the GNU Lesser General Public License,
  * Version 2.  See the file LICENSE for more details.
  */

#ifndef _TELOSB_BOARD_H
#define _TELOSB_BOARD_H

/**
 * @defgroup    TelosB
 * @ingroup		TelosB
 *
<h2>Compontents</h2>
\li MSP430
\li CC2420

* @{
*/

/**
 * @file
 * @brief		TelosB Board
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *
 */

//MSB430 core
#define MSP430_INITIAL_CPU_SPEED    8000000uL
#define MSP430_HAS_DCOR             0
#define MSP430_HAS_EXTERNAL_CRYSTAL 1

/* LEDs ports MSB430 */
#define LEDS_PxDIR P5DIR
#define LEDS_PxOUT P5OUT
#define LEDS_CONF_RED		0x10
#define LEDS_CONF_GREEN		0x20
#define LEDS_CONF_BLUE	0x40

#define LED_RED_ON      LEDS_PxOUT &=~LEDS_CONF_RED
#define LED_RED_OFF     LEDS_PxOUT |= LEDS_CONF_RED
#define LED_RED_TOGGLE     LEDS_PxOUT ^= LEDS_CONF_RED

#define LED_GREEN_ON      LEDS_PxOUT &=~LEDS_CONF_GREEN
#define LED_GREEN_OFF     LEDS_PxOUT |= LEDS_CONF_GREEN
#define LED_GREEN_TOGGLE     LEDS_PxOUT ^= LEDS_CONF_GREEN

#define LED_BLUE_ON      LEDS_PxOUT &=~LEDS_CONF_BLUE
#define LED_BLUE_OFF     LEDS_PxOUT |= LEDS_CONF_BLUE
#define LED_BLUE_TOGGLE     LEDS_PxOUT ^= LEDS_CONF_BLUE

#include <stdint.h>
#include <msp430x16x.h>

typedef uint8_t radio_packet_length_t;

/** @} */
#endif // _TELOSB_BOARD_H
