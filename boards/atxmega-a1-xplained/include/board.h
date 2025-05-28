/*
 * Copyright (C) 2021 Gerson Fernando Budke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_atxmega-a1-xplained
 * @{
 *
 * @file
 * @brief       Board specific definitions for the ATxmegaA1 Xplained board.
 *
 * @author      Gerson Fernando Budke <nandojve@gmail.com>
 */

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
 * @brief Use the UART-2 for STDIO on this board
 */
#define STDIO_UART_DEV      UART_DEV(2)

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
#define LED_PORT            PORTE

#define LED0_PIN            GPIO_PIN(PORT_E, 0)
#define LED0_MODE           GPIO_OUT
#define LED0_MASK           (PIN0_bm)
#define LED0_ON             (LED_PORT.OUTCLR = LED0_MASK)
#define LED0_OFF            (LED_PORT.OUTSET = LED0_MASK)
#define LED0_TOGGLE         (LED_PORT.OUTTGL = LED0_MASK)

#define LED1_PIN            GPIO_PIN(PORT_E, 1)
#define LED1_MODE           GPIO_OUT
#define LED1_MASK           (PIN1_bm)
#define LED1_ON             (LED_PORT.OUTCLR = LED1_MASK)
#define LED1_OFF            (LED_PORT.OUTSET = LED1_MASK)
#define LED1_TOGGLE         (LED_PORT.OUTTGL = LED1_MASK)

#define LED2_PIN            GPIO_PIN(PORT_E, 2)
#define LED2_MODE           GPIO_OUT
#define LED2_MASK           (PIN2_bm)
#define LED2_ON             (LED_PORT.OUTCLR = LED2_MASK)
#define LED2_OFF            (LED_PORT.OUTSET = LED2_MASK)
#define LED2_TOGGLE         (LED_PORT.OUTTGL = LED2_MASK)

#define LED3_PIN            GPIO_PIN(PORT_E, 3)
#define LED3_MODE           GPIO_OUT
#define LED3_MASK           (PIN3_bm)
#define LED3_ON             (LED_PORT.OUTCLR = LED3_MASK)
#define LED3_OFF            (LED_PORT.OUTSET = LED3_MASK)
#define LED3_TOGGLE         (LED_PORT.OUTTGL = LED3_MASK)

#define LED4_PIN            GPIO_PIN(PORT_E, 4)
#define LED4_MODE           GPIO_OUT
#define LED4_MASK           (PIN4_bm)
#define LED4_ON             (LED_PORT.OUTCLR = LED4_MASK)
#define LED4_OFF            (LED_PORT.OUTSET = LED4_MASK)
#define LED4_TOGGLE         (LED_PORT.OUTTGL = LED4_MASK)

#define LED5_PIN            GPIO_PIN(PORT_E, 5)
#define LED5_MODE           GPIO_OUT
#define LED5_MASK           (PIN5_bm)
#define LED5_ON             (LED_PORT.OUTCLR = LED5_MASK)
#define LED5_OFF            (LED_PORT.OUTSET = LED5_MASK)
#define LED5_TOGGLE         (LED_PORT.OUTTGL = LED5_MASK)

#define LED6_PIN            GPIO_PIN(PORT_E, 6)
#define LED6_MODE           GPIO_OUT
#define LED6_MASK           (PIN6_bm)
#define LED6_ON             (LED_PORT.OUTCLR = LED6_MASK)
#define LED6_OFF            (LED_PORT.OUTSET = LED6_MASK)
#define LED6_TOGGLE         (LED_PORT.OUTTGL = LED6_MASK)

#define LED7_PIN            GPIO_PIN(PORT_E, 7)
#define LED7_MODE           GPIO_OUT
#define LED7_MASK           (PIN7_bm)
#define LED7_ON             (LED_PORT.OUTCLR = LED7_MASK)
#define LED7_OFF            (LED_PORT.OUTSET = LED7_MASK)
#define LED7_TOGGLE         (LED_PORT.OUTTGL = LED7_MASK)

#define LED_PORT_MASK       (LED0_MASK | LED1_MASK | LED2_MASK | LED3_MASK | \
                             LED4_MASK | LED5_MASK | LED6_MASK | LED7_MASK)
/** @} */

/**
 * @name    Button pin configuration
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PORT_D, 0)
#define BTN0_MODE           (GPIO_IN | GPIO_OPC_PU | GPIO_SLEW_RATE)
#define BTN0_INT_FLANK      (GPIO_ISC_FALLING | GPIO_LVL_LOW)

#define BTN1_PIN            GPIO_PIN(PORT_D, 1)
#define BTN1_MODE           (GPIO_IN | GPIO_OPC_PU | GPIO_SLEW_RATE)
#define BTN1_INT_FLANK      (GPIO_ISC_FALLING | GPIO_LVL_LOW)

#define BTN2_PIN            GPIO_PIN(PORT_D, 2)
#define BTN2_MODE           (GPIO_IN | GPIO_OPC_PU | GPIO_SLEW_RATE)
#define BTN2_INT_FLANK      (GPIO_ISC_FALLING | GPIO_LVL_LOW)

#define BTN3_PIN            GPIO_PIN(PORT_D, 3)
#define BTN3_MODE           (GPIO_IN | GPIO_OPC_PU | GPIO_SLEW_RATE)
#define BTN3_INT_FLANK      (GPIO_ISC_FALLING | GPIO_LVL_LOW)

#define BTN4_PIN            GPIO_PIN(PORT_D, 4)
#define BTN4_MODE           (GPIO_IN | GPIO_OPC_PU | GPIO_SLEW_RATE)
#define BTN4_INT_FLANK      (GPIO_ISC_FALLING | GPIO_LVL_LOW)

#define BTN5_PIN            GPIO_PIN(PORT_D, 5)
#define BTN5_MODE           (GPIO_IN | GPIO_OPC_PU | GPIO_SLEW_RATE)
#define BTN5_INT_FLANK      (GPIO_ISC_FALLING | GPIO_LVL_LOW)

#define BTN6_PIN            GPIO_PIN(PORT_R, 0)
#define BTN6_MODE           (GPIO_IN | GPIO_OPC_PU | GPIO_SLEW_RATE)
#define BTN6_INT_FLANK      (GPIO_ISC_FALLING | GPIO_LVL_LOW)

#define BTN7_PIN            GPIO_PIN(PORT_R, 1)
#define BTN7_MODE           (GPIO_IN | GPIO_OPC_PU | GPIO_SLEW_RATE)
#define BTN7_INT_FLANK      (GPIO_ISC_FALLING | GPIO_LVL_LOW)
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

/** @} */
