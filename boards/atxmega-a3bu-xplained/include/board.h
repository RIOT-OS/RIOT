/*
 * Copyright (C) 2021 Gerson Fernando Budke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_atxmega-a3bu-xplained
 * @{
 *
 * @file
 * @brief       Board specific definitions for the ATxmegaA3BU Xplained board.
 *
 * @author      Gerson Fernando Budke <nandojve@gmail.com>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "macros/units.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Clock configuration
 */
#define CLOCK_CORECLOCK     MHZ(32)

/**
 * @name   Baudrate for STDIO terminal
 *
 * The standard configuration for STDIO in cpu/atxmega/periph/uart.c
 * is to use double speed.
 *
 * For 32MHz F_CPU following Baudrate have good error rates
 *  115200
 *
 * Matches this with BAUD in Board/Makefile.include
 *
 * @{
 */
#ifndef STDIO_UART_BAUDRATE
#define STDIO_UART_BAUDRATE (115200U)
#endif
/** @} */

/**
 * @name   LED pin definitions and handlers
 * @{
 */
#define LED_PORT            PORTR

#define LED0_PIN            GPIO_PIN(PORT_R, 0)
#define LED0_MODE           GPIO_OUT
#define LED0_MASK           (PIN0_bm)
#define LED0_ON             (LED_PORT.OUTCLR = LED0_MASK)
#define LED0_OFF            (LED_PORT.OUTSET = LED0_MASK)
#define LED0_TOGGLE         (LED_PORT.OUTTGL = LED0_MASK)

#define LED1_PIN            GPIO_PIN(PORT_R, 1)
#define LED1_MODE           GPIO_OUT
#define LED1_MASK           (PIN1_bm)
#define LED1_ON             (LED_PORT.OUTCLR = LED1_MASK)
#define LED1_OFF            (LED_PORT.OUTSET = LED1_MASK)
#define LED1_TOGGLE         (LED_PORT.OUTTGL = LED1_MASK)

#define LED_PORT_MASK       (LED0_MASK | LED1_MASK)
/** @} */

/**
 * @name    Button pin configuration
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PORT_E, 5)
#define BTN0_MODE           (GPIO_IN | GPIO_OPC_PU | GPIO_SLEW_RATE)
#define BTN0_INT_FLANK      (GPIO_ISC_FALLING | GPIO_LVL_LOW)

#define BTN1_PIN            GPIO_PIN(PORT_F, 1)
#define BTN1_MODE           (GPIO_IN | GPIO_OPC_PU | GPIO_SLEW_RATE)
#define BTN1_INT_FLANK      (GPIO_ISC_FALLING | GPIO_LVL_LOW)

#define BTN2_PIN            GPIO_PIN(PORT_F, 2)
#define BTN2_MODE           (GPIO_IN | GPIO_OPC_PU | GPIO_SLEW_RATE)
#define BTN2_INT_FLANK      (GPIO_ISC_FALLING | GPIO_LVL_LOW)
/** @} */

/**
 * @name xtimer configuration values
 * if XTIMER_HZ > 1MHz then (XTIMER_HZ != (1000000ul << XTIMER_SHIFT))
 * if XTIMER_HZ < 1MHz then ((XTIMER_HZ << XTIMER_SHIFT) != 1000000ul)
 *
 * 32MHz Core Clock
 * XTIMER_HZ 4000000 (clkdiv 8 )    XTIMER_SHIFT 2
 * XTIMER_HZ 1000000 ()             XTIMER_SHIFT 0
 * XTIMER_HZ  500000 (clkdiv 64)    XTIMER_SHIFT 1
 * XTIMER_HZ  250000 (clkdiv 128)   XTIMER_SHIFT 2
 * XTIMER_HZ   31250 (clkdiv 1024)  XTIMER_SHIFT 5
 *
 * @{
 */
#define XTIMER_DEV          TIMER_DEV(0)
#define XTIMER_CHAN         (0)
#define XTIMER_WIDTH        (16)
#define XTIMER_HZ           KHZ(500)
#define XTIMER_BACKOFF      (150)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
