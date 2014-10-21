/*
 * Copyright 2009, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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
 * @file
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
