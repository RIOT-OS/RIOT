/*
 * Copyright (C) 2014 INRIA
 *               2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef BOARD_H
#define BOARD_H

/**
 * @ingroup     boards_z1
 *
<h2>Components</h2>
\li MSP430F2617
\li CC2420

* @{
*
 * @file
 * @brief       Zolertia Z1 board configuration
 *
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 */

#include <stdint.h>

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Define the CPU model for the <msp430.h>
 */
#ifndef __MSP430F2617__
#define __MSP430F2617__
#endif

/**
 * @name    Xtimer configuration
 * @{
 */
#define XTIMER_WIDTH                (16)
#define XTIMER_BACKOFF              (40)
/** @} */

/**
 * @name    CPU core configuration
 * @{
 */
/** @todo   Move this to the periph_conf.h */
#define MSP430_INITIAL_CPU_SPEED    8000000uL
#ifndef F_CPU
#define F_CPU                       MSP430_INITIAL_CPU_SPEED
#endif
#define F_RC_OSCILLATOR             32768
#define MSP430_HAS_DCOR             0
#define MSP430_HAS_EXTERNAL_CRYSTAL 1
/** @} */

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN                    GPIO_PIN(5, 4)
#define LED1_PIN                    GPIO_PIN(5, 6)
#define LED2_PIN                    GPIO_PIN(5, 5)

#define LED_OUT_REG                 P5OUT
#define LED0_MASK                   (0x10)
#define LED1_MASK                   (0x40)
#define LED2_MASK                   (0x20)

#define LED0_ON                     (LED_OUT_REG &=~LED0_MASK)
#define LED0_OFF                    (LED_OUT_REG |= LED0_MASK)
#define LED0_TOGGLE                 (LED_OUT_REG ^= LED0_MASK)

#define LED1_ON                     (LED_OUT_REG &=~LED1_MASK)
#define LED1_OFF                    (LED_OUT_REG |= LED1_MASK)
#define LED1_TOGGLE                 (LED_OUT_REG ^= LED1_MASK)

#define LED2_ON                     (LED_OUT_REG &=~LED2_MASK)
#define LED2_OFF                    (LED_OUT_REG |= LED2_MASK)
#define LED2_TOGGLE                 (LED_OUT_REG ^= LED2_MASK)
/** @} */

/**
 * @name    User button configuration
 * @{
 */
#define BTN0_PIN            P2IN
#define BTN0_MASK           (0x20)
#define BTN0_MODE           GPIO_IN

#define BTN0_PRESSED        ((BTN0_PIN & BTN0_MASK) == 0)
#define BTN0_RELEASED       ((BTN0_PIN & BTN0_MASK) != 0)
/** @} */

/**
 * @name    Definition of the interface to the CC2420 radio
 * @{
 */
#define CC2420_PARAM_CS            GPIO_PIN(P3, 0)
#define CC2420_PARAM_FIFO          GPIO_PIN(P1, 3)
#define CC2420_PARAM_FIFOP         GPIO_PIN(P1, 2)
#define CC2420_PARAM_CCA           GPIO_PIN(P1, 4)
#define CC2420_PARAM_SFD           GPIO_PIN(P4, 1)
#define CC2420_PARAM_VREFEN        GPIO_PIN(P4, 5)
#define CC2420_PARAM_RESET         GPIO_PIN(P4, 6)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* BOARD_H */
