/*
 * Copyright (C) 2014 INRIA
 *               2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef Z1_BOARD_H_
#define Z1_BOARD_H_

/**
 * @defgroup    boards_z1 Zolertia Z1
 * @ingroup     boards
 * @brief       Support for the Zolertia Z1 board.
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
 * @brief   Xtimer configuration
 * @{
 */
#define XTIMER                      (0)
#define XTIMER_CHAN                 (0)
#define XTIMER_MASK                 (0xffff0000)
#define XTIMER_SHIFT_ON_COMPARE     (4)
#define XTIMER_BACKOFF              (40)
/** @} */

/**
 * @brief   Standard input/output device configuration
 * @{
 */
#define UART_STDIO_DEV              (UART_DEV(0))
#define UART_STDIO_BAUDRATE         (115200U)
#define UART_STDIO_RX_BUFSIZE       (64U)
/** @} */

/**
 * @brief   CPU core configuration
 *
 * @todo    Move this to the periph_conf.h
 * @{
 */
#define MSP430_INITIAL_CPU_SPEED    8000000uL
#ifndef F_CPU
#define F_CPU                       MSP430_INITIAL_CPU_SPEED
#endif
#define F_RC_OSCILLATOR             32768
#define MSP430_HAS_DCOR             0
#define MSP430_HAS_EXTERNAL_CRYSTAL 1
/** @} */

/**
 * @brief   LED pin definitions and handlers
 * @{
 */
#define LED0_PIN                    GPIO_PIN(4, 0)
#define LED1_PIN                    GPIO_PIN(4, 1)
#define LED2_PIN                    GPIO_PIN(4, 2)

#define LED_OUT_REG                 P5OUT
#define LED0_MASK                   (0x10)
#define LED1_MASK                   (0x20)
#define LED2_MASK                   (0x40)

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
 * @brief   User button configuration
 * @{
 */
#define USER_BTN_PxIN      P2IN
#define USER_BTN_MASK      0x20

#define USER_BTN_PRESSED   ((USER_BTN_PxIN & USER_BTN_MASK) == 0)
#define USER_BTN_RELEASED  ((USER_BTN_PxIN & USER_BTN_MASK) != 0)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /*  Z1_BOARD_H_ */
