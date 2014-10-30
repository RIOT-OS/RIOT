/*
 * Copyright 2009, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_msb430h MSB-430H
 * @ingroup     boards
 * @brief       Support for the ScatterWeb MSB-430H board
 * @{
 *
 * @file
 * @brief       Basic definitions for the MSB-430H board
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics, FeuerWhere project
 */

#ifndef _MSB_BOARD_H
#define _MSB_BOARD_H

// for correct inclusion of <msp430.h>
#ifndef __MSP430F1612__
#define __MSP430F1612__
#endif

//MSB430 core
#define MSP430_INITIAL_CPU_SPEED    7372800uL
#define F_CPU                       MSP430_INITIAL_CPU_SPEED
#define F_RC_OSCILLATOR             32768
#define MSP430_HAS_DCOR             1
#define MSP430_HAS_EXTERNAL_CRYSTAL 1

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

/**
 * @brief Boards has a radio
 */
#define HAS_RADIO

typedef uint8_t radio_packet_length_t;

/** @} */
#endif // _MSB_BOARD_H
