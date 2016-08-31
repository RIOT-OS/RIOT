/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 *               2014 Hinnerk van Bruinehsen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_arduino-mega2560
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the Arduino Mega 2560 board
 *
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_CONF_H_
#define PERIPH_CONF_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Clock configuration
 * @{
 */
#define CLOCK_CORECLOCK     (16000000L)
/** @} */

/**
 * @brief   Timer configuration
 *
 * The timer driver only supports the four 16-bit timers (Timer1, Timer3,
 * Timer4, Timer5), so those are the only onces we can use here.
 *
 * @{
 */
#define TIMER_NUMOF         (2U)

#define TIMER_0             MEGA_TIMER1
#define TIMER_0_MASK        &TIMSK1
#define TIMER_0_FLAG        &TIFR1
#define TIMER_0_ISRA        TIMER1_COMPA_vect
#define TIMER_0_ISRB        TIMER1_COMPB_vect
#define TIMER_0_ISRC        TIMER1_COMPC_vect

#define TIMER_1             MEGA_TIMER4
#define TIMER_1_MASK        &TIMSK4
#define TIMER_1_FLAG        &TIFR4
#define TIMER_1_ISRA        TIMER4_COMPA_vect
#define TIMER_1_ISRB        TIMER4_COMPB_vect
#define TIMER_1_ISRC        TIMER4_COMPC_vect
/** @} */

/**
 * @brief   UART configuration
 *
 * The UART devices have fixed pin mappings, so all we need to do, is to specify
 * which devices we would like to use and their corresponding RX interrupts. See
 * the reference manual for the fixed pin mapping.
 *
 * @{
 */
#define UART_NUMOF          (4U)

#define UART_0              MEGA_UART0
#define UART_0_ISR          USART0_RX_vect

#define UART_1              MEGA_UART1
#define UART_1_ISR          USART1_RX_vect

#define UART_2              MEGA_UART2
#define UART_2_ISR          USART2_RX_vect

#define UART_3              MEGA_UART3
#define UART_3_ISR          USART3_RX_vect
/** @} */

/**
 * @brief SPI configuration
 *
 * The atmega2560 has only one hardware SPI with fixed pin configuration, so all
 * we can do here, is to enable or disable it...
 *
 * The fixed pins used, are:
 * MOSI - PB2 (Arduino pin 51)
 * MISO - PB3 (Arduino pin 50)
 * SCK  - PB1 (Arduino pin 52)
 * SS   - PB0 (Arduino pin 53) -> this pin is configured as output, but not used
 *
 * @{
 */
#define SPI_NUMOF           1           /* set to 0 to disable SPI */
#define SPI_0_EN            1           /* remove once SPI rework is done */
#define MEGA_PRR            PRR0        /* Power Reduction Register is PRR0 */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H_ */
