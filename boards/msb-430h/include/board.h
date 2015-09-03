/*
 * Copyright 2009, 2015 Freie Universität Berlin
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
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef MSB_BOARD_H_
#define MSB_BOARD_H_

#ifdef __cplusplus
extern "C" {
#endif

/* for correct inclusion of <msp430.h> */
#ifndef __MSP430F1612__
#define __MSP430F1612__
#endif

/**
 * @brief   Xtimer configuration
 * @{
 */
#define XTIMER                      (0)
#define XTIMER_CHAN                 (0)
#define XTIMER_MASK                 (0xffff0000)
/** @} */

/**
 * @brief   Defines for compatibility with hwtimer
 * @deprecated
 */
#define HW_TIMER                    (0)

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

#ifdef __cplusplus
}
#endif

#include "board-conf.h"

/** @} */
#endif /* MSB_BOARD_H_ */
