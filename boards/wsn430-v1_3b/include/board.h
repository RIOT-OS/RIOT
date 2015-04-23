/*
 * Copyright (C) 2013 Milan Babel <babel@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_wsn430-v1_3b WSN430 v1.3b
 * @ingroup     boards
 * @brief       Support for the Senslab WSN430 v1.3b board
 *
 * <h2>Compontents</h2>
 * \li MSP430
 * \li CC1100
 *
 * @{
 *
 * @file
 * @brief       Basic definitions for the Senslab WSN430 v1.3b board
 *
 * @author      Milan Babel <babel@inf.fu-berlin.de>
 */

#ifndef WSN_BOARD_H
#define WSN_BOARD_H

#include "board-conf.h"

#ifdef __cplusplus
extern "C" {
#endif

// for correct inclusion of <msp430.h>
#ifndef MSP430F1611__
#define MSP430F1611__
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
#define LEDS_CONF_BLUE  0x06

#define LED_RED_ON          LEDS_PxOUT &=~LEDS_CONF_RED
#define LED_RED_OFF         LEDS_PxOUT |= LEDS_CONF_RED
#define LED_RED_TOGGLE      LEDS_PxOUT ^= LEDS_CONF_RED

#define LED_GREEN_ON        LEDS_PxOUT &=~LEDS_CONF_GREEN
#define LED_GREEN_OFF       LEDS_PxOUT |= LEDS_CONF_GREEN
#define LED_GREEN_TOGGLE    LEDS_PxOUT ^= LEDS_CONF_GREEN

#define LED_BLUE_ON         LEDS_PxOUT &=~LEDS_CONF_BLUE
#define LED_BLUE_OFF        LEDS_PxOUT |= LEDS_CONF_BLUE
#define LED_BLUE_TOGGLE     LEDS_PxOUT ^= LEDS_CONF_BLUE

#ifdef __cplusplus
}
#endif

#include <msp430x16x.h>

typedef uint8_t radio_packet_length_t;

/** @} */
#endif // _WSN_BOARD_H
