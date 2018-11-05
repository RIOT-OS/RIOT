/*
 * Copyright (C) 2016 RWTH Aachen, Josua Arndt
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_jiminy-mega256rfr2
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the Jiminy Mega 256rfr2 board
 *
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <avr/io.h>
#include "periph_cpu.h"
#include "atmega_regs_common.h"
#include "periph_cpu_common.h"

/**
 * @name   Clock configuration
 * @{
 */
#define CLOCK_CORECLOCK     (8000000UL)
/** @} */

/**
 * @name   Timer configuration
 *
 * ATTENTION RIOT Timer 0 is used for Xtimer which is system Timer
 *
 * The timer driver only supports the four 16-bit timers
 * (Timer1, TST, Timer3, Timer4, Timer5), so those are the Timer we can use here.
 * Timer 1, TST, PORT B5/6/7 Out, Port D4/6 In,  Analog Comparator Input Capture, Output Compare Modulator, PWM
 * Timer 3, TST, PORT E3/4/5 Out, Port E/6/7 In, Input or Output Compare and PWM Output
 * Timer 4, TST, It can not be connected to any I/O Pin,
 * Timer 5, TST, It can not be connected to any I/O Pin,
 *
 * Using Atmel Timer 4 and 5 seems to be the best choice
 * Using Atmel Timer 4 as Xtimer
 * and Atmel Timer 5 as timer available for the the application seems to be the best choice,
 * as the special functions of the other timer are not lost.
 * Atmel Timer1 to be used as PWM timer for RGB LED
 * @{
 */
#define TIMER_NUMOF         (3U)

#define TIMER_0             MEGA_TIMER4
#define TIMER_0_MASK        &TIMSK4
#define TIMER_0_FLAG        &TIFR4
#define TIMER_0_ISRA        TIMER4_COMPA_vect
#define TIMER_0_ISRB        TIMER4_COMPB_vect
#define TIMER_0_ISRC        TIMER4_COMPC_vect

#define TIMER_1             MEGA_TIMER5
#define TIMER_1_MASK        &TIMSK5
#define TIMER_1_FLAG        &TIFR5
#define TIMER_1_ISRA        TIMER5_COMPA_vect
#define TIMER_1_ISRB        TIMER5_COMPB_vect
#define TIMER_1_ISRC        TIMER5_COMPC_vect

#define TIMER_2             MEGA_TIMER1
#define TIMER_2_MASK        &TIMSK1
#define TIMER_2_FLAG        &TIFR1
#define TIMER_2_ISRA        TIMER1_COMPA_vect
#define TIMER_2_ISRB        TIMER1_COMPB_vect
#define TIMER_2_ISRC        TIMER1_COMPC_vect
/** @} */

/**
 * @name   UART configuration
 *
 * The UART devices have fixed pin mappings, so all we need to do, is to specify
 * which devices we would like to use and their corresponding RX interrupts. See
 * the reference manual for the fixed pin mapping.
 *
 * @{
 */
#define UART_NUMOF          (2U)

/* UART0 is used for stdio */
#define UART_0              MEGA_UART0
#define UART_0_ISR          USART0_RX_vect

#define UART_1              MEGA_UART1
#define UART_1_ISR          USART1_RX_vect
/** @} */

/**
 * @name SPI configuration
 *
 * The atmega256rfr has only one hardware SPI with fixed pin configuration, so all
 * we can do here, is to enable or disable it.
 *
 * PINS SS  SCK MOSI MISO
 *      PB0 PB1 PB2  PB3
 * @{
 */
#define SPI_NUMOF           (1U)           /* set to 0 to disable SPI */
/** @} */

/**
 * @name    I2C configuration
 * @{
 */
#define I2C_BUS_SPEED       I2C_SPEED_FAST
#define I2C_NUMOF           (1U)
/** @} */

/**
 * @name ADC Configuration
 * @{
 */
#define ADC_NUMOF           (8U)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* PERIPH_CONF_H */
