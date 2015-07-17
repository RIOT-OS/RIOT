/*
 * board.h - Wismote Board.
 * Copyright (C) 2015, Sumankumar Panchal <suman@ece.iisc.ernet.in>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_wismote Wismote
 * @ingroup     boards
 * @brief       Support for the Wismote board
 * @{
 *
 * @file
 * @brief       Wismote board configuration
 *
 * @author      Sumankumar Panchal <suman@ece.iisc.ernet.in>
 */

#ifndef _WISMOTE_BOARD_H
#define _WISMOTE_BOARD_H

#include "bitarithm.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MSP430F5437__
#define __MSP430F5437__
#endif

/* MSP430x core */
#define MSP430_INITIAL_CPU_SPEED    8000000uL
#define F_CPU                       MSP430_INITIAL_CPU_SPEED
#define F_RC_OSCILLATOR             32768
#define MSP430_HAS_DCOR             0
#define MSP430_HAS_EXTERNAL_CRYSTAL 1

/* LEDs ports MSB430 */
#define LEDS_CONF_RED   0x40
#define LEDS_CONF_GREEN 0x10
#define LEDS_CONF_BLUE  0x04

#define LED_RED_ON	P8OUT &= ~LEDS_CONF_RED
#define LED_RED_OFF     P8OUT |= LEDS_CONF_RED
#define LED_RED_TOGGLE  P8OUT ^= LEDS_CONF_RED

#define LED_GREEN_ON   P2OUT &= ~LEDS_CONF_GREEN
#define LED_GREEN_OFF  P2OUT |= LEDS_CONF_GREEN
#define LED_GREEN_TOGGLE  P2OUT ^= LEDS_CONF_GREEN

#define LED_BLUE_ON	P5OUT &= ~LEDS_CONF_BLUE
#define LED_BLUE_OFF  P5OUT |= LEDS_CONF_BLUE
#define LED_BLUE_TOGGLE	P5OUT ^= LEDS_CONF_BLUE

#ifdef __cplusplus
}
#endif

#include <stdint.h>

typedef uint8_t radio_packet_length_t;

/** @} */
#endif /* _WISMOTE_BOARD_H */
