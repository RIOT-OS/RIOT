/*
 * Copyright (C) 2013, 2014 INRIA
 *               2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_telosb
 *
 * <h2>Components</h2>
 * \li MSP430
 * \li CC2420
 *
 * @{
 *
 * @file
 * @brief       Basic definitions for the TelosB board
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Define the CPU model for the <msp430.h>
 */
#ifndef __MSP430F1611__
#define __MSP430F1611__
#endif

/**
 * @name    Override default baudrate for STDIO
 * @{
 */
#ifndef STDIO_UART_BAUDRATE
#define STDIO_UART_BAUDRATE         (9600)
#endif
/** @} */

/**
 * @name    Xtimer configuration
 * @{
 */
#define XTIMER_WIDTH                (16)
#define XTIMER_BACKOFF              (40)
/** @} */

/**
 * @name    LED pin definitions and handlers
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
 * @name    Definition of the interface to the CC2420 radio
 * @{
 */
#define CC2420_PARAM_SPI_CLK        (SPI_CLK_1MHZ)
#define CC2420_PARAM_CS             GPIO_PIN(P4, 2)
#define CC2420_PARAM_FIFO           GPIO_PIN(P1, 3)
#define CC2420_PARAM_FIFOP          GPIO_PIN(P1, 0)
#define CC2420_PARAM_CCA            GPIO_PIN(P1, 4)
#define CC2420_PARAM_SFD            GPIO_PIN(P4, 1)
#define CC2420_PARAM_VREFEN         GPIO_PIN(P4, 5)
#define CC2420_PARAM_RESET          GPIO_PIN(P4, 6)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* BOARD_H */
