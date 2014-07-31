/*
 * Copyright (C) 2013 INRIA
 *
 * This source code is licensed under the GNU Lesser General Public License,
 * Version 2.  See the file LICENSE for more details.
 */

/**
 * @defgroup    boards_telosb TelosB
 * @ingroup     boards
 * @brief       Support for the TelosB board
 *
 * <h2>Compontents</h2>
 * \li MSP430
 * \li CC2420
 *
 * @{
 *
 * @file        board.h
 * @brief       Basic definitions for the TelosB board
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 */

#ifndef _TELOSB_BOARD_H
#define _TELOSB_BOARD_H

// for correct inclusion of <msp430.h>
#ifndef __MSP430F1611__
#define __MSP430F1611__
#endif

//TelosB core
#define MSP430_INITIAL_CPU_SPEED    2457600uL
#define F_CPU                       MSP430_INITIAL_CPU_SPEED
#define F_RC_OSCILLATOR             32768
#define MSP430_HAS_DCOR             0
#define MSP430_HAS_EXTERNAL_CRYSTAL 1

/* LEDs ports MSB430 */
#define LEDS_PxDIR P5DIR
#define LEDS_PxOUT P5OUT
#define LEDS_CONF_RED       0x10
#define LEDS_CONF_GREEN     0x20
#define LEDS_CONF_BLUE      0x40

#define LED_RED_ON          LEDS_PxOUT &=~LEDS_CONF_RED
#define LED_RED_OFF         LEDS_PxOUT |= LEDS_CONF_RED
#define LED_RED_TOGGLE      LEDS_PxOUT ^= LEDS_CONF_RED

#define LED_GREEN_ON        LEDS_PxOUT &=~LEDS_CONF_GREEN
#define LED_GREEN_OFF       LEDS_PxOUT |= LEDS_CONF_GREEN
#define LED_GREEN_TOGGLE    LEDS_PxOUT ^= LEDS_CONF_GREEN

#define LED_BLUE_ON         LEDS_PxOUT &=~LEDS_CONF_BLUE
#define LED_BLUE_OFF        LEDS_PxOUT |= LEDS_CONF_BLUE
#define LED_BLUE_TOGGLE     LEDS_PxOUT ^= LEDS_CONF_BLUE

#include <stdint.h>

typedef uint8_t radio_packet_length_t;

/** @} */
#endif // _TELOSB_BOARD_H
