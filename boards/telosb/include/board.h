/*
 * Copyright (C) 2013, 2014 INRIA
 *               2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_telosb TelosB
 * @ingroup     boards
 * @brief       Support for the TelosB board
 *
 * <h2>Compontents</h2>
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

#ifndef TELOSB_BOARD_H
#define TELOSB_BOARD_H

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
 * @brief   Override default baudrate for STDIO
 * @{
 */
#ifndef UART_STDIO_BAUDRATE
#define UART_STDIO_BAUDRATE         (9600)
#endif
/** @} */

/**
 * @brief   Xtimer configuration
 * @{
 */
#define XTIMER_DEV                  (0)
#define XTIMER_CHAN                 (0)
#define XTIMER_WIDTH                (16)
#define XTIMER_BACKOFF              (40)
/** @} */

/**
 * @brief   CPU core configuration
 *
 * @todo    Move this to the periph_conf.h
 * @{
 */
#define MSP430_INITIAL_CPU_SPEED    2457600uL
#define F_CPU                       MSP430_INITIAL_CPU_SPEED
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
 * @brief   Definition of the interface to the CC2420 radio
 */
#define CC2420_PARAMS_BOARD   {.spi        = SPI_DEV(0), \
                               .spi_clk    = SPI_CLK_1MHZ , \
                               .pin_cs     = GPIO_PIN(P4, 2), \
                               .pin_fifo   = GPIO_PIN(P1, 3), \
                               .pin_fifop  = GPIO_PIN(P1, 0), \
                               .pin_cca    = GPIO_PIN(P1, 4), \
                               .pin_sfd    = GPIO_PIN(P4, 1), \
                               .pin_vrefen = GPIO_PIN(P4, 5), \
                               .pin_reset  = GPIO_PIN(P4, 6)}

#ifdef __cplusplus
}
#endif

/** @} */
#endif /*  TELOSB_BOARD_H */
