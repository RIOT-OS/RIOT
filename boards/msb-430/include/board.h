/*
Copyright 2009, Freie Universitaet Berlin (FUB). All rights reserved.

These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
-------------------------------------------------------------------------------
This file is part of RIOT.

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

RIOT is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see http://www.gnu.org/licenses/ .
--------------------------------------------------------------------------------
For further information and questions please use the web site
    http://scatterweb.mi.fu-berlin.de
and the mailinglist (subscription via web site)
    scatterweb@lists.spline.inf.fu-berlin.de
*******************************************************************************/

/**
 * @defgroup    boards_msb430 MSB-430
 * @ingroup     boards
 * @brief       Support for the ScatterWeb MSB-430 board
 *
 * <h2>Compontents</h2>
 * \li MSP430
 * \li CC1100
 *
 * @{
 *
 * @file        board.h
 * @brief       Central defnitions for the ScatterWeb MSB-430 board
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics, FeuerWhere project
 */

#ifndef _MSB_BOARD_H
#define _MSB_BOARD_H

#include "board-conf.h"

// for correct inclusion of <msp430.h>
#ifndef __MSP430F1612__
#define __MSP430F1612__
#endif

//MSB430 core
#define MSP430_INITIAL_CPU_SPEED    2457600uL
#define F_CPU                       MSP430_INITIAL_CPU_SPEED
#define F_RC_OSCILLATOR             32768
#define MSP430_HAS_DCOR             1
#define MSP430_HAS_EXTERNAL_CRYSTAL 0

/* LEDs ports MSB430 */
#define LEDS_PxDIR P5DIR
#define LEDS_PxOUT P5OUT
#define LEDS_CONF_RED       0x80
#define LEDS_CONF_GREEN     0x00
#define LEDS_CONF_YELLOW    0x00

#define LED_RED_ON          LEDS_PxOUT &=~LEDS_CONF_RED
#define LED_RED_OFF         LEDS_PxOUT |= LEDS_CONF_RED
#define LED_RED_TOGGLE      LEDS_PxOUT ^= LEDS_CONF_RED

#include "board-conf.h"

typedef uint8_t radio_packet_length_t;

/** @} */
#endif // _MSB_BOARD_H
