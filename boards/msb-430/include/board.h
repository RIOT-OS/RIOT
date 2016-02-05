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

#ifndef MSB_BOARD_H_
#define MSB_BOARD_H_

#include "board-conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/*  for correct inclusion of <msp430.h> */
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
#define XTIMER_SHIFT_ON_COMPARE     (4)
#define XTIMER_BACKOFF              (40)
/** @} */

/* MSB430 core */
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

#ifdef __cplusplus
}
#endif

#include "board-conf.h"

/** @} */
#endif /*  MSB_BOARD_H_ */
