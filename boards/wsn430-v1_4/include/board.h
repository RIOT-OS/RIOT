/*
 * Copyright (C) 2013 Milan Babel <babel@inf.fu-berlin.de>
 *
 * This source code is licensed under the GNU Lesser General Public License,
 * Version 2.  See the file LICENSE for more details.
 */

/**
 * @defgroup    boards_wsn430-v1_4 WSN430 v1.4
 * @ingroup     boards
 * @brief       Support for the Senslab WSN430 v1.4 board
 *
 * <h2>Compontents</h2>
 * \li MSP430
 * \li CC2420
 *
 * @{
 *
 * @file
 * @brief       Basic definitions for the Senslab WSN430 v1.4 board
 *
 * @author      Milan Babel <babel@inf.fu-berlin.de>
 */

#ifndef _WSN_BOARD_H
#define _WSN_BOARD_H

#include "board-conf.h"

// for correct inclusion of <msp430.h>
#ifndef __MSP430F1611__
#define __MSP430F1611__
#endif

//MSB430 core
#define MSP430_INITIAL_CPU_SPEED    800000uL
#define F_CPU                       MSP430_INITIAL_CPU_SPEED
#define F_RC_OSCILLATOR             32768
#define MSP430_HAS_DCOR             0
#define MSP430_HAS_EXTERNAL_CRYSTAL 1

/* LEDs ports MSB430 */
#define LEDS_PxDIR P5DIR
#define LEDS_PxOUT P5OUT
#define LEDS_CONF_RED       0x04
#define LEDS_CONF_GREEN     0x05
#define LEDS_CONF_BLUE      0x06

#define LED_RED_ON          LEDS_PxOUT &=~LEDS_CONF_RED
#define LED_RED_OFF         LEDS_PxOUT |= LEDS_CONF_RED
#define LED_RED_TOGGLE      LEDS_PxOUT ^= LEDS_CONF_RED

#define LED_GREEN_ON        LEDS_PxOUT &=~LEDS_CONF_GREEN
#define LED_GREEN_OFF       LEDS_PxOUT |= LEDS_CONF_GREEN
#define LED_GREEN_TOGGLE    LEDS_PxOUT ^= LEDS_CONF_GREEN

#define LED_BLUE_ON         LEDS_PxOUT &=~LEDS_CONF_BLUE
#define LED_BLUE_OFF        LEDS_PxOUT |= LEDS_CONF_BLUE
#define LED_BLUE_TOGGLE     LEDS_PxOUT ^= LEDS_CONF_BLUE

#include <msp430x16x.h>

/**
 * @brief Boards has a radio
 */
#define HAS_RADIO
typedef uint8_t radio_packet_length_t;

/** @} */
#endif // _WSN_BOARD_H
