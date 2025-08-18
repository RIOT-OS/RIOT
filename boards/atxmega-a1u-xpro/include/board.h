/*
 * SPDX-FileCopyrightText: 2021 Gerson Fernando Budke
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_atxmega-a1u-xpro
 * @{
 *
 * @file
 * @brief       Board specific definitions for the ATxmegaA1U Xplained Pro board.
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
#define LED_PORT            PORTQ

#define LED0_PIN            GPIO_PIN(PORT_Q, 3)
#define LED0_MODE           GPIO_OUT
#define LED0_MASK           (PIN3_bm)
#define LED0_ON             (LED_PORT.OUTCLR = LED0_MASK)
#define LED0_OFF            (LED_PORT.OUTSET = LED0_MASK)
#define LED0_TOGGLE         (LED_PORT.OUTTGL = LED0_MASK)

#define LED_PORT_MASK       (LED0_MASK)
/** @} */

/**
 * @name    Button pin configuration
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PORT_Q, 2)
#define BTN0_MODE           (GPIO_IN | GPIO_OPC_PU | GPIO_SLEW_RATE)
#define BTN0_INT_FLANK      (GPIO_ISC_FALLING | GPIO_LVL_LOW)
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
